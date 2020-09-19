$zipCount = (Get-ChildItem -Filter release-1911-gdal-*.zip).count
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
$zipLibsCount = (Get-ChildItem -Filter release-1911-gdal-*-libs.zip).count
if ($zipLibsCount -ne 1) {
    Write-Host "Missing the GIS internals compiled libs and headers zip file."
    Write-Host "Two zip files are required: compiled binaries and compiled libs and headers."
    Pause
    Exit
}

Get-ChildItem temp\* -Force | Remove-Item -force -recurse

Get-ChildItem -Filter release-1911-gdal-*.zip | Expand-Archive -DestinationPath temp\

Write-Host "Licenses for GIS Internals are available in the folder: $PSScriptRoot\temp\"
do { $myInput = (Read-Host 'Continue to use GIS Internals? (Y/N)').ToLower() } while ($myInput -notin @('y','n'))
if ($myInput -ne 'y') {
    Exit
}

Write-Host "Moving files..."
Get-ChildItem GDAL_SDK\v141\bin\* -Force -Exclude !!!*.txt |  Remove-Item -force -recurse
Get-ChildItem GDAL_SDK\v141\include\* -Force -Exclude !!!*.txt |  Remove-Item -force -recurse
Get-ChildItem GDAL_SDK\v141\lib\* -Force -Exclude !!!*.txt |  Remove-Item -force -recurse
Move-Item -Path .\temp\bin\* -Destination .\GDAL_SDK\v141\bin\win32\
Move-Item -Path .\temp\include\* -Destination .\GDAL_SDK\v141\include\win32\
Move-Item -Path .\temp\lib\* -Destination .\GDAL_SDK\v141\lib\win32\

Write-Host "Success."
Write-Host "SupportLibs.sln can now be built."