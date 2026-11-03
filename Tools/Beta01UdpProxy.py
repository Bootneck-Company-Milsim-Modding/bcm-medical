"""Bounded loopback-only UDP impairment fixture. No system network settings are changed."""
import argparse
import heapq
import json
from pathlib import Path
import random
import selectors
import socket
import time

p = argparse.ArgumentParser()
p.add_argument('--listen', type=int, default=23001)
p.add_argument('--server', type=int, default=2001)
p.add_argument('--delay-ms', type=float, default=75)
p.add_argument('--jitter-ms', type=float, default=20)
p.add_argument('--loss', type=float, default=0.01)
p.add_argument('--seconds', type=int, default=180)
p.add_argument('--seed', type=int, default=101)
p.add_argument('--stats', type=Path, required=True)
a = p.parse_args()
if not (0 <= a.loss < 1 and a.delay_ms >= a.jitter_ms >= 0 and 1 <= a.seconds <= 3600):
    p.error('Invalid impairment bounds')
rng = random.Random(a.seed)
sel = selectors.DefaultSelector()
listener = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
listener.bind(('127.0.0.1', a.listen))
listener.setblocking(False)
sel.register(listener, selectors.EVENT_READ, None)
peers = {}
pending = []
sequence = 0
started = time.monotonic()
stats = {'listenPort': a.listen, 'serverPort': a.server, 'delayMs': a.delay_ms,
         'jitterMs': a.jitter_ms, 'lossProbability': a.loss, 'seed': a.seed,
         'received': 0, 'forwarded': 0, 'dropped': 0, 'peers': 0, 'connectionResets': 0, 'errors': []}
def save_stats():
    temporary = a.stats.with_suffix('.tmp')
    temporary.write_text(json.dumps(stats, indent=2)+'\n', encoding='utf-8')
    temporary.replace(a.stats)
next_write = 0
try:
    while time.monotonic() - started < a.seconds:
        now = time.monotonic()
        for key, _ in sel.select(0.005):
            try:
                packet, source = key.fileobj.recvfrom(65535)
            except ConnectionResetError:
                # Windows reports ICMP errors this way after the owned server closes.
                # The native driver independently rejects premature server exit.
                stats['connectionResets'] += 1
                continue
            stats['received'] += 1
            if key.fileobj is listener:
                if source not in peers:
                    peer = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                    peer.bind(('127.0.0.1', 0))
                    peer.connect(('127.0.0.1', a.server))
                    peer.setblocking(False)
                    peers[source] = peer
                    sel.register(peer, selectors.EVENT_READ, source)
                    stats['peers'] = len(peers)
                target, address = peers[source], None
            else:
                target, address = listener, key.data
            if rng.random() < a.loss:
                stats['dropped'] += 1
                continue
            sequence += 1
            due = now + (a.delay_ms + rng.uniform(-a.jitter_ms, a.jitter_ms)) / 1000
            heapq.heappush(pending, (due, sequence, target, address, packet))
        while pending and pending[0][0] <= time.monotonic():
            _, _, target, address, packet = heapq.heappop(pending)
            if address is None:
                target.send(packet)
            else:
                target.sendto(packet, address)
            stats['forwarded'] += 1
        if now >= next_write:
            stats['elapsedSeconds'] = round(now-started, 3)
            save_stats()
            next_write = now + 1
except Exception as exc:
    stats['errors'].append(repr(exc))
    raise
finally:
    stats['elapsedSeconds'] = round(time.monotonic()-started, 3)
    save_stats()
    for peer in peers.values():
        peer.close()
    listener.close()
    sel.close()
