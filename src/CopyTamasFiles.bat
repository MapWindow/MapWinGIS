@echo off
REM *************************************************************
REM * Copy files from Tamas to bin folder of MapWinGIS          *
REM * But only the needed files                                 *
REM *                                                           *
REM * Usage CopyTamasFiles from_dir to_dir                      *       
REM *                                                           *
REM * Paul Meems, March 2014                                    * 
REM * Paul Meems, update for ecw dll, June 2015                 *
REM * Paul Meems, update for ecw dll to v5.3, Aug 2017          *
REM * Paul Meems, update for xerces and lti_dsdk dll, Aug 2018  *
REM * Paul Meems, update for GDAL v3+, Jan 2022                 *
REM * Usage to test:                                            *
REM * CopyTamasFiles.bat D:\dev\MapwinGIS\GitHub\support\GDAL_SDK\v140\bin\win32 D:\dev\MapwinGIS\GitHub\src\bin\Win32\
REM *************************************************************

set _from_dir=%1
set _to_dir=%2
if '%_from_dir%'=='' if '%_to_dir%'=='' GOTO usage

REM Copy gdal plugins:
FOR %%G IN (gdal_MrSID.dll gdal_netCDF.dll gdal_HDF4.dll gdal_HDF4Image.dll gdal_HDF5.dll gdal_HDF5Image.dll gdal_ECW_JP2ECW.dll gdal_MG4Lidar.dll) DO (
	IF EXIST %_from_dir%\gdal\plugins\%%G (
    xcopy /v /c /r /y %_from_dir%\gdal\plugins\%%G  %_to_dir%gdalplugins\
	)
)

REM Copy gdal plugins:
REM FOR %%G IN (gdal_ECW_JP2ECW.dll ogr_PG.dll) DO (
REM	IF EXIST %_from_dir%\gdal\plugins-optional\%%G (
REM    xcopy /v /c /r /y %_from_dir%\gdal\plugins-optional\%%G  %_to_dir%gdalplugins\
REM	)
REM )

REM Copy PROJ4 data:
xcopy /v /c /r /y %_from_dir%\gdal-data\*.* %_to_dir%gdal-data\
REM xcopy /v /c /r /y %_from_dir%\proj\SHARE\*.* %_to_dir%..\PROJ_NAD\

REM Copy Proj7 data. TODO: Check if copied to correct location:
xcopy /v /c /r /y %_from_dir%\proj7\share\*.* %_to_dir%proj7\share\

REM Copy needed Tamas binaries:
FOR %%G IN (cfitsio.dll freexl.dll geos.dll geos_c.dll hdf.dll hdf5.dll hdf5_hl.dll hdf5_cpp.dll hdf5_hl_cpp.dll libcrypto-1_1.dll libcrypto-1_1-x64.dll 
			libcurl.dll libexpat.dll tiff.dll tiffxx.dll ogdi.dll mfhdf.dll  
            iconv-2.dll libmysql.dll libpng16.dll libpq.dll libssl-1_1.dll libssl-1_1-x64.dll libxml2.dll lti_lidar_dsdk_1.1.dll netcdf.dll   
            openjp2.dll spatialite.dll sqlite3.dll szip.dll tbb.dll xdr.dll zlib.dll  
            NCSEcw.dll) DO (
	IF EXIST %_from_dir%\%%G (
    xcopy /v /c /r /y %_from_dir%\%%G  %_to_dir%
	)
)

REM gdal contains the version number, so use a wildcard:
del /f /q %_to_dir%\gdal*.dll
xcopy /v /c /r /y %_from_dir%\gdal3*.dll  %_to_dir%

REM xerces contains a version number, so use a wildcard:
del /f /q %_to_dir%\xerces-c*.dll
xcopy /v /c /r /y %_from_dir%\xerces-c*.dll  %_to_dir%

REM lti_dsdk contains a version number, so use a wildcard:
del /f /q %_to_dir%\lti_dsdk*.dll
xcopy /v /c /r /y %_from_dir%\lti_dsdk*.dll  %_to_dir%

REM Updates for GDAL v3:
del /f /q %_to_dir%\gdal3*.dll
xcopy /v /c /r /y %_from_dir%\gdal3*.dll  %_to_dir%
del /f /q %_to_dir%\proj_7*.dll
xcopy /v /c /r /y %_from_dir%\proj_7*.dll  %_to_dir%

REM Copy licenses:
xcopy /v /c /r /y %_from_dir%\..\..\..\licenses\*.rtf %_to_dir%..\Licenses\

 
GOTO EOF

:usage
echo incorrect usage:
echo Use CopyTamasFiles from_dir to_dir  
 
 
:EOF
