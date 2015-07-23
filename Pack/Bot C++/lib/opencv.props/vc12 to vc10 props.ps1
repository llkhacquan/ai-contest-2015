(Get-Content vc12.opencv.debug.win32.props) | ForEach-Object { $_ -replace "vc12", "vc10" } | Set-Content vc10.opencv.debug.win32.props

(Get-Content vc12.opencv.debug.x64.props) | ForEach-Object { $_ -replace "vc12", "vc10" } | Set-Content vc10.opencv.debug.x64.props

(Get-Content vc12.opencv.release.win32.props) | ForEach-Object { $_ -replace "vc12", "vc10" } | Set-Content vc10.opencv.release.win32.props

(Get-Content vc12.opencv.release.x64.props) | ForEach-Object { $_ -replace "vc12", "vc10" } | Set-Content vc10.opencv.release.x64.props