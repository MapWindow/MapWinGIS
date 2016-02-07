@setlocal enableextensions
@cd /d "%~dp0"
rem The lines above are from http://www.codeproject.com/Tips/119828/Running-a-bat-file-as-administrator-Correcting-cur.aspx

REM Unregister current version:
regsvr32 /u /s MapWinGIS.ocx

