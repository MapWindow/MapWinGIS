//********************************************************************************************************
//File name: Utils.h
//Description: Declaration of the CUtils.
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
//3-28-2005 dpa - Added include for the grid interpolator.
//********************************************************************************************************

#pragma once
#include "GeometryConverter.h"
#include "GridInterpolate.h"
#include "XRedBlackTree.h"
#include "YRedBlackTree.h"

extern "C"
{	
//# include "gpc.h"
}

struct BreakVal
{	
	double lowVal;
	double highVal;
};

struct CallbackParams
{
	ICallback *cBack;
	const char *sMsg;
};

struct RasterPoint
{	
public:
	RasterPoint()
	{	column = 0;
		row = 0;
	}
	RasterPoint( long c, long r )
	{	column = c;
		row = r;
	}

	long row;
	long column;
};

#define GEOTRSFRM_TOPLEFT_X            0
#define GEOTRSFRM_WE_RES               1
#define GEOTRSFRM_ROTATION_PARAM1      2
#define GEOTRSFRM_TOPLEFT_Y            3
#define GEOTRSFRM_ROTATION_PARAM2      4
#define GEOTRSFRM_NS_RES               5

// CUtils

class ATL_NO_VTABLE CUtils : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUtils, &CLSID_Utils>,
	public IDispatchImpl<IUtils, &IID_IUtils, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CUtils()
	{
		USES_CONVERSION;

		pip_left = 0;
		pip_right = 0;
		pip_top = 0;
		pip_bottom = 0;		

		lastErrorCode = tkNO_ERROR;
		globalCallback = NULL;
		key = A2BSTR("");
			
		BufferA_R = NULL;
		BufferA_G = NULL;
		BufferA_B = NULL;
		BufferB_R = NULL;
		BufferB_G = NULL;
		BufferB_B = NULL;
		BufferLastUsed = 'Z';
		BufferANum = -1;
		BufferBNum = -1;
		rasterDataset = NULL;

		bSubCall = FALSE;
		_tileProjections[0] = NULL;
		_tileProjections[1] = NULL;
	}
	~CUtils()
	{
		for(int i = 0; i < 2; i++)
		{
			if (_tileProjections[i]) {
				_tileProjections[i]->Release();
				_tileProjections[i] = NULL;
			}
		}
		::SysFreeString(key);
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_UTILS)

DECLARE_NOT_AGGREGATABLE(CUtils)

BEGIN_COM_MAP(CUtils)
	COM_INTERFACE_ENTRY(IUtils)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


