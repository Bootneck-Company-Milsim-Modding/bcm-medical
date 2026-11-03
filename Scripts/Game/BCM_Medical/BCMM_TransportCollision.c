// Temporary collision state belongs to one entity. Physics handles are local:
// the engine does not permit retaining them across frames.
class BCMM_TransportCollision
{
 protected IEntity m_Entity;
 protected int m_Previous;
 protected int m_AppliedMask;
 protected ref array<int> m_Geometry={};
 protected ref array<string> m_GeometryNames={};
 void Apply(IEntity entity,int mask)
 {
  if(!entity)return;
  Physics current=entity.GetPhysics();if(!current){Clear();return;}
  if(entity!=m_Entity || current.GetNumGeoms()!=m_Geometry.Count())
  {
   Clear();m_Entity=entity;m_Previous=current.GetInteractionLayer();
   for(int index=0;index<current.GetNumGeoms();index++)
   {
    m_Geometry.Insert(current.GetGeomInteractionLayer(index));
    m_GeometryNames.Insert(current.GetGeomName(index));
   }
  }
  else
  {
   for(int index=0;index<current.GetNumGeoms();index++)
    if(current.GetGeomInteractionLayer(index)!=m_AppliedMask || current.GetGeomName(index)!=m_GeometryNames[index])
    {
     m_Geometry[index]=current.GetGeomInteractionLayer(index);m_GeometryNames[index]=current.GetGeomName(index);
    }
  }
  m_AppliedMask=mask;
  if(current.GetInteractionLayer()!=mask)current.SetInteractionLayer(mask);
 }
 void Restore(IEntity entity)
 {
  if(entity && entity==m_Entity)
  {
   Physics current=entity.GetPhysics();
   if(current && current.GetNumGeoms()==m_Geometry.Count())
   {
    // Preserve a native lifecycle change instead of overwriting it with an
    // earlier transport snapshot. Restore only geometry we still control.
    for(int index=0;index<m_Geometry.Count();index++)
     if(current.GetGeomName(index)==m_GeometryNames[index] && current.GetGeomInteractionLayer(index)==m_AppliedMask)
      current.SetGeomInteractionLayer(index,m_Geometry[index]);
    if(m_Geometry.IsEmpty() && current.GetInteractionLayer()==m_AppliedMask)current.SetInteractionLayer(m_Previous);
   }
  }
  Clear();
 }
 protected void Clear(){m_Entity=null;m_Geometry.Clear();m_GeometryNames.Clear();}
}
