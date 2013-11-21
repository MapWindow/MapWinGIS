@echo off

call autogen.bat

echo Builds the GEOS project for 32-Bit
nmake /f makefile.vc MSVC_VER=1500 clean
if not %errorlevel% == 0 (
	echo --------------- Cleaning project failed!   Error: %errorlevel%
	goto error_raised
)
if %errorlevel% == 0 echo [%DATE% %Time%] Project was cleaned successful

REM workaround for leftover obj files:
del capi\*.obj

nmake /f makefile.vc MSVC_VER=1500
if not %errorlevel% == 0 (
	echo --------------- Building project failed!   Error: %errorlevel%
	goto error_raised
)
if %errorlevel% == 0 echo [%DATE% %Time%] Project was build successful

nmake /f makefile.vc MSVC_VER=1500 devinstall
if not %errorlevel% == 0 (
	echo --------------- Copying project files failed!   Error: %errorlevel%
	goto error_raised
)
if %errorlevel% == 0 echo [%DATE% %Time%] Project files were copied successful


echo This script was successful. The project has been made.

goto eof

:error_raised
echo This script was unsuccessful

:eof