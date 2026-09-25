Add-Type -AssemblyName System.Drawing

function New-WizardImage {
    param([int]$W, [int]$H, [string]$OutPath, [bool]$Small)

    $bmp = New-Object System.Drawing.Bitmap $W, $H, ([System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias

    $rect = New-Object System.Drawing.Rectangle 0, 0, $W, $H
    $c1 = [System.Drawing.Color]::FromArgb(255, 8, 14, 32)
    $c2 = [System.Drawing.Color]::FromArgb(255, 16, 26, 58)
    $mode = [System.Drawing.Drawing2D.LinearGradientMode]::Vertical
    $grad = New-Object System.Drawing.Drawing2D.LinearGradientBrush -ArgumentList @($rect, $c1, $c2, $mode)
    $g.FillRectangle($grad, $rect)

    $cx = [int]($W * 0.5)
    $cy = if ($Small) { [int]($H * 0.50) } else { [int]($H * 0.25) }
    $r  = if ($Small) { [int]($W * 0.20) } else { [int]($W * 0.24) }

    for ($i = 6; $i -ge 1; $i--) {
        $rr = $r + $i * ($r * 0.30)
        $a  = [int](6 + (7 - $i) * 4)
        $b = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb($a, 255, 232, 178))
        $g.FillEllipse($b, ($cx - $rr), ($cy - $rr), (2 * $rr), (2 * $rr))
    }

    $mr = New-Object System.Drawing.Rectangle ($cx - $r), ($cy - $r), (2 * $r), (2 * $r)
    $p1 = New-Object System.Drawing.Point (($cx - $r)), ($cy - $r)
    $p2 = New-Object System.Drawing.Point (($cx + $r)), ($cy + $r)
    $mc1 = [System.Drawing.Color]::FromArgb(255, 255, 250, 235)
    $mc2 = [System.Drawing.Color]::FromArgb(255, 255, 208, 108)
    $mb = New-Object System.Drawing.Drawing2D.LinearGradientBrush -ArgumentList @($p1, $p2, $mc1, $mc2)
    $g.FillEllipse($mb, $mr)

    $cr = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(46, 232, 188, 118))
    $g.FillEllipse($cr, ($cx - [int]($r*0.55)), ($cy - [int]($r*0.45)), [int]($r*0.40), [int]($r*0.32))
    $g.FillEllipse($cr, ($cx + [int]($r*0.05)), ($cy + [int]($r*0.00)), [int]($r*0.50), [int]($r*0.40))
    $g.FillEllipse($cr, ($cx - [int]($r*0.10)), ($cy + [int]($r*0.35)), [int]($r*0.26), [int]($r*0.20))

    $rnd = New-Object System.Random 20260925
    for ($i = 0; $i -lt 34; $i++) {
        $sx = $rnd.Next(4, $W - 4)
        $sy = $rnd.Next(4, $H - 4)
        $dx = $sx - $cx; $dy = $sy - $cy
        if ([Math]::Sqrt($dx*$dx + $dy*$dy) -lt ($r * 1.6)) { continue }
        $sr = $rnd.Next(1, 3)
        $sa = $rnd.Next(90, 236)
        $sb = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb($sa, 255, 255, 255))
        $g.FillEllipse($sb, $sx, $sy, $sr, $sr)
    }

    if (-not $Small) {
        $hill = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 5, 11, 27))
        $baseY = [int]($H * 0.80)
        $pts = New-Object System.Collections.Generic.List[System.Drawing.Point]
        $pts.Add((New-Object System.Drawing.Point 0, $baseY))
        $seed = 7
        $x = 0
        while ($x -lt $W) {
            $seed = ($seed * 1103515245 + 12345) -band 0x7FFFFFFF
            $peak = $baseY - 14 - ($seed % 26)
            $pts.Add((New-Object System.Drawing.Point ($x + [int]($W * 0.14)), $peak))
            $x += [int]($W * 0.28)
        }
        $pts.Add((New-Object System.Drawing.Point $W, $baseY))
        $pts.Add((New-Object System.Drawing.Point $W, $H))
        $pts.Add((New-Object System.Drawing.Point 0, $H))
        $g.FillPolygon($hill, $pts.ToArray())

        $refl = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(38, 255, 220, 150))
        for ($i = 0; $i -lt 9; $i++) {
            $yy = $baseY + 12 + $i * 6
            $ww = [int]($r * 1.5 * (1 - $i / 10.0))
            if ($ww -lt 4) { continue }
            $g.FillRectangle($refl, ($cx - [int]($ww/2)), $yy, $ww, 2)
        }

        # Chinese strings via Unicode escapes: 閻庣懓鐏氬﹢鈧柤璇″灠椤?/ 濞?缂?閹?闁?        $title = [char]0x5BB5 + [char]0x6708 + [char]0x826F + [char]0x5BB5
        $sub   = [char]0x4E2D + ' ' + [char]0x79CB + ' ' + [char]0x5E86 + ' ' + [char]0x5178

        $fontTitle = New-Object System.Drawing.Font ((New-Object System.Drawing.FontFamily 'Microsoft YaHei')), 15, ([System.Drawing.FontStyle]::Bold), ([System.Drawing.GraphicsUnit]::Pixel)
        $fontSub   = New-Object System.Drawing.Font ((New-Object System.Drawing.FontFamily 'Microsoft YaHei')), 10, ([System.Drawing.FontStyle]::Regular), ([System.Drawing.GraphicsUnit]::Pixel)
        $sf = New-Object System.Drawing.StringFormat
        $sf.Alignment = [System.Drawing.StringAlignment]::Center
        $titleBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 255, 236, 196))
        $subBrush   = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(210, 214, 226, 255))
        $g.DrawString($title, $fontTitle, $titleBrush, (New-Object System.Drawing.PointF ([float]$cx), ([float]([int]($H * 0.50)))), $sf)
        $g.DrawString($sub, $fontSub, $subBrush, (New-Object System.Drawing.PointF ([float]$cx), ([float]([int]($H * 0.50) + 22))), $sf)
    }

    $g.Dispose()
    $bmp.Save($OutPath, [System.Drawing.Imaging.ImageFormat]::Bmp)
    $bmp.Dispose()
    Write-Output "Wrote $OutPath"
}

$out = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) '.'
New-WizardImage -W 164 -H 314 -OutPath (Join-Path $out 'wizard.bmp')       -Small $false
New-WizardImage -W 55  -H 55  -OutPath (Join-Path $out 'wizard_small.bmp') -Small $true