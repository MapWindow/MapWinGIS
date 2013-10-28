REM GDAL include files
xcopy /Y C:\dev\SupportingLibraries\GDAL\include\*.* C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\include\

REM Active Template Library
xcopy /Y /S C:\dev\SupportingLibraries\include\atlhttp C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\include\atlhttp

REM Copy necessary lib files:
rem xcopy /Y C:\dev\SupportingLibraries\GDAL\ERDAS-ECW-JPEG-2000-SDK-5.0\lib\vc90\win32\NCSEcw.lib C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\lib\Win32
xcopy /Y C:\dev\SupportingLibraries\GDAL\LizardTech-SDK\Win32\Lidar_DSDK\lib\lti_lidar_dsdk.lib C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\lib\Win32
xcopy /Y C:\dev\SupportingLibraries\GDAL\LizardTech-SDK\Win32\Raster_DSDK\lib\lti_dsdk.lib C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\lib\Win32
xcopy /Y C:\dev\SupportingLibraries\lib\Win32\geos.lib C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\lib\Win32
xcopy /Y C:\dev\SupportingLibraries\lib\Win32\proj.lib C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\lib\Win32
rem xcopy /Y C:\dev\SupportingLibraries\lib\Win32\SpatialIndex.lib C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\lib\Win32
xcopy /Y C:\dev\SupportingLibraries\lib\Win32\gdal.lib C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\lib\Win32
xcopy /Y C:\dev\SupportingLibraries\lib\Win32\netcdf.lib C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\lib\Win32
xcopy /Y C:\dev\SupportingLibraries\lib\Win32\cqlib.lib C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\lib\Win32


REM Copy necessary dll files:
rem xcopy /Y  C:\dev\SupportingLibraries\GDAL\ERDAS-ECW-JPEG-2000-SDK-5.0\redistributable\vc90\win32\NCSEcw.dll C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\bin\Win32
rem xcopy /Y C:\dev\SupportingLibraries\GDAL\LizardTech-SDK\Win32\Lidar_DSDK\lib\lti_lidar_dsdk.dll C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\bin\Win32
xcopy /Y C:\dev\SupportingLibraries\GDAL\LizardTech-SDK\Win32\Raster_DSDK\lib\lti_dsdk_9.0.dll C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\bin\Win32
xcopy /Y C:\dev\SupportingLibraries\GDAL\LizardTech-SDK\Win32\Raster_DSDK\lib\tbb.dll C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\bin\Win32
xcopy /Y C:\dev\SupportingLibraries\bin\Win32\netcdf.dll C:\dev\MapWinGIS4Dev\MapWinGIS\trunk\SupportingLibraries\bin\Win32
