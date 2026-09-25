Add-Type -AssemblyName System.Drawing

$size = 256
$bmp = New-Object System.Drawing.Bitmap $size, $size
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
$g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
$g.Clear([System.Drawing.Color]::Transparent)

# ---- 夜空圆底（墨青蓝） ----
$bg = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 10, 18, 40))
$g.FillEllipse($bg, 6, 6, 244, 244)

# ---- 微弱外光晕 ----
$halo = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(55, 255, 224, 160))
$g.FillEllipse($halo, 52, 44, 152, 152)
$halo2 = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(40, 255, 214, 130))
$g.FillEllipse($halo2, 66, 58, 124, 124)

# ---- 满月（暖金渐变） ----
$moonRect = New-Object System.Drawing.Rectangle 86, 76, 104, 104
$moonBrush = New-Object System.Drawing.Drawing2D.LinearGradientBrush (
    (New-Object System.Drawing.Point 90, 80),
    (New-Object System.Drawing.Point 190, 180),
    [System.Drawing.Color]::FromArgb(255, 255, 248, 231),
    [System.Drawing.Color]::FromArgb(255, 255, 211, 110))
$g.FillEllipse($moonBrush, $moonRect)

# ---- 月面纹理（淡淡环形山） ----
$crater = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(50, 233, 190, 120))
$g.FillEllipse($crater, 112, 102, 26, 20)
$g.FillEllipse($crater, 146, 130, 34, 26)
$g.FillEllipse($crater, 130, 152, 18, 14)
$g.FillEllipse($crater, 164, 100, 12, 10)

# ---- 星光 ----
function Draw-Star($cx, $cy, $r, $alpha) {
    $brush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb($alpha, 255, 255, 255))
    $g.FillEllipse($brush, $cx - $r, $cy - $r, 2 * $r, 2 * $r)
    # 十字光芒
    $pen = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb($alpha, 255, 255, 255)), 1.6
    $g.DrawLine($pen, $cx - $r * 2.2, $cy, $cx + $r * 2.2, $cy)
    $g.DrawLine($pen, $cx, $cy - $r * 2.2, $cx, $cy + $r * 2.2)
}
Draw-Star 54 66 3.2 230
Draw-Star 208 58 2.6 200
Draw-Star 196 176 3.0 220
Draw-Star 60 178 2.4 190
Draw-Star 130 34 2.0 180
Draw-Star 226 122 2.0 170

# ---- 底部剪影（远山 / 屋脊） ----
$hill = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 4, 10, 26))
$pts = [System.Drawing.Point[]]@(
    (New-Object System.Drawing.Point 6, 224),
    (New-Object System.Drawing.Point 40, 196),
    (New-Object System.Drawing.Point 78, 220),
    (New-Object System.Drawing.Point 120, 190),
    (New-Object System.Drawing.Point 164, 218),
    (New-Object System.Drawing.Point 200, 200),
    (New-Object System.Drawing.Point 250, 226),
    (New-Object System.Drawing.Point 250, 250),
    (New-Object System.Drawing.Point 6, 250)
)
$g.FillPolygon($hill, $pts)

$g.Dispose()

$work = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) 'icon_sizes'
New-Item -ItemType Directory -Force -Path $work | Out-Null
$sizes = 16, 24, 32, 48, 64, 128, 256
$pngPaths = @()
foreach ($s in $sizes) {
    $small = New-Object System.Drawing.Bitmap $bmp, $s, $s
    $p = Join-Path $work ("icon_{0}.png" -f $s)
    $small.Save($p, [System.Drawing.Imaging.ImageFormat]::Png)
    $small.Dispose()
    $pngPaths += $p
}
$bmp.Dispose()

# ---- 封装 ICO（PNG 压缩条目，Vista+） ----
function New-IconFromPngs {
    param([string[]]$PngPaths, [string]$OutPath)
    $entries = @()
    $dataOffset = 6 + 16 * $PngPaths.Count
    foreach ($p in $PngPaths) {
        $bytes = [System.IO.File]::ReadAllBytes($p)
        $size = [System.Drawing.Image]::FromFile($p).Width
        $entries += [pscustomobject]@{ Size = $size; Bytes = $bytes; Offset = $dataOffset }
        $dataOffset += $bytes.Length
    }
    $ms = New-Object System.IO.MemoryStream
    $bw = New-Object System.IO.BinaryWriter $ms
    $bw.Write([uint16]0); $bw.Write([uint16]1); $bw.Write([uint16]$entries.Count)
    foreach ($e in $entries) {
        $w = if ($e.Size -ge 256) { 0 } else { $e.Size }
        $bw.Write([byte]$w); $bw.Write([byte]$w); $bw.Write([byte]0); $bw.Write([byte]0)
        $bw.Write([uint16]1); $bw.Write([uint16]32)
        $bw.Write([uint32]$e.Bytes.Length); $bw.Write([uint32]$e.Offset)
    }
    foreach ($e in $entries) { $bw.Write($e.Bytes) }
    $bw.Flush()
    [System.IO.File]::WriteAllBytes($OutPath, $ms.ToArray())
    $ms.Dispose(); $bw.Dispose()
}

New-IconFromPngs $pngPaths (Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) 'app.ico')
Write-Output "ICO created"
