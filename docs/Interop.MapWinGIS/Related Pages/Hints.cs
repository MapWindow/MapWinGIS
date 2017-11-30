#pragma warning disable 1587
/// \page hints Hints
/// 
/// <b>A. COM objects.</b> \n\n
/// MapWinGIS is COM-based, therefore it counts the references of particular objects to determine when they are no longer needed and can be released.
/// -# Environments like .NET or VB6 add and release these references automatically. Calling the operations 
/// explicitly (Marshal.AddRef, Marshal.Release for example) in most cases will cause problems and sometimes even crashes.\n\n
/// -# In the languages like unmanaged C++ the client is responsible for maintaining the number of reference, 
/// so AddRef(), Release() must be called explicitly.\n
/// .
/// Try to avoid the usage of the same COM object as a child to several objects. For example it's a good practice not to insert the same 
/// shape in several shapefiles, unless there is a good reason to do it and you understand the possible effects. In any other case call  
/// Clone (Shape.Clone for example) before the operation.\n
/// \code 
/// // let's copy a shape from one shapefile to another
/// Shapefile sf = some_shapefile;
/// Shapefile sfNew = sf.Clone();
/// 
/// // 1. without Clone()
/// Shape shp = sf.get_Shape(shapeIndex);
/// sfNew.EditAddShape(shp);             // risky, the same instance of shape will be used by two shapefiles
/// 
/// // 2. with Clone()
/// Shape shp = sf.get_Shape(shapeIndex);
/// sfNew.EditAddShape(shp.Clone());     // safe, there are 2 different instances of shape
/// \endcode
/// MapWinGIS objects are not thread-safe. Therefore accessing the same object from several threads should be made with caution especially 
/// when the editing takes place.\n
/// 
/// <b>B. Error handling and progress information.</b> \n\n
/// By design MapWinGIS doesn't throw exceptions to return the information about errors. If an unhandled exception is still thrown,
/// in the most cases it should be treated as a bug and reported to the <a href = "http://mapwingis.codeplex.com/workitem/list/basic">Issue tracker.</a>\n
/// 
/// To notify a client about success or failure of certain method the following approaches are used:
/// -# A boolean return value (for C++ it's VARIANT_BOOL* [out, retval] parameter);
/// -# Null reference in case the method must return some object reference;
/// -# For C++ it's also possible to check HRESULT return values, but it's not recommended as MapWinGIS relies on first 2 approaches.
/// \code
/// string filename = "somefile.shp";
/// Shapefile sf = new Shapefile();
/// 
/// // function returns false on failure
/// bool success = sf.Open(filename, null);
/// if (!success)
/// {
///     Debug.Print(sf.get_ErrorMsg(sf.LastErrorCode));
/// }
/// else
/// {
///     // function returns null pointer on failure
///     Shapefile sfCopy = sf.Clone();
///     if (sfCopy == null)
///     {
///         Debug.Print(sf.get_ErrorMsg(sf.LastErrorCode));
///     }
/// }
/// \endcode
/// 
/// If the execution failed, there are generally 2 ways to get information about the reason of failure.\n\n
/// // 1. Class.LastErrorCode and Class.get_ErrorMessage properties, which are defined for all major classes.
/// If no error took place within this instance of class "No error" string will be return. 
/// Every call of LastErrorCode property will clear the error, i.e. reset it to the "No error" state.
/// \code Shapefile sf = some_shapefile;
/// Shape shp = sf.get_Shape(sf.NumShapes);              // deliberately faulty line; the last index is NumShapes - 1
/// Debug.Print(sf.get_ErrorMessage(sf.LastErrorCode));  // "Index Out of Bounds" error will be reported
/// Debug.Print(sf.get_ErrorMessage(sf.LastErrorCode));  // "No error" will be reported as the error was cleared by previous call
/// \endcode\n
/// // 2. Class.GlobalCallback property. If the object which implements ICallback interface was set to this property
/// the error description will be passed to the ICallback.Error method.
/// 
/// ICallback interface can also be used to report the information about the progress of some lasting task. 
/// \code
/// public void Test()
/// {
///     Shapefile sf = new Shapefile();
///     sf.GlobalCallback = new Callback();
/// }
/// class Callback : ICallback
/// { 
///     public void Error(string KeyOfSender, string ErrorMsg)
///     {
///         Debug.Print("Error reported: " + ErrorMsg);
///     }
///     public void Progress(string KeyOfSender, int Percent, string Message)
///     {
///         Debug.Print(Message + ": " + Percent + "%");
///     }
/// }
/// \endcode
/// 
/// <b> C. Map redraw.</b>\n\n
/// Map control tracks some of the changes of its properties and data layers and trigger redraws to display them. It's usually applicable
/// to all the AxMap members. But changes to the data layers made through API of other classes (like Shapefile or Image) will not be tracked. 
/// Therefore an explicit AxMap.Redraw() call will be needed to display the changes. In general it's a good practice to call redraw explicitly 
/// and not to rely on built-in tracking of state change. \n
/// \code AxMap axMap = map_instance;
/// Shapefile sf = some_shapefile;
/// axMap.AddLayer(sf, true);                // the map will be updated automatically
/// sf.DefaultDrawingOptions.LineWidth = 3;  // the map wasn't be updated
/// axMap.Redraw();                          // the thick lines will be shown only here
/// \endcode
/// AxMap.Refresh rather AxMap.%Redraw should be called when there is need to update only temporary objects on map (so-called "drawing layers")
/// rather then to redraw the whole map. The former operation is fast and can be used for display of objects being dragged atop the map for example.
/// \code AxMap axMap = map_instance;
/// int handle = axMap.NewDrawing(tkScreenReferencedList);
/// axMap.DrawPolygon(arguments);
/// axMap.Refresh();                        // redraw of drawing layer only to see the polygon (fast)
/// axMap.Redraw();                         // complete redraw of the map (slow)
/// \endcode
/// 
/// <b>D. Some aspects of interaction with .NET.</b> \n\n
/// 1. Interop libraries. In order to use MapWinGIS in .NET environment 2 interop assemblies (wrappers) must be generated: AxInteriop.MapWinGIS.dll
/// and Interop.MapWinGIS dll. Visual Studio generates these assemblies automatically in the process of adding AxMap control on the form. Tlbimp.exe and
/// Aximp.exe command line utilities can be used to do the same tasks manually.\n\n
/// 2. Passing colors. The OLE_COLOR data type used by MapWinGIS is mapped to UInt32 in .NET, which doesn't provide immediate means 
/// for specification of colors using System.Drawing.Color class. The following 2 approaches can be used.\n
/// \code
/// Shapefile sf = new Shapefile();
/// 
/// //1. ColorTranslator class can be used to do the job 
/// sf.DefaultDrawingOptions.FillColor = Convert.ToUInt32(ColorTranslator.ToOle(Color.Red));
/// 
/// //2. there is ColorByName method of the Utils class
/// Utils utils = new Utils();
/// sf.DefaultDrawingOptions.FillColor = utils.ColorByName(tkMapColor.Red);
/// \endcode \n
/// 3. Consuming the variant data type. .NET doesn't have data type equivalent to COM VARIANT. Therefore arguments of of variant type
/// are converted to the object data type. Variant parameters can be used in the following scenarios:\n
///     - to return *any* COM class with IDispath interface (see AxMap.get_GetObject);
///     - to return the array of objects (see Shapefile.SelectShapes);
///     - to get or set the values in the attribute table, which can be of either double, integer or string type (see Table.get_CellValue).
///     Use the documentation to find out the data type the output values should be cast to on the .NET side. 
/// .
/// 
/// <b>E. MapWindow 4</b> \n\n
/// MapWindow 4 is the largest application build upon MapWinGIS (http://www.mapwindow.org).
/// -# From version 4.8 (May 2011) MapWinGIS can load MapWindow 4 projects using AxMap.LoadMapState call. 
/// It provides the the fastest way to setup the visualization options for the layers and pass them to custom application. \n\n
/// -# The functionality present in MapWindow can be more or less easily implemented in other custom application based on MapWinGIS. 
/// MapWindow 4 repository is <a href = "http://svn.mapwindow.org/svnroot/MapWindow4Dev/">here.</a>
#pragma warning restore 1587