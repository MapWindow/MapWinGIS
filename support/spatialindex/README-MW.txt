This is a static lib version of the SpatialIndex library found at:

http://libspatialindex.org
https://github.com/libspatialindex/libspatialindex

------

To build the libraries simply open the "spatialindex-mw.sln" file using VS2008 and use Batch Build to build both the Win32 and x64 versions.

------

The following changes were made to build as a static lib:

1. Switched "Configuration Type" from "Dynamic Library (.dll)" to "Static Library (.lib)" in VS project properties

2. Removed the SIDX_DLL keyword from include files

3. Adjusted property settings to put LIB files in the proper directories

------

The following changes were made for the MW version of SpatialIndex:

1. Changed the project name to "spatialindex-mw"

2. Changed the index file extensions from "dat" and "idx" to "mwd" and "mwx"
      
   
------
Updating the source code 

1. Download master zip from https://github.com/libspatialindex/libspatialindex

2a. Copy include folder from zip to \support\include\spatialindex overwriting existing files
2b. Copy src folder from zip to \support\spatialindex\src overwriting existing files

3. Changed the index file extensions from "dat" and "idx" to "mwd" and "mwx"
   In \support\spatialindex\src\storagemanager\DiskStorageManager.cc	L53, L161
   
4. Removed the SIDX_DLL keyword from include files