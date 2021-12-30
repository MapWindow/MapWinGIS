# win32 binaries
Write-Host "Parsing Win32 files..."
$zipCount = (Get-ChildItem -Filter release-19??-gdal-2-*.zip).count
if ($zipCount -lt 2) {
    Write-Host "Missing GIS internals zip files."
    Write-Host "Two zip files are required: compiled binaries and compiled libs and headers."
    Pause
    Exit
}
if ($zipCount -gt 2) {
    Write-Host "Too many zip files."
    Write-Host "Two zip files are required: compiled binaries and compiled libs and headers."
    Pause
    Exit
}
$zipLibsCount = (Get-ChildItem -Filter release-19??-gdal-2-*-libs.zip).count
if ($zipLibsCount -ne 1) {
    Write-Host "Missing the GIS internals compiled libs and headers zip file."
    Write-Host "Two zip files are required: compiled binaries and compiled libs and headers."
    Pause
    Exit
}

Get-ChildItem temp\* -Force | Remove-Item -force -recurse

Get-ChildItem -Filter release-19??-gdal-2-*.zip | Expand-Archive -DestinationPath temp\

#Write-Host "Licenses for GIS Internals are available in the folder: $PSScriptRoot\temp\"
#do { $myInput = (Read-Host 'Continue to use GIS Internals? (Y/N)').ToLower() } while ($myInput -notin @('y','n'))
#if ($myInput -ne 'y') {
#    Exit
#}

Write-Host "Removing previous files..."
Get-ChildItem GDAL_SDK\v141\bin\win32\ -Force -Exclude !!!*.txt |  Remove-Item -force -recurse
Get-ChildItem GDAL_SDK\v141\include\win32\ -Force -Exclude !!!*.txt |  Remove-Item -force -recurse
Get-ChildItem GDAL_SDK\v141\lib\win32\ -Force -Exclude !!!*.txt |  Remove-Item -force -recurse
Write-Host "Moving files..."
Move-Item -Path .\temp\bin\* -Destination .\GDAL_SDK\v141\bin\win32\
Move-Item -Path .\temp\include\* -Destination .\GDAL_SDK\v141\include\win32\
Move-Item -Path .\temp\lib\* -Destination .\GDAL_SDK\v141\lib\win32\


# TODO: Merge with above code:
# win64 binaries
Write-Host "Parsing x64 files..."
$zipCount = (Get-ChildItem -Filter release-19??-x64-gdal-2-*.zip).count
if ($zipCount -lt 2) {
    Write-Host "Missing GIS internals zip files (x64)."
    Write-Host "Two zip files are required: compiled binaries and compiled libs and headers."
    Pause
    Exit
}
if ($zipCount -gt 2) {
    Write-Host "Too many zip files."
    Write-Host "Two zip files are required: compiled binaries and compiled libs and headers."
    Pause
    Exit
}
$zipLibsCount = (Get-ChildItem -Filter release-19??-x64-gdal-2-*-libs.zip).count
if ($zipLibsCount -ne 1) {
    Write-Host "Missing the GIS internals compiled libs and headers zip file (x64)."
    Write-Host "Two zip files are required: compiled binaries and compiled libs and headers."
    Pause
    Exit
}

Get-ChildItem temp\* -Force | Remove-Item -force -recurse

Get-ChildItem -Filter release-19??-x64-gdal-2-*.zip | Expand-Archive -DestinationPath temp\

Write-Host "Removing previous files..."
Get-ChildItem GDAL_SDK\v141\bin\x64\ -Force -Exclude !!!*.txt |  Remove-Item -force -recurse
Get-ChildItem GDAL_SDK\v141\include\x64\ -Force -Exclude !!!*.txt |  Remove-Item -force -recurse
Get-ChildItem GDAL_SDK\v141\lib\x64\ -Force -Exclude !!!*.txt |  Remove-Item -force -recurse

Write-Host "Moving files..."
Move-Item -Path .\temp\bin\* -Destination .\GDAL_SDK\v141\bin\x64\
Move-Item -Path .\temp\include\* -Destination .\GDAL_SDK\v141\include\x64\
Move-Item -Path .\temp\lib\* -Destination .\GDAL_SDK\v141\lib\x64\


Write-Host "Success."
Write-Host "MapWinGIS.sln can now be built."