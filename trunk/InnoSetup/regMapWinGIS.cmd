@setlocal enableextensions
@cd /d "%~dp0"
rem The lines above are from http://www.codeproject.com/Tips/119828/Running-a-bat-file-as-administrator-Correcting-cur.aspx

REM Unregister any previous versions:
regsvr32 /u /s MapWinGIS.ocx
REM Register current version:
regsvr32 /s MapWinGIS.ocx

REM Set GDAL/PROJ4 environment variables:
setenv -a PROJ_LIB "%cd%\PROJ_NAD"
setenv -a GDAL_DATA "%cd%\gdal-data"

REM Set the gdal-plugins folder:
setenv -a GDAL_DRIVER_PATH "%cd%\gdalplugins"

