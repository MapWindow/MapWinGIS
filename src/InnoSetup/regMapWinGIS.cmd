@setlocal enableextensions
@cd /d "%~dp0"
rem The lines above are from http://www.codeproject.com/Tips/119828/Running-a-bat-file-as-administrator-Correcting-cur.aspx

REM Unregister any previous versions:
regsvr32 /u /s MapWinGIS.ocx
REM Register current version:
regsvr32 /s MapWinGIS.ocx

