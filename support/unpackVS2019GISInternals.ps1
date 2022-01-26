###################################################################
#
# Powershell script to unpack the GDAL zips downloaded from
# https://www.gisinternals.com/release.php
# These files need to be manually downloaded first and saved in
# the support folder, the same folder this script is in
#
# The script expects 4 zip files. 2 for Win32 and 2 for x64
# If you only need 1 set of 2 zip files you can comment the call
# to UnpackGisInternalsZips at the bottom of this script
#
###################################################################

Function UnpackGisInternalsZips{
    Param ([boolean]$Is64Bit)

    # The names and filters for the Win32 zip files:
    $regExZipfileNames = "release-19??-gdal-3*.zip"
    $regExZipfileLibName = "release-19??-gdal-3*-libs.zip"
    $subFolderName = "win32"

    if($Is64Bit) {
        # The names and filters for the x64 zip files:
        $regExZipfileNames = "release-19??-x64-gdal-3*.zip"
        $regExZipfileLibName = "release-19??-x64-gdal-3*-libs.zip"
        $subFolderName = "x64"
    }

    Write-Host "Checking files (x64: $($Is64Bit))..."
    $zipCount = (Get-ChildItem -Filter $regExZipfileNames).count
    if ($zipCount -lt 2) {
        Write-Host "Missing GIS internals zip files (x64: $($Is64Bit))."
        Write-Host "Two zip files are required: compiled binaries and compiled libs and headers."
        Pause
        Exit
    }
    if ($zipCount -gt 2) {
        Write-Host "Too many zip files (x64: $($Is64Bit))."
        Write-Host "Two zip files are required: compiled binaries and compiled libs and headers."
        Pause
        Exit
    }

    $zipLibsCount = (Get-ChildItem -Filter $regExZipfileLibName).count
    if ($zipLibsCount -ne 1) {
        Write-Host "Missing the GIS internals compiled libs and headers zip file (x64: $($Is64Bit))."
        Write-Host "Two zip files are required: compiled binaries and compiled libs and headers."
        Pause
        Exit
    }

    Write-Host "Clearing temp folder..."
    CreateDirIfNeeded("temp")
    Get-ChildItem temp\* -Force | Remove-Item -force -recurse

    Write-Host "Unzipping..."
    Get-ChildItem -Filter $regExZipfileNames | Expand-Archive -DestinationPath temp\

    # Move the files to the correct GDAL_SDK subfolders:
    MoveFiles .\temp\bin\* .\GDAL_SDK\v142\bin\$($subFolderName)\
    MoveFiles .\temp\include\* .\GDAL_SDK\v142\include\$($subFolderName)\
    MoveFiles .\temp\lib\* .\GDAL_SDK\v142\lib\$($subFolderName)\

    Write-Host "Successfully copied the GisInternals files (x64: $($Is64Bit))."    
}

Function CreateDirIfNeeded{
    Param ([string]$FolderName)

    if(![System.IO.Directory]::Exists($FolderName))
    {
        Write-Host "Folder Doesn't Exists"
    
        #PowerShell Create directory:
        New-Item $FolderName -ItemType Directory
    }    
}

Function MoveFiles{
    Param ([string]$Path, [string]$Destination)

    # Extra check, folder should exist:
    CreateDirIfNeeded($Destination)

    Write-Host "Removing previous files..."
    Get-ChildItem $Destination -Force -Exclude !!!*.txt |  Remove-Item -force -recurse

    Write-Host "Moving files..."
    Move-Item -Path $Path -Destination $Destination
}



###################################################################
###################################################################
# Entry point
###################################################################
###################################################################

# Unzip the 2 Win32 zip files:
UnpackGisInternalsZips $False

# Unzip the 2 x64 zip files:
UnpackGisInternalsZips $True

Write-Host "MapWinGIS.sln can now be built."