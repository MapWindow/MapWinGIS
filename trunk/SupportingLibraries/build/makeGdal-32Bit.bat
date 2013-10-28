@echo off

echo Builds the GDAL project for 32-Bit
nmake /f makefile.vc MSVC_VER=1500 clean
if not %errorlevel% == 0 (
	echo --------------- Cleaning project failed!   Error: %errorlevel%
	goto error_raised
)
if %errorlevel% == 0 echo [%DATE% %Time%] Project was cleaned successful

REM delete copied binaries:
del /Q C:\dev\SupportingLibraries\GDAL\bin\Win32\gdal*.*
del /Q C:\dev\SupportingLibraries\GDAL\bin\Win32\ogr*.*
del /Q C:\dev\SupportingLibraries\GDAL\bin\Win32\nearblack.exe
del /Q C:\dev\SupportingLibraries\GDAL\bin\Win32\testepsg.exe

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

REM Copy additional dlls:
rem Not available for VS2008: xcopy /Y C:\dev\SupportingLibraries\GDAL\LizardTech-SDK\Win32\Lidar_DSDK\lib\lti_lidar_dsdk.dll C:\dev\SupportingLibraries\bin\Win32
xcopy /Y C:\dev\SupportingLibraries\GDAL\LizardTech-SDK\Win32\Raster_DSDK\lib\lti_dsdk_9.0.dll C:\dev\SupportingLibraries\bin\Win32
xcopy /Y C:\dev\SupportingLibraries\GDAL\LizardTech-SDK\Win32\Raster_DSDK\lib\tbb.dll C:\dev\SupportingLibraries\bin\Win32
REM Not needed anymore, statically linked: xcopy /Y  C:\dev\SupportingLibraries\GDAL\ERDAS-ECW-JPEG-2000-SDK-5.0\redistributable\vc90\win32\NCSEcw.dll C:\dev\SupportingLibraries\bin\Win32

echo This script was successful. The project has been made.

goto eof

:error_raised
echo This script was unsuccessful

:eof