/// \page overview Library Overview
/// 
/// \section overview_zero I. Specification.
/// <table border = "0">
/// <tr><td>Open-source:</td><td>yes</td></tr>
/// <tr><td>Source language:</td><td>unmanaged C++</td></tr>
/// <tr><td>Technology:</td><td>ActiveX control and COM classes</td></tr>
/// <tr><td>Platform:</td><td>Microsoft Windows (Win32 or Win64)</td></tr>
/// <tr><td>License: </td><td>Mozilla Public License v.1.1. (http://www.mozilla.org/MPL/)</td></tr>
/// <tr><td>Functionality:</td><td>data visualization and geoprocessing</td></tr>
/// <tr><td>Intended use:</td><td>desktop applications with GIS functionality</td></tr>
/// <tr><td>Homepage:</td><td>http://mapwingis.codeplex.com</td></tr>
/// </table>
/// 
/// \section overview_nine II. Registration.
/// As any COM server MapWinGIS requires registration in Windows to make its classes available to clients.
/// MapWinGIS installation package performs this task. Alternatively to register or unregister the library, 
/// <b>regsvr32</b> command line utility can be used. \n\n
/// The prerequisite for successful registration is the presence of all the 
/// external dlls, either in the same folder with MapWinGIS or in the <a href = "http://msdn.microsoft.com/en-us/library/windows/desktop/ms682586(v=vs.85).aspx">
/// other search locations.</a> Use <a href = "http://www.dependencywalker.com/">Dependency Walker</a> to find out the missing dependencies.  \n
/// 
/// \section overview_one III. The list of supported data formats.
/// <table>
/// <tr>
/// <th>%Vector</th>
/// <th>Raster</th>
/// <th>%Grid</th>
/// </tr>
/// <tr>
/// <td valign = "top">ESRI Shapefile (.shp)</td>
/// <td valign = "top">ASCII %Grid Images (.asc)\n
/// Binary Terrain Images (.bt)\n
/// BIL (ESRI HDR/BIL Images) (.bil)\n
/// Bitmap Images (.bmp)\n
/// Dem Images (.dem)\n
/// ECW Images (.ecw)\n
/// Erdas Imagine Images (.img)\n
/// GIF Images (.gif)\n
/// PC Raster Images (.map)\n
/// JPEG2000 Images (.jp2)\n
/// JPEG Images (.jpg)\n
/// SID (MrSID Images) (.sid)\n
/// PNG Images (.pgm;.pnm;.png;.ppm)\n
/// CADRG (.LF2)\n
/// BSB/KAP (.kap)\n
/// Tagged %Image File Format (.tif)\n</td>
/// <td valign = "top">
/// ASCII Text (ESRI Ascii %Grid) (.asc, .arc)\n
///	BIL (ESRI HDR/BIL Images) (.bil)\n
///	DTED Elevation (.dhm, .dt0, .dt1)\n
///	ERDAS Imagine (.img)\n
///	ESRI FLT (.flt)\n
///	ESRI %Grid (sta.adf)\n
///	GeoTIFF (.tif)\n
///	PAux (PCI .aux Labelled) (.aux)\n
///	PIX (PCIDSK Database File) (.pix)\n
///	USGS ASCII DEM (.dem)\n
///	USGS SDTS 30m (.ddf)\n
///	USU Binary (.bgd)\n
/// </td>
/// </tr>
/// </table>
/// <a href="www.gdal.org">GDAL library</a> is used as a major provider of raster and grid data. 
/// Check http://www.gdal.org for the detailed information about the supported formats.\n
/// 
/// \section overview_two IV. External libraries.
/// MapWinGIS depends on a number of libraries. They can be linked either:
/// - statically (included in the file);
/// - dynamically (called in runtime).
/// .
/// In 4.8 an effort was made to link all the major dependencies statically, which led to the larger size of binary file but easier deployment policies.\n\n
/// Here are some major external libraries.
/// <table>
/// <tr><th>Library</th><th>Description</th><th>Home page</th><th>Default linking (v.4.8)</th></tr>
/// <tr><td>GDAL</td><td>Supports raster and image formats, XML serialization.</td><td>http://www.gdal.org</td><td>static</td></tr>
/// <tr><td>GEOS</td><td>Geoprocessing operations for vector data.</td><td>http://geos.osgeo.org</td><td>static</td></tr>
/// <tr><td>PROJ.4</td><td>Reprojection of data.</td><td>http://proj.osgeo.org</td><td>static</td></tr>
/// <tr><td>Libspatialindex</td><td>The file-based spatial index for ESRI shapefiles.</td><td>http://libspatialindex.github.com/</td><td>static</td></tr>
/// </table>
/// The binary versions of dlls can be found on the home pages of corresponding products or 
/// <a href = "http://svn.mapwindow.org/svnroot/SupportLibraries/bin/"> here</a>. \n
/// For those who want to build MapWinGIS from the source, the .lib files for the external libraries 
/// can be downloaded <a href = "http://svn.mapwindow.org/svnroot/SupportLibraries/lib">here</a>. \n
/// 
