@setlocal enableextensions
@cd /d "%~dp0"
rem The lines above are from http://www.codeproject.com/Tips/119828/Running-a-bat-file-as-administrator-Correcting-cur.aspx

REM Unregister current version:
regsvr32 /u /s MapWinGIS.ocx

REM Unset GDAL/PROJ4 environment variables:
setenv -d PROJ_LIB "%cd%\PROJ_NAD"
setenv -d GDAL_DATA "%cd%\gdal-data"

REM Set the gdal-plugins folder:
setenv -d GDAL_DRIVER_PATH "%cd%\gdalplugins"

