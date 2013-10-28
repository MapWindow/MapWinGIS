@setlocal enableextensions
@cd /d "%~dp0"
rem The lines above are from http://www.codeproject.com/Tips/119828/Running-a-bat-file-as-administrator-Correcting-cur.aspx
regsvr32 /u /s MapWinGIS.ocx
regsvr32 MapWinGIS.ocx
setenv -a PROJ_LIB "%cd%\..\PROJ_NAD"
setenv -a GDAL_DATA "%cd%\..\gdal-data"