// IUtils
public:
	STDMETHOD(ClipPolygon)(/*[in]*/ PolygonOperation op, /*[in]*/ IShape * SubjectPolygon, /*[in]*/ IShape * ClipPolygon, /*[out, retval]*/ IShape ** retval);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Area)(/*[in]*/ IShape * Shape, /*[out, retval]*/ double *pVal);
	STDMETHOD(get_Perimeter)(/*[in]*/ IShape * Shape, /*[out, retval]*/ double *pVal);
	STDMETHOD(get_Length)(/*[in]*/ IShape * Shape, /*[out, retval]*/ double *pVal);
	STDMETHOD(RemoveColinearPoints)(/*[in,out]*/ IShapefile * Shapes, /*[in]*/double LinearTolerance, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(GridInterpolateNoData)(/*[in, out]*/ IGrid * Grid, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(GridReplace)(/*[in, out]*/ IGrid * Grid, /*[in]*/ VARIANT OldValue, /*[in]*/ VARIANT NewValue, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(PointInPolygon)(/*[in]*/ IShape * Shape, /*[in]*/IPoint * TestPoint, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(ShapeMerge)(/*[in]*/ IShapefile * Shapes, /*[in]*/long IndexOne, /*[in]*/long IndexTwo, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ IShape ** retval);
	STDMETHOD(GridMerge)(/*[in]*/ VARIANT Grids, /*[in]*/ BSTR MergeFilename, /*[in, optional, defaultvalue(TRUE)]*/ VARIANT_BOOL InRam, /*[in, optional, defaultvalue(UseExtension)]*/ GridFileType GrdFileType, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ IGrid ** retval);
	STDMETHOD(hBitmapToPicture)(/*[in]*/ long hBitmap, /*[out, retval]*/ IPictureDisp ** retval);
	STDMETHOD(ShapefileToGrid)(/*[in]*/ IShapefile * Shpfile,/* [in, optional, defaultvalue(TRUE)]*/ VARIANT_BOOL UseShapefileBounds, /*[in, optional, defaultvalue(NULL)]*/IGridHeader * GrdHeader, /*[in,optional, defaultvalue(30.0)]*/double Cellsize, /*[in,optional, defaultvalue(TRUE)]*/VARIANT_BOOL UseShapeNumber, /*[in,optional, defaultvalue(1)]*/short SingleValue,/*[out, retval]*/ IGrid ** retval);
	STDMETHOD(TinToShapefile)(/*[in]*/ ITin * Tin, /*[in]*/ ShpfileType Type, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ IShapefile ** retval);
	STDMETHOD(ShapeToShapeZ)(/*[in]*/ IShapefile * Shapefile, /*[in]*/ IGrid * Grid, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ IShapefile ** retval);
	STDMETHOD(GridToGrid)(/*[in]*/ IGrid * Grid, /*[in]*/ GridDataType OutDataType, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ IGrid ** retval);
	STDMETHOD(GridToShapefile)(/*[in]*/ IGrid * Grid, /*[in, optional, defaultvalue(NULL)]*/ IGrid * ConnectionGrid, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ IShapefile ** retval);
	STDMETHOD(GridToImage)(/*[in]*/ IGrid * Grid, /*[in]*/ IGridColorScheme * ci, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ IImage ** retval);
	STDMETHOD(GenerateHillShade)(/*[in]*/ BSTR bstrGridFilename, /*[in]*/ BSTR bstrShadeFilename, /*[in, optional, defaultvalue(1)]*/ float z, /*[in, optional, defaultvalue(1)]*/ float scale, /*[in, optional, defaultvalue(315)]*/ float az, /*[in, optional, defaultvalue(45)]*/ float alt, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(GenerateContour)(/*[in]*/ BSTR pszSrcFilename, /*[in]*/ BSTR pszDstFilename, /*[in]*/ double dfInterval, /*[in, optional, defaultvalue(0)]*/ double dfNoData, /*[in, optional, defaultvalue(FALSE)]*/ VARIANT_BOOL Is3D, /* [in, optional, defaultvalue(NULL)] */ VARIANT dblFLArray, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(TranslateRaster)(/*[in]*/ BSTR bstrSrcFilename, /*[in]*/ BSTR bstrDstFilename, /*[in]*/ BSTR bstrOptions, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(GDALInfo)(/*[in]*/ BSTR bstrSrcFilename, /*[in]*/ BSTR bstrOptions, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ BSTR * bstrInfo);

	STDMETHOD(GDALWarp)(/*[in]*/ BSTR bstrSrcFilename, /*[in]*/ BSTR bstrDstFilename, /*[in]*/ BSTR bstrOptions, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(GDALBuildVrt)(/*[in]*/ BSTR bstrDstFilename, /*[in]*/ BSTR bstrOptions, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(GDALAddOverviews)(/*[in]*/ BSTR bstrSrcFilename, /*[in]*/ BSTR bstrDstFilename, /*[in]*/ BSTR bstrLevels, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(GDALRasterize)(/*[in]*/ BSTR bstrSrcFilename, /*[in]*/ BSTR bstrDstFilename, /*[in]*/ BSTR bstrOptions, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(OGRInfo)(/*[in]*/ BSTR bstrSrcFilename, /*[in]*/ BSTR bstrOptions, /*[in, optional]*/ BSTR bstrLayers, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ BSTR * bstrInfo);
	STDMETHOD(OGR2OGR)(/*[in]*/ BSTR bstrSrcFilename, /*[in]*/ BSTR bstrDstFilename, /*[in]*/ BSTR bstrOptions, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);

	STDMETHOD(OGRLayerToShapefile)(/*[in]*/BSTR Filename, /*[in, optional, defaultvalue(SHP_NULLSHAPE)]*/ ShpfileType shpType, /*[in, optional, defaultvalue(NULL)]*/ICallback *cBack, /*[out, retval]*/IShapefile** sf);
	STDMETHOD(MergeImages)(/*[in]*/SAFEARRAY* InputNames, /*[in]*/BSTR OutputName, VARIANT_BOOL* retVal);
	STDMETHOD(ReprojectShapefile)(IShapefile* sf, IGeoProjection* source, IGeoProjection* target, IShapefile** result);

	STDMETHOD(ClipGridWithPolygon)(BSTR inputGridfile, IShape* poly, BSTR resultGridfile, VARIANT_BOOL keepExtents, VARIANT_BOOL* retVal);
	STDMETHOD(ClipGridWithPolygon2)(IGrid* grid, IShape* poly, BSTR resultGridfile, VARIANT_BOOL keepExtents, VARIANT_BOOL* retVal);
	STDMETHOD(GridStatisticsToShapefile)(IGrid* grid, IShapefile* sf, VARIANT_BOOL selectedOnly, VARIANT_BOOL overwriteFields, VARIANT_BOOL* retVal) ;
	STDMETHOD(Polygonize)(/*[in]*/ BSTR pszSrcFilename, /*[in]*/ BSTR pszDstFilename, /*[in, optional, defaultvalue(1)]*/ int iSrcBand, /*[in, optional, defaultvalue(FALSE)]*/ VARIANT_BOOL NoMask, /*[in, optional, defaultvalue(NULL)]*/ BSTR pszMaskFilename, /*[in, optional, defaultvalue("GML")]*/ BSTR pszOGRFormat, /*[in, optional, defaultvalue("out")]*/ BSTR pszDstLayerName, /*[in, optional, defaultvalue("DN")]*/ BSTR pszPixValFieldName, /*[in, optional, defaultvalue(NULL)]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(CreateInstance)(tkInterface interfaceId, IDispatch** retVal);
	STDMETHOD(CopyNodataValues)(BSTR sourceFilename, BSTR destFilename, VARIANT_BOOL* retVal);
	STDMETHOD(ColorByName)(tkMapColor name, OLE_COLOR* retVal);
	STDMETHOD(ConvertDistance)(tkUnitsOfMeasure sourceUnit, tkUnitsOfMeasure targetUnit, DOUBLE* value, VARIANT_BOOL* retval);
	STDMETHOD(GeodesicDistance)(double lat1, double lng1, double lat2, double lng2, double* retVal);
	STDMETHOD(MaskRaster)(BSTR filename, BYTE newPerBandValue, VARIANT_BOOL* retVal);
	STDMETHOD(GridStatisticsForPolygon)(IGrid* grid, IGridHeader* header, IExtents* gridExtents, IShape* shape, 
										  double noDataValue, double* meanValue, double* minValue, double* maxValue, VARIANT_BOOL* retVal);
	STDMETHOD(GridToImage2)(IGrid * Grid, IGridColorScheme * ci, tkGridProxyFormat imageFormat, VARIANT_BOOL inRam, ICallback* cBack, IImage ** retval);
	//STDMETHOD(GridToImageInRam)(IGrid * Grid, IGridColorScheme * ci, ICallback* cBack, IImage ** retval);
	STDMETHOD(ErrorMsgFromObject)(IDispatch * comClass, BSTR* retVal);
	STDMETHOD(TileProjectionToGeoProjection)(tkTileProjection projection, IGeoProjection** retVal);
	STDMETHOD(get_ComUsageReport)(VARIANT_BOOL unreleasedOnly, BSTR* retVal);
private:
	inline long findBreak( std::deque<BreakVal> & bvals, double val );
	bool PolygonToGrid(IShape * shape, IGrid ** grid, short cellValue);
	//Polygonal Algorithm
	//Used to minimize stack in recursive call for trace_polygon
	//Cell 4
	long cell4_x;
	long cell4_y;
	//Cell 6
	long cell6_x;
	long cell6_y;
	//Cell 2
	long cell2_x;
	long cell2_y;
	//Cell 8			
	long cell8_x;
	long cell8_y;
	//Flow Directions
	double flow2;
	double flow8;
	double flow4;
	double flow6;
	IGrid * expand_grid;
	IGrid * connection_grid;

		bool CanScanlineBuffer;
	char BufferLastUsed;
	int BufferANum;
	int BufferBNum;
	_int32 * BufferA_R;
	_int32 * BufferA_G;
	_int32 * BufferA_B;
	_int32 * BufferB_R;
	_int32 * BufferB_G;
	_int32 * BufferB_B;
	GDALRasterBand * poBand_R;
	GDALRasterBand * poBand_G;
	GDALRasterBand * poBand_B;
	GDALDataset * rasterDataset;
	vector<double> pip_xs;
	vector<double> pip_xs_parts;
	std::deque<long> pip_cache_parts;
	std::deque<double> pip_cache_pointsX;
	std::deque<double> pip_cache_pointsY;
	double pip_left;
	double pip_right;
	double pip_top;
	double pip_bottom;
	long lastErrorCode;
	ICallback * globalCallback;
	BSTR key;
	IGeoProjection* _tileProjections[2];

	void trace_polygon( long x, long y, std::deque<RasterPoint> & polygon );
	inline bool is_joint( double cell2, double cell8, double cell4, double cell6 );
	inline double getValue( IGrid * Grid, long column, long row );
	inline void setValue( IGrid * Grid, long column, long row, double val );
	void scan_fill_to_edge( double & nodata, long x, long y );
	void mark_edge( double & polygon_id, long x, long y );
	inline bool is_decision( IGrid * g, int x, int y );
private:
	inline bool does_cross( int SH, int NSH, double corner_oneX, double corner_oneY, double corner_twoX, double corner_twoY );
	inline void set_sign( double val, int & SH );
	bool is_clockwise( double x0, double y0, double x1, double y1, double x2, double y2); //ah 11/8/05
	bool is_clockwise(IShape *Shape); //ah 6/3/05
	bool is_clockwise(Poly *polygon);//ah 6/3/05
	
	bool isColinear( POINT one, POINT two, POINT test, double tolerance );
	//STDMETHODIMP CUtils::GridToImage_DiskBased(IGrid *Grid, IGridColorScheme *ci, ICallback *cBack, IImage ** retval);
	//STDMETHODIMP CUtils::GridToImage_InRAM(IGrid *Grid, IGridColorScheme *ci, ICallback *cBack, IImage ** retval);
	inline void PutBitmapValue(long col, long row, _int32 Rvalue, _int32 Gvalue, _int32 Bvalue, long totalWidth);
	void CreateBitmap(CStringW filename, long cols, long rows, tkGridProxyFormat format, VARIANT_BOOL * retval);
	bool MemoryAvailable(double bytes);
	void FinalizeAndCloseBitmap(int totalWidth);

	HRESULT CUtils::RunGridToImage(IGrid * Grid, IGridColorScheme * ci, tkGridProxyFormat imageFormat, 
								bool inRam, bool checkMemory, ICallback* callback, IImage ** retval);
	void GridToImageCore(IGrid *Grid, IGridColorScheme *ci, ICallback *cBack, bool inRam, IImage ** retval);
	inline void WritePixel(IImage* img, int row, int col, OLE_COLOR color, 
								int nodataColor_R, int nodataColor_G, int nodataColor_B, int ncols, bool inRam);
	void WriteWorldFile(CStringW worldFile, CStringW imageFile, double dx, double dy, double xll, double yll, int nrows);
	void CUtils::ErrorMessage(long ErrorCode);
	

	/* GDAL/OGR functions */
	void Parse(CString sOrig, int * opts);
	BOOL ProcessGeneralOptions(int * opts);
	HRESULT ResetConfigOptions(long ErrorCode = 0);

	/* GDAL/OGR variables */
	int bSubCall;
	CStringArray sArr;
	CStringArray sConfig;
};

double CalcPolyGeodesicArea(std::vector<Point2D>& points);

OBJECT_ENTRY_AUTO(__uuidof(Utils), CUtils)

