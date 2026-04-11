# 生成描边极简图标：透明底 + 蓝色描边圆角 + 蓝色 A
Add-Type -AssemblyName System.Drawing

function New-OutlineIcon([int]$size) {
    $bmp = New-Object System.Drawing.Bitmap $size, $size
    $bmp.SetResolution(96, 96)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = 'AntiAlias'
    $g.TextRenderingHint = 'AntiAliasGridFit'
    $g.InterpolationMode = 'HighQualityBicubic'
    $g.Clear([System.Drawing.Color]::Transparent)

    $blue = [System.Drawing.Color]::FromArgb(255, 59, 130, 246)
    $strokeW = [Math]::Max(1.5, $size / 16.0)
    $r = [float]($size / 4.0)
    $pad = $strokeW + 1
    $d = $r * 2
    $w = [float]($size - 2 * $pad)
    $h = $w

    # 圆角矩形描边
    $path = New-Object System.Drawing.Drawing2D.GraphicsPath
    $path.AddArc([float]$pad, [float]$pad, $d, $d, 180, 90)
    $path.AddArc([float]($pad + $w - $d), [float]$pad, $d, $d, 270, 90)
    $path.AddArc([float]($pad + $w - $d), [float]($pad + $h - $d), $d, $d, 0, 90)
    $path.AddArc([float]$pad, [float]($pad + $h - $d), $d, $d, 90, 90)
    $path.CloseFigure()

    $pen = New-Object System.Drawing.Pen($blue, $strokeW)
    $g.DrawPath($pen, $path)
    $pen.Dispose()
    $path.Dispose()

    # A 字母
    $fs = $size * 0.48
    $font = New-Object System.Drawing.Font("Segoe UI", $fs, [System.Drawing.FontStyle]::Bold, [System.Drawing.GraphicsUnit]::Pixel)
    $sf = New-Object System.Drawing.StringFormat
    $sf.Alignment = [System.Drawing.StringAlignment]::Center
    $sf.LineAlignment = [System.Drawing.StringAlignment]::Center
    $tbr = New-Object System.Drawing.SolidBrush($blue)
    $g.DrawString("A", $font, $tbr, [float]($size / 2), [float]($size / 2), $sf)

    $font.Dispose(); $tbr.Dispose(); $sf.Dispose(); $g.Dispose()
    return $bmp
}

$sizes = @(16, 32, 48, 256)
$pngData = @()

foreach ($sz in $sizes) {
    $bmp = New-OutlineIcon $sz
    $ms = New-Object System.IO.MemoryStream
    $bmp.Save($ms, [System.Drawing.Imaging.ImageFormat]::Png)
    $pngData += ,($ms.ToArray())
    $ms.Dispose()
    $bmp.Dispose()
}

# ICO 文件格式
$ico = New-Object System.IO.MemoryStream
$bw = New-Object System.IO.BinaryWriter($ico)

$bw.Write([UInt16]0)
$bw.Write([UInt16]1)
$bw.Write([UInt16]$sizes.Count)

$offset = 6 + 16 * $sizes.Count

for ($i = 0; $i -lt $sizes.Count; $i++) {
    $sz = $sizes[$i]
    $bw.Write([byte]$(if ($sz -ge 256) { 0 } else { $sz }))
    $bw.Write([byte]$(if ($sz -ge 256) { 0 } else { $sz }))
    $bw.Write([byte]0)
    $bw.Write([byte]0)
    $bw.Write([UInt16]1)
    $bw.Write([UInt16]32)
    $bw.Write([UInt32]$pngData[$i].Length)
    $bw.Write([UInt32]$offset)
    $offset += $pngData[$i].Length
}

foreach ($data in $pngData) { $bw.Write($data) }

$bw.Flush()
[System.IO.File]::WriteAllBytes("e:\Project\capsbar\app.ico", $ico.ToArray())
$ico.Dispose()

Write-Host "描边极简图标 app.ico 已生成 ($($sizes.Count) sizes)"
