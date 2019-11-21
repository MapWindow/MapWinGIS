//********************************************************************************************************
//File name: ErrorCodes.cpp
//Description:  Contains error codes.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
//3-28-2005 dpa - Identical to public domain version.
//********************************************************************************************************
# include "stdafx.h"
# include "ErrorCodes.h"
# include "gdal.h"

#ifdef _AFX // Use MFC mem leak facilities for operator new (but only if compiled with MFC :-))

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#endif

const char * ErrorMsg( long ErrorCode )
{	
	switch( ErrorCode )
	{	
		//0 = tkNO_ERROR
		case tkNO_ERROR:
			return "No Error";

		//1 - 200 = Common
		case tkINDEX_OUT_OF_BOUNDS:
			return "Index Out of Bounds";
		case tkUNEXPECTED_NULL_PARAMETER:
			return "Parameter was NULL";
		case tkINVALID_FILE_EXTENSION:
			return "Invalid File Extension";
		case tkINVALID_FILENAME:
			return "Invalid Filename";
		case tkUNRECOVERABLE_ERROR:
			return "Unrecoverable Error";
		case tkFILE_NOT_OPEN:
			return "File Not Open";
		case tkZERO_LENGTH_STRING:
			return "Zero Length String";
		case tkINCORRECT_VARIANT_TYPE:
			return "Incorrect Variant Type";
		case tkINVALID_PARAMETER_VALUE:
			return "Invalid Parameter Value";
		case tkINTERFACE_NOT_SUPPORTED:
			return "Interface Not Supported";
		case tkUNAVAILABLE_IN_DISK_MODE:
			return "Desired procedure requires data to be in memory";
		case tkCANT_OPEN_FILE:
			return "Couldn't open file.";
		case tkUNSUPPORTED_FILE_EXTENSION:
			return "Unsupported file extension";
		case tkCANT_CREATE_FILE:
			return "Couldn't create file";
		case tkINVALID_FILE:
			return "Invalid File";
		case tkINVALID_VARIANT_TYPE:
			return "Invalid VARIANT type";
		case tkOUT_OF_RANGE_0_TO_1:
			return "Value was out of range (0 to 1)";
		case tkCANT_COCREATE_COM_INSTANCE:
			return "Unable to retrieve COM object from DLL.";
		case tkFAILED_TO_ALLOCATE_MEMORY:
			return "Failed to allocate memory";
		case tkUNSUPPORTED_FORMAT:
			return "Unsupported datasource format. It may be caused by GlobalSettings.GdalPluginPath pointing to the wrong folder.";
		case tkPROPERTY_DEPRECATED:
			return "The property you called is deprecated";
		case tkPROPERTY_NOT_IMPLEMENTED:
			return "The property you called is not implemented";
		case tkINVALID_FOR_INMEMORY_OBJECT:
			return "The method isn't applicable to the in-memory object";
		case tkCANT_DELETE_FILE:
			return "Failed to delete a file";
		case tkINVALID_EXPRESSION:
			return "Invalid expression";
		case tkFILE_EXISTS:
			return "File to be written already exists.";
		case tkGDAL_ERROR:
			return CPLGetLastErrorMsg();
		case tkMETHOD_NOT_IMPLEMENTED:
			return "The method you called is not implemented";
		case tkFOLDER_NOT_EXISTS:
			return "Folder doesn't exist";
		case tkFILE_NOT_EXISTS:
			return "File doesn't exist";
		case tkMETHOD_DEPRECATED:
			return "Method deprecated.";
		case tkINVALID_OPEN_STRATEGY:
			return "Invalid open strategy for dataset.";
		case tkSPATIAL_OPERATION_FAILED:
			return "Spatial operation failed";
		case tkFAILED_TO_READ_INPUT_NAMES:
			return "Failed to read the names of input files";
		case tkINVALID_PARAMETERS_ARRAY:
			return "Failed to read array of input parameters";
		
			//201 - 300 = tkshp
		case tkUNSUPPORTED_SHAPEFILE_TYPE:
			return "Unsupported Shapefile Type";
		case tkINCOMPATIBLE_SHAPEFILE_TYPE:
			return "Incompatible Shapefile Type";
		case tkCANT_OPEN_SHP:
			return "Couldn't open .shp file";
		case tkCANT_OPEN_SHX:
			return "Couldn't open .shx file";
		case tkINVALID_SHP_FILE:
			return "Invalid .shp file";
		case tkINVALID_SHX_FILE:
			return "Invalid .shx file";
		case tkSHPFILE_IN_EDIT_MODE:
			return "Shapefile function can not be executed in edit mode";
		case tkSHPFILE_NOT_IN_EDIT_MODE:
			return "Shapefile function can not be executed in non-edit mode";
		case tkCANT_CREATE_SHP:
			return "Couldn't create .shp file";
		case tkCANT_CREATE_SHX:
			return "Couldn't create .shx file";
		case tkSHP_FILE_EXISTS:
			return ".shp file already exists";
		case tkSHX_FILE_EXISTS:
			return ".shx file already exists";
		case tkINCOMPATIBLE_SHAPE_TYPE:
			return "Shape type is incompatible with the Shapefile type";
		case tkPARENT_SHAPEFILE_NOT_EXISTS:
			return "Parent shapefile wasn't set";
		case tkCANT_CONVERT_SHAPE_GEOS:
			return "Can't convert shape to GEOS geometry";
		case tkSHAPEFILE_UNINITIALIZED:
			return "Shapefile wasn't created or opened";
		case tkSHP_READ_VIOLATION:
			return "Can't read while writing shapefile";
		case tkSHP_WRITE_VIOLATION:
			return "Can't write while reading shapefile";
		case tkSELECTION_EMPTY:
			return "No shapes are selected, when selection was expected";
		case tkINVALID_SHAPE:
			return "Invalid shape";
		case tkUNEXPECTED_SHAPE_TYPE:
			return "Unexpected shape type";
		case tkBOUNDS_NOT_INTERSECT:
			return "Bounds of input data don't intersect";
		case tkINVALID_RETURN_TYPE:
			return "Invalid Shapefile type specified as return value";
		case tkSHPFILE_WITH_INVALID_SHAPES:
			return "Shapefile contains invalid shapes";
		case tkCATEGORY_WASNT_FOUND:
			return "Category wasn't found";
		case tkRESULTINGSHPFILE_EMPTY:
			return "Resulting shapefile has no shapes";
		case tkABORTED_ON_INPUT_VALIDATION:
			return "Operation was aborted because of invalid input shapes. Fix invalid shapes in the input or set more loose validation rules. See GlobalSettings.ShapeInputValidationMode.";
		case tkABORTED_ON_OUTPUT_VALIDATION:
			return "No results were returned because of invalid shapes in output. Try to set more loose validation rules. See GlobalSettings.ShapeOutputValidationMode.";
		case tkSHAPEFILE_IS_EMPTY:
			return "Operation wasn't executed because input shapefile has no shapes.";
		case tkFAILED_TO_BUILD_SPATIAL_INDEX:
			return "Failed to build temporary spatial index for operation.";
		case tkINMEMORY_SHAPEFILE_EXPECTED:
			return "Operation is valid for in-memory shapefiles only.";
		case tkNOT_ENOUGH_POINTS_FOR_SHAPE_TYPE:
			return "Shape contains too few points for specified shape type.";
		case tkNO_FIXING_IN_DISK_MODE:
			return "Fixing of shapes for Shapefile.Validate method is available in edit mode only. Mode was switched to ReportWithoutFixing.";
		case tkUNDO_LIST_NO_MAP:
			return "Undo list is not attached to a map.";
		case tkALREADY_WITHIN_BATCH:
			return "Already withing batch for undo list.";
		case tkSHAPE_EDITOR_NO_MAP:
			return "Shape editor is not attached to a map.";
		case tkNOT_ENOUGH_POINTS:
			return "Shape doesn't have enough points for its type.";
		case tkSAME_SUBJECT_AND_OVERLAY_SHAPE:
			return "The same shape was selected both for subject and overlay.";
		case tkNO_LAYER_HANDLE_FOR_SHAPE_EDITOR:
			return "No layer handle is set for shape editor.";
		case tkFAILED_CREATE_BUFFER_PARAMS:
			return "Failed to created buffer parameters.";
		case tkSPLIT_LINEAR_INTERSECTION:
			return "Split line has linear intersection with subject line.";
		case tkCANT_START_BATCH_OPERATION:
			return "New batch operation attempted when the previous one is not finished.";
		case tkEDITOR_OVERLAY_NO_SUBJECT:
			return "Editor must have a subject shape in order to start overlay operation.";
		case tkEDITOR_OVERLAY_ALREADY_STARTED:
			return "Overlay operation has been already started.";
		case tkADD_SHAPE_MODE_ONLY:
			return "Method can be called with cmAddShape cursor only.";
		case tkNO_INTERACTIVE_EDITING:
			return "Shapefile.InteractiveEditing is supposed to be on.";
		case tkEXPRESSION_NO_SHAPEFILE:
			return "Expression can be set only for labels of shapefile layer.";
		case tkAPPEND_MODE_NO_FILE:
			return "Append mode works with disk-based shapefiles in non-edit mode only.";
		
		//301-400 = OGR
		case tkFAILED_TO_OPEN_OGR_LAYER:
			return "Failed to open OGR layer.";
		case tkOGR_DATASOURCE_UNINITIALIZED:
			return "OGR datasource wasn't initialized.";
		case tkOGR_LAYER_UNINITIALIZED:
			return "OGR layer wasn't initialized.";
		case tkINVALID_LAYER_NAME:
			return "Invalid layer name.";
		case tkFAILED_TO_CREATE_OGR_LAYER:
			return "Failed to create OGR layer.";
		case tkUNEXPECTED_OGR_SOURCE_TYPE:
			return "Operation isn't supported for layers of such source type.";
		case tkOGR_QUERY_FAILED:
			return "OGR SQL query has failed.";
		case tkFAILED_TO_OPEN_OGR_DATASOURCE:
			return "Failed to open OGR datasource.";
		case tkNO_OGR_DATA_WAS_LOADED:
			return "No data was loaded for OGR layer.";
		case tkOGR_NO_MODIFICATIONS:
			return "OGR layer has no local modifications to be saved.";
		case tkFID_COLUMN_NOT_FOUND:
			return "Feature ID (FID) column wasn't found.";
		case tkCANT_SAVE_REPROJECTED_GEOMETRIES:
			return "Geometry data was reprojected and can't be saved to the source.";
		case tkOGR_RANDOM_WRITE_NOT_SUPPORTED:
			return "Random write operations aren't supported for this layer. It's either layer was opened without forUpdate flag or driver doesn't support it at all (check GDAL docs).";
		case tkOGR_ABORTED_INVALID_SHAPES:
			return "Operation aborted because of the invalid shapes. Consider changing shape validation mode.";
		case tkOGR_LAYER_TRIMMED:
			return "Number of features for the layer exceeds the limit. Only part of them will be available via OgrLayer.Data property. See GlobalSettings.OgrLayerMaxFeatureCount.";
		case tkNOT_ALLOWED_IN_OGR_DYNAMIC_MODE:
			return "The method is not allowed in dynamic loading mode.";
		case tkOGR_NO_STYLE_TABLE_CREATION:
			return "Creation of style table is not supported for the current driver.";
		case tkOGR_NO_STYLE_FOR_QUERIES:
			return "Styles are supported for temporary OGR layers created from SQL queries.";
		case tkOGR_FAILED_TO_CREATE_STYLE_TABLE:
			return "Failed to create style table in the datasource.";
		case tkOGR_INVALID_FIELD_NAME:
			return "No field with specified name was found in datasource.";
		case tkOGR_NO_SHAPEFILE:
			return "Failed to access underlying shapefile.";
		case tkOGR_STYLE_NOT_FOUND:
			return "OGR style with specified name wasn't found.";
		case tkNOT_UNIQUE_CLASSIFICATION_FOR_STRINGS:
			return "String fields don't support classification types other than unique values.";
		case tkTOO_MANY_CATEGORIES:
			return "The specified number of categories is too large.";
		case tkOGR_DATASOURCE_EMPTY:
			return "OGR datasource has no layers.";

		//401-600 = tkgrd
		case tkGRID_NOT_INITIALIZED:
			return "Grid is not initialized";
		case tkINVALID_DATA_TYPE:
			return "Invalid Data Type";
		case tkINVALID_GRID_FILE_TYPE:
			return "Invalid GridFile Type";
		case tkZERO_ROWS_OR_COLS:
			return "Rows or Columns is zero";
		case tkINCOMPATIBLE_DATA_TYPE:
			return "Incompatible DataType";
		case tkESRI_DLL_NOT_INITIALIZED:
			return "ESRI dll not Initialized";
		case tkESRI_INVALID_BOUNDS:
			return "ESRI ReadBounds Failed";
		case tkESRI_ACCESS_WINDOW_SET:
			return "ESRI AccessWindowSet Failed";
		case tkCANT_ALLOC_MEMORY:
			return "Couldn't allocate memory";
		case tkESRI_LAYER_OPEN:
			return "ESRI LayerOpen Failed";
		case tkESRI_LAYER_CREATE:
			return "ESRI LayerCreate Failed";
		case tkESRI_CANT_DELETE_FILE:
			return "ESRI DeleteLayer Failed";
		case tkSDTS_BAD_FILE_HEADER:
			return "SDTS bad file header";
		case tkGDAL_GRID_NOT_OPENED:
			return "A multitband grid wasn't opened";
		case tkCANT_DISPLAY_WO_PROXY:
			return "Can't display grid without proxy. Check CanDisplayWithoutProxy for the reason. Set Grid.PreferedDisplayMode to gpmAuto to avoid the error.";
		case tkFAILED_TO_SAVE_GRID:
			return "Failed to save grid";

		//601-800 = tkimg
		case tkCANT_WRITE_WORLD_FILE:
			return "Couldn't write the world file.";
		case tkINVALID_WIDTH_OR_HEIGHT:
			return "Width and Height must be greater than zero.";
		case tkINVALID_DY:
			return "dY must be greater than zero.";
		case tkINVALID_DX:
			return "dX must be greater than zero.";
		case tkCANT_CREATE_DDB_BITMAP:
			return "Couldn't create DDB bitmap. Not enough memory.";
		case tkAPPLICABLE_GDAL_ONLY:
			return "The method or property is applicable to GDAL-based datasources only.";
		case tkNOT_APPLICABLE_TO_GDAL:
			return "The method or property isn't applicable to GDAL images.";
		case tkGDAL_DATASET_IS_READONLY:
			return "GDAL dataset was opened in read-only mode.";
		case tkIMAGE_BUFFER_IS_EMPTY:
			return "Image buffer is empty.";
		case tkICON_OR_TEXTURE_TOO_BIG:
			return "Image or texture is too big.";
		case tkFAILED_TO_OBTAIN_DC:
			return "Failed to obtain device context from handle.";
		case tkCANT_DISPLAY_WITHOUT_EXTERNAL_SCHEME:
			return "Can't display datasource without external color scheme. Therefore Image.AllowGridRendering can't be set to false.";
		case tkAT_LEAST_TWO_DATASOURCES_EXPECTED:
			return "At least 2 datasources expected.";
		case tkIMAGES_MUST_HAVE_THE_SAME_SIZE:
			return "Images must have the same size.";
		case tkGDAL_INVALID_SAVE_IMAGE_EXTENSION:
			return "GDAL images can currently be saved in the same format only. Extension of the output file may be not valid for its format.";
		case tkCOPYING_DISK_BASED_BMP_NOT_SUPPORTED:
			return "Saving of disk-based (no in-memory) BMP to another location isn't supported.";
		case tkGDIPLUS_SAVING_AVAILABLE_INRAM:
			return "GDI+ saving is available only for in-RAM images.";

		//801-1000 = tkdbf
		case tkCANT_OPEN_DBF:
			return "Couldn't open .dbf file";
		case tkDBF_IN_EDIT_MODE:
			return "DBF Function cannot execute in edit mode";
		case tkDBF_NOT_IN_EDIT_MODE:
			return "DBF Function cannot execute unless in edit mode";
		case tkDBF_FILE_EXISTS:
			return ".dbf file already exists";
		case tkDBF_FILE_DOES_NOT_EXIST:
			return ".dbf file does not exist";
		case tkCANT_CREATE_DBF:
			return "Couldn't create .dbf file";
		case tkDBF_CANT_ADD_DBF_FIELD:
			return "Couldn't add a field to the .dbf file";
		case tkCANT_CHANGE_FIELD_TYPE:
			return "Can't change field type for the file that was added to the table";
		case tkSAME_FIELD_TYPE_EXPECTED:
			return "Fields of the same type are expected for this operation";
		case tkDBF_RECORDS_SHAPES_MISMATCH:
			return "Number of records in attribute table doesn't match the number of shapes in shapefile.";
		case tkDBF_CANT_WRITE_ROW:
			return "Failed to write DBF record to the disk.";
		case tkDBF_NO_EDIT_MODE_WHEN_APPENDING:
			return "Can't start editing mode for DBF table when append mode is on (call Shapefile.StopAppendMode).";
		case tkDBF_WIDTH_TOO_SMALL:
			return "Can't add the field because the width is too small.";
		case tkDBF_PRECISION_TOO_SMALL:
			return "Can't add the field because the precision is too small.";

		//1001-1200 = tkutils
		case tkOUT_OF_RANGE_0_TO_180:
			return "Value was out of range (0 to 180)";
		case tkOUT_OF_RANGE_M360_TO_360:
			return "Value was out of range (-360 to 360)";
		case tkSHAPEFILE_LARGER_THAN_GRID:
			return "The extents of the grid are smaller than the shapefile's extents";
		case tkCONCAVE_POLYGONS:
			return "Concave polygons require a valid Connection Grid";
		case tkINCOMPATIBLE_DX:
			return "The dX of the Grids is incompatible";
		case tkINCOMPATIBLE_DY:
			return "The dY of the Grids is incompatible";
		case tkINVALID_FINAL_POINT_INDEX:
			return "The FinalPointIndex must be 0 or number of points - 1";
		case tkTOLERANCE_TOO_LARGE:
			return "The Tolerance is larger than the length of a shape";
		case tkNOT_ALIGNED:
			return "The number of shapes is different than the network size";
		case tkINVALID_NODE:
			return "The current node is invalid";
		case tkNODE_AT_OUTLET:
			return "The current node is the outlet";
		case tkNO_NETWORK:
			return "The network does not exist";
		case tkCANT_CHANGE_OUTLET_PARENT:
			return "The outlet's parent cannot be changed";
		case tkNET_LOOP:
			return "A loop exists or would be created by this action";
		case tkMISSING_FIELD:
			return "A necessary field is missing from the dbf";
		case tkINVALID_FIELD:
			return "A necessary field is invalid";
		case tkINVALID_FIELD_VALUE:
			return "A necessary field value is invalid";
		case tkNON_SINGLE_BYTE_PER_BAND:
			return "A raster with 1 byte per pixel per band is expected";
		case tkFAILED_READ_BLOCK:
			return "Failed to read GDAL band block";
		case tkFAILED_WRITE_BLOCK:
			return "Failed to write GDAL band block";
		case tkINPUT_RASTERS_DIFFER:
			return "Input rasters must have the same size, number of bands and data type";
		case tkGRID_MERGE_PROJECTION_MISMATCH:
			return "Projection mismatch on merging grids";

		//1201-1400 = tkmap
		case tkINVALID_LAYER_HANDLE:
			return "Invalid LayerHandle";
		case tkINVALID_DRAW_HANDLE:
			return "Invalid DrawHandle";
		case tkWINDOW_LOCKED:
			return "Window Locked";
		case tkINVALID_LAYER_POSITION:
			return "Invalid Layer Position";
		case  tkINIT_INVALID_DC:
			return "Cannot Initialize: Invalid DC";
		case tkINIT_CANT_SETUP_PIXEL_FORMAT:
			return "Cannot Initialize: Setup Pixel Format";
		case tkINIT_CANT_CREATE_CONTEXT:
			return "Cannot Initialize: Create Context";
		case tkINIT_CANT_MAKE_CURRENT:
			return "Cannot Initialize: Make Current";
		case tkUNEXPECTED_LAYER_TYPE:
			return "Unexpected Layer Type";
		case tkMAP_NOT_INITIALIZED:
			return "Map isn't initialized";
		case tkMAP_INVALID_MAPSTATE:
			return "MapState string is Invalid";
		case tkMAP_MAPSTATE_LAYER_LOAD_FAILED:
			return "Layer specified in MapState string failed to load";
		case tkMAP_PROJECTION_NOT_SET:
			return "Can't perform the requested action, map projection isn't set.";
		case tkINVALID_GEOGRAPHIC_COORDINATES:
			return "Invalid geographic coordinates.";
		case tkNO_INTERACTIVE_SHAPEFILES:
			return "No visible shapefiles with InteractiveEditing property set to true on the map.";

		//1401-1600 = tktinvuc
		case tkVALUE_MUST_BE_2_TO_N:
			return "The value must be 2^n.";
		case tkNOT_INITIALIZED:
			return "The viewer is not initialized.";
			//1501-1600 = Itkfeature
		
		//1601-1800 = labels
		case tkLABELS_CANT_SYNCHRONIZE:
			return "Failed to synchronize labels with shapefile. Number of the objects differs.";
		case tkLABELS_NOT_SYNCHRONIZE:
			return "The method applicable when labels are synchronized only.";
		case tkLABELS_NOT_SAVED:
			return "Failed to load labels as the positions weren't saved.";
		case tkINVALID_FLOAT_NUMBER_FORMAT:
			return "Invalid format string for floating point numbers (see documentation of C printf function for valid formats, e.g. %%g or %%.2f).";
        case tkLABEL_POSITIONING_INVALID:
            return "Invalid label positioning for shape type.";
		
		//1801-2000 = geoprojections
		case tkOGR_NOT_ENOUGH_DATA:
			return "OGR: not enough data.";
		case tkOGR_NOT_ENOUGH_MEMORY:
			return "OGR: not enough memory.";
		case tkOGR_UNSUPPORTED_GEOMETRY_TYPE:
			return "OGR: unsupported geometry type.";
		case tkOGR_UNSUPPORTED_OPERATION:
			return "OGR: unsupported operation.";
		case tkOGR_CORRUPT_DATA:
			return "OGR: corrupt data.";
		case tkOGR_FAILURE:
			return "OGR: undefined failure.";
		case tkOGR_UNSUPPORTED_SRS:
			return "OGR: unsupported spatial reference.";
		case tkOGR_INVALID_HANDLE:
			return "OGR: invalid handle.";
		case tkFAILED_TO_REPROJECT:
			return "Failed to start coordinate transformation.";
		case tkPROJECTION_NOT_INITIALIZED:
			return "Geoprojection is not initialized.";
		case tkPRJ_FILE_EXISTS:
			return "Projection file with such name already exists.";
		case tkTRANSFORMATION_NOT_INITIALIZED:
			return "Transformation isn't initialized.";
		case tkFAILED_TO_COPY_PROJECTION:
			return "Failed to copy projection.";
		case tkPROJECTION_IS_FROZEN:
			return "It's not allowed to change GeoProjection object which is used by map.";
		case tkGEOGRAPHIC_PROJECTION_EXPECTED:
			return "Projection mismatch. Map uses geographic coordinate system while data seems to be outside [-180;180],[-90;90] bounds.";
		case tkMISSING_GEOPROJECTION:
			return "Can't add layer to the map. No metadata about projection can be found.";
		case tkPROJECTION_MISMATCH:
			return "Can't add layer to the map. Layer projection doesn't match map projection.";
		case tkNO_REPROJECTION_FOR_IMAGES:
			return "Can't add layer to the map. Layer projection doesn't match map projection. Automatic transformation isn't supported for images.";
		case tkFAILED_TRANSFORM_WGS84_TO_MAP:
			return "Failed to start transformation from WGS84 to map coordinate system.";
		case tkFAILED_TRANSFORM_MAP_TO_WGS84:
			return "Failed to start transformation from map coordinate system to WGS84.";
		case tkREPROJECTION_TOO_MUCH_SHAPES:
			return "Layer to be reprojected has too many shapes. Consider to open it in native projection or to save reprojection results as a new datasource. "
					"Doing reprojection on the fly can be too costly. This behavior can be changed via GlobalSettings.MaxReprojectionShapeCount property.";

		//2001-2200 = tiles
		case tkINVALID_PROVIDER_ID:
			return "Invalid provider Id.";
		case tkCANT_DELETE_DEFAULT_PROVIDER:
			return "Can't delete default provider.";
		case tkPROVIDER_ID_RESERVED:
			return "Provider id is reserved.";
		case tkPROVIDER_ID_IN_USE:
			return "Provider id is in use.";
		case tkINVALID_PROJECTION:
			return "Invalid projection id.";
		case tkINVALID_URL:
			return "Invalid URL pattern.";
		case tkINVALID_ZOOM_LEVEL:
			return "Invalid zoom level.";
		

		//Default
		default:
			return "Invalid Error Code";
	}
}
