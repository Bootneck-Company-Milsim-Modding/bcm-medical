param(
    [Parameter(Mandatory = $true)]
    [string]$DataDirectory
)

Add-Type -AssemblyName System.Drawing

function Convert-ToArgbBitmap {
    param([string]$Path)

    $source = [System.Drawing.Bitmap]::FromFile($Path)
    try {
        $result = [System.Drawing.Bitmap]::new(
            $source.Width,
            $source.Height,
            [System.Drawing.Imaging.PixelFormat]::Format32bppArgb
        )
        $graphics = [System.Drawing.Graphics]::FromImage($result)
        try {
            $graphics.DrawImageUnscaled($source, 0, 0)
        }
        finally {
            $graphics.Dispose()
        }

        return $result
    }
    finally {
        $source.Dispose()
    }
}

function Get-BitmapBytes {
    param([System.Drawing.Bitmap]$Bitmap)

    $rectangle = [System.Drawing.Rectangle]::new(0, 0, $Bitmap.Width, $Bitmap.Height)
    $data = $Bitmap.LockBits(
        $rectangle,
        [System.Drawing.Imaging.ImageLockMode]::ReadOnly,
        [System.Drawing.Imaging.PixelFormat]::Format32bppArgb
    )
    try {
        $bytes = [byte[]]::new([Math]::Abs($data.Stride) * $data.Height)
        [System.Runtime.InteropServices.Marshal]::Copy($data.Scan0, $bytes, 0, $bytes.Length)
        return ,$bytes
    }
    finally {
        $Bitmap.UnlockBits($data)
    }
}

function Save-BitmapBytes {
    param(
        [byte[]]$Bytes,
        [int]$Width,
        [int]$Height,
        [string]$Path
    )

    $bitmap = [System.Drawing.Bitmap]::new(
        $Width,
        $Height,
        [System.Drawing.Imaging.PixelFormat]::Format32bppArgb
    )
    try {
        $rectangle = [System.Drawing.Rectangle]::new(0, 0, $Width, $Height)
        $data = $bitmap.LockBits(
            $rectangle,
            [System.Drawing.Imaging.ImageLockMode]::WriteOnly,
            [System.Drawing.Imaging.PixelFormat]::Format32bppArgb
        )
        try {
            [System.Runtime.InteropServices.Marshal]::Copy($Bytes, 0, $data.Scan0, $Bytes.Length)
        }
        finally {
            $bitmap.UnlockBits($data)
        }

        $bitmap.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
    }
    finally {
        $bitmap.Dispose()
    }
}

function New-ReforgerTextureSet {
    param(
        [string]$Prefix,
        [string]$OutputPrefix
    )

    $albedo = Convert-ToArgbBitmap (Join-Path $DataDirectory "${Prefix}_Albedo.png")
    $roughness = Convert-ToArgbBitmap (Join-Path $DataDirectory "${Prefix}_Roughness.png")
    $normal = Convert-ToArgbBitmap (Join-Path $DataDirectory "${Prefix}_Normal.png")
    $metallic = Convert-ToArgbBitmap (Join-Path $DataDirectory "${Prefix}_Metallic.png")
    $ao = Convert-ToArgbBitmap (Join-Path $DataDirectory "${Prefix}_AO.png")

    try {
        $width = $albedo.Width
        $height = $albedo.Height
        foreach ($bitmap in @($roughness, $normal, $metallic, $ao)) {
            if ($bitmap.Width -ne $width -or $bitmap.Height -ne $height) {
                throw "All ${Prefix} source textures must have matching dimensions."
            }
        }

        $albedoBytes = Get-BitmapBytes $albedo
        $roughnessBytes = Get-BitmapBytes $roughness
        $normalBytes = Get-BitmapBytes $normal
        $metallicBytes = Get-BitmapBytes $metallic
        $aoBytes = Get-BitmapBytes $ao
        $bcrBytes = [byte[]]::new($albedoBytes.Length)
        $nmoBytes = [byte[]]::new($albedoBytes.Length)

        for ($index = 0; $index -lt $albedoBytes.Length; $index += 4) {
            # System.Drawing exposes Format32bppArgb memory as B, G, R, A.
            $bcrBytes[$index] = $albedoBytes[$index]
            $bcrBytes[$index + 1] = $albedoBytes[$index + 1]
            $bcrBytes[$index + 2] = $albedoBytes[$index + 2]
            $bcrBytes[$index + 3] = $roughnessBytes[$index + 2]

            $nmoBytes[$index] = $metallicBytes[$index + 2]
            $nmoBytes[$index + 1] = $normalBytes[$index + 1]
            $nmoBytes[$index + 2] = $normalBytes[$index + 2]
            $nmoBytes[$index + 3] = $aoBytes[$index + 2]
        }

        Save-BitmapBytes $bcrBytes $width $height (Join-Path $DataDirectory "${OutputPrefix}_BCR.png")
        Save-BitmapBytes $nmoBytes $width $height (Join-Path $DataDirectory "${OutputPrefix}_NMO.png")
    }
    finally {
        $albedo.Dispose()
        $roughness.Dispose()
        $normal.Dispose()
        $metallic.Dispose()
        $ao.Dispose()
    }
}

New-ReforgerTextureSet -Prefix "BCM_BodyBag" -OutputPrefix "BCM_BodyBag"
New-ReforgerTextureSet -Prefix "BCM_Corpse" -OutputPrefix "BCM_Corpse"
