@echo off

echo Builds the Proj4 project for 32-Bit
nmake /f makefile.vc clean
if not %errorlevel% == 0 (
	echo --------------- Cleaning project failed!   Error: %errorlevel%
	goto error_raised
)
if %errorlevel% == 0 echo [%DATE% %Time%] Project was cleaned successful

nmake /f makefile.vc
if not %errorlevel% == 0 (
	echo --------------- Building project failed!   Error: %errorlevel%
	goto error_raised
)
if %errorlevel% == 0 echo [%DATE% %Time%] Project was build successful

cd nad
nmake /f makefile.vc install-all
if not %errorlevel% == 0 (
	echo --------------- Creating NAD files failed!   Error: %errorlevel%
	goto error_raised
)
if %errorlevel% == 0 echo [%DATE% %Time%] NAD files were created successful

cd ..
nmake /f makefile.vc install-all
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