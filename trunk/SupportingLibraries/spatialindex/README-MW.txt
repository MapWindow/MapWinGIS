This is a static lib version of the SpatialIndex library found at:

http://libspatialindex.org

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