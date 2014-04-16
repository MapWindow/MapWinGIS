//********************************************************************************************************
//File name: ErrorCodes.cpp
//Description:  Contains error codes.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
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
			return "Unable to retreive COM object from dll";
		case tkFAILED_TO_ALLOCATE_MEMORY:
			return "Failed to allocate memory";
		case tkUNSUPPORTED_FORMAT:
			return "Operation isn't supported for this file format";
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

			//201 - 400 = tkshp
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
			return "Shapefile Function cannot execute in edit mode";
		case tkSHPFILE_NOT_IN_EDIT_MODE:
			return "Shapefile Function cannot execute unless in edit mode";
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
			return "A mulitband grid wasn't opened";
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
		case tkNOT_APPLICABLE_TO_BITMAP:
			return "The method or property is applicable for GDAL-based images only.";
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
			return "Map projection isn't set. See Map.SetGeoProjection method.";
		case tkINVALID_GEOGRAPHIC_COORDINATES:
			return "Invalid geographic coordinates.";

		//1401-1600 = tktinvuc
		case tkVALUE_MUST_BE_2_TO_N:
			return "The value must be 2^n";
		case tkNOT_INITIALIZED:
			return "The viewer is not initialized";
			//1501-1600 = Itkfeature
		
		//1601-1800 = labels
		case tkLABELS_CANT_SYNCHRONIZE:
			return "Failed to synchronize labels with shapefile. Number of the objects differs.";
		case tkLABELS_NOT_SYNCHRONIZE:
			return "The method applicable when labels are synchronized only";
		case tkLABELS_NOT_SAVED:
			return "Failed to load labels as the positions weren't saved";
		
		//1801-2000 = geoprojections
		case tkOGR_NOT_ENOUGH_DATA:
			return "OGR: not enough data";
		case tkOGR_NOT_ENOUGH_MEMORY:
			return "OGR: not enough memory";
		case tkOGR_UNSUPPORTED_GEOMETRY_TYPE:
			return "OGR: unsupported geometry type";
		case tkOGR_UNSUPPORTED_OPERATION:
			return "OGR: unsupported operation";
		case tkOGR_CORRUPT_DATA:
			return "OGR: corrupt data";
		case tkOGR_FAILURE:
			return "OGR: undefined failure";
		case tkOGR_UNSUPPORTED_SRS:
			return "OGR: unsupported spatial reference";
		case tkOGR_INVALID_HANDLE:
			return "OGR: invalid handle";
		case tkFAILED_TO_REPROJECT:
			return "Failed to reproject";
		case tkPROJECTION_NOT_INITIALIZED:
			return "Geoprojection is not initilized";
		case tkPRJ_FILE_EXISTS:
			return "Projection file with such name already exists";
		case tkTRANSFORMATION_NOT_INITIALIZED:
			return "Transformation isn't initialized";
		case tkFAILED_TO_COPY_PROJECTION:
			return "Failed to copy projection";
		case tkPROJECTION_IS_FROZEN:
			return "It's not allowed to change GeoProjection object which is used by map.";

		//2001-2200 = tiles
		case tkINVALID_PROVIDER_ID:
			return "Invalid provider Id";
		case tkCANT_DELETE_DEFAULT_PROVIDER:
			return "Can't delete default provider";
		case tkPROVIDER_ID_RESERVED:
			return "Provider id is reserved";
		case tkPROVIDER_ID_IN_USE:
			return "Provider id is in use";
		case tkINVALID_PROJECTION:
			return "Invalid projection id";
		case tkINVALID_URL:
			return "Invalid URL pattern";
		case tkINVALID_ZOOM_LEVEL:
			return "Invalid zoom level";
		//Default
		default:
			return "Invalid Error Code";
	}
}
