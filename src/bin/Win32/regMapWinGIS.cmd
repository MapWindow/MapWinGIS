@setlocal enableextensions
@cd /d "%~dp0"
rem The lines above are from http://www.codeproject.com/Tips/119828/Running-a-bat-file-as-administrator-Correcting-cur.aspx

REM Unregister any previous versions:
%systemroot%\SysWoW64\regsvr32 /u /s MapWinGIS.ocx
REM Register current version:
%systemroot%\SysWoW64\regsvr32 /s MapWinGIS.ocx

