//********************************************************************************************************
//File name: Image.h
//Description: Declaration of the CImageClass.
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
//********************************************************************************************************
#pragma once
#include "tkBitmap.h"
#include "tkRaster.h"
#include "ImageGroup.h"
#include "InMemoryBitmap.h"

struct DataPixels
{
public:
	colour value;
	int position; 
};

// CImageClass
class ATL_NO_VTABLE CImageClass : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CImageClass, &CLSID_Image>,
	public IDispatchImpl<IImage, &IID_IImage, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	
	// constructor
	CImageClass::CImageClass()
	{
		globalCallback = NULL;
		ImageData = NULL;
		
		m_drawingMethod = 0;
		m_drawingMethod |= idmNewWithResampling;
		m_drawingMethod |= idmGDIPlusDrawing;
		m_drawingMethod |= idmGDIPlusResampling;

		dX = dY = 1.0;
		XllCenter = YllCenter = 0.0;
		Width = Height = 0;

		lastErrorCode = tkNO_ERROR;

		USES_CONVERSION;
		key = A2BSTR("");

		m_labels = NULL;
		CoCreateInstance(CLSID_Labels,NULL,CLSCTX_INPROC_SERVER,IID_ILabels,(void**)&m_labels);
		
		_bitmapImage = NULL;
		_rasterImage = NULL;
		
		ImgType = USE_FILE_EXTENSION;
		
		gdalImage = false;

		m_groupID = -1;
		m_pixels = NULL;
		m_pixelsCount = 0;
		_pixelsSaved = false;
		_bufferReloadIsNeeded = true;
		_imageChanged = false;
		_screenBitmap = NULL;

		_sourceType = istUninitialized;

		isGridProxy = false;
		sourceGridName = "";

		m_iconGdiPlus = NULL;

		SetDefaults();
		
		gReferenceCounter.AddRef(tkInterface::idImage);
	}
	
	void CImageClass::SetDefaults()
	{
		m_downsamplingMode = m_globalSettings.imageDownsamplingMode;	// imNone
		m_upsamplingMode = m_globalSettings.imageUpsamplingMode;		// imBilinear

		m_transparencyPercent = 1.0;

		setRGBToGrey = false;
		m_downSampling = false;

		transColor = RGB(0,0,0);
		transColor2 = RGB(0,0,0);
		useTransColor = VARIANT_FALSE;

		_canUseGrouping = true;
	}

	// destructor
	CImageClass::~CImageClass()
	{	
		VARIANT_BOOL rt;
		this->Close(&rt);
		
		::SysFreeString(key);

		if( globalCallback )
		{
			globalCallback->Release();
			globalCallback = NULL;
		}

		if (m_labels != NULL)
		{
			m_labels->Release();
			m_labels = NULL;
		}

		if (_screenBitmap)
		{
			delete _screenBitmap;
			_screenBitmap = NULL;
		}
		
		if (m_iconGdiPlus)
		{
			delete m_iconGdiPlus;
			m_iconGdiPlus = NULL;
		}
		gReferenceCounter.Release(tkInterface::idImage);
	}

public:
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_IMAGE)

	DECLARE_NOT_AGGREGATABLE(CImageClass)

	BEGIN_COM_MAP(CImageClass)
		COM_INTERFACE_ENTRY(IImage)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

// IImage
public:
	STDMETHOD(SetImageBitsDC)(/*[in]*/ long hDC, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(GetImageBitsDC)(/*[in]*/ long hDC, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(get_Filename)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Picture)(/*[out, retval]*/ IPictureDisp * *pVal);
	STDMETHOD(putref_Picture)(/*[in]*/ IPictureDisp *newVal);
	STDMETHOD(get_ImageType)(/*[out, retval]*/ ImageType *pVal);
	STDMETHOD(get_FileHandle)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback* *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback* newVal);
	STDMETHOD(get_CdlgFilter)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_UseTransparencyColor)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_UseTransparencyColor)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_TransparencyColor)(/*[out, retval]*/ OLE_COLOR *pVal);
	STDMETHOD(put_TransparencyColor)(/*[in]*/ OLE_COLOR newVal);
	STDMETHOD(get_IsInRam)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_Value)(/*[in]*/long row, /*[in]*/long col, /*[out, retval]*/ long *pVal);
	STDMETHOD(put_Value)(/*[in]*/long row, /*[in]*/long col, /*[in]*/ long newVal);
	STDMETHOD(get_dX)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_dX)(/*[in]*/ double newVal);
	STDMETHOD(get_dY)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_dY)(/*[in]*/ double newVal);
	STDMETHOD(get_XllCenter)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_XllCenter)(/*[in]*/ double newVal);
	STDMETHOD(get_YllCenter)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_YllCenter)(/*[in]*/ double newVal);
	STDMETHOD(get_Height)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Width)(/*[out, retval]*/ long *pVal);
	STDMETHOD(GetRow)(/*[in]*/ long Row, /*[in, out]*/ long * Vals, /*[out, retval]*/VARIANT_BOOL * retval);
	STDMETHOD(Clear)(/*[in, optional, defaultvalue(0xffffff)]*/ OLE_COLOR CanvasColor, /*[in, optional]*/ICallback * CBack, /*[out, retval]*/VARIANT_BOOL * retval);
	STDMETHOD(Close)(/*[out, retval]*/VARIANT_BOOL * retval);
	STDMETHOD(CreateNew)(/*[in]*/long NewWidth, /*[in]*/long NewHeight, /*[out, retval]*/VARIANT_BOOL * retval);
	STDMETHOD(Save)(/*[in]*/BSTR ImageFileName, /*[in, optional, defaultvalue(FALSE)]*/VARIANT_BOOL WriteWorldFile,/*[in, optional, defaultvalue(USE_FILE_EXTENSION)]*/ImageType FileType, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(Open)(/*[in]*/BSTR ImageFileName, /*[in, optional, defaultvalue(USE_FILE_EXTENSION)]*/ImageType FileType, /*[in, optional, defaultvalue(TRUE)]*/ VARIANT_BOOL InRam, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	//Rob Cairns
	STDMETHOD (SetVisibleExtents)(/*[in]*/double newMinX,/*[in]*/ double newMinY,/*[in]*/ double newMaxX,/*[in]*/ double newMaxY,/*[in]*/long pixInView, /*[in]*/float transPercent);
	STDMETHOD(SetProjection)(/*[in]*/BSTR Proj4, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(GetProjection)(/*[out, retval]*/ BSTR * Proj4);
	STDMETHOD(Resource)(/*[in]*/ BSTR newSrcPath, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(_pushSchemetkRaster)(/*[in]*/ IGridColorScheme * cScheme, /*[out, retval]*/ VARIANT_BOOL * retval);
	//Rob Cairns 1-Jun-09

	// deprecated
	STDMETHOD(GetOriginalXllCenter)(/*[out, retval]*/ double *pVal);
	STDMETHOD(GetOriginalYllCenter)(/*[out, retval]*/ double *pVal);
	STDMETHOD(GetOriginal_dX)(/*[out, retval]*/ double *pVal);
	STDMETHOD(GetOriginal_dY)(/*[out, retval]*/ double *pVal);
	STDMETHOD(GetOriginalHeight)(/*[out, retval]*/ long *pVal);
	STDMETHOD(GetOriginalWidth)(/*[out, retval]*/ long *pVal);
	// deprecated

	STDMETHOD(get_AllowHillshade)(/*[out]*/VARIANT_BOOL *pVal);
	STDMETHOD(put_AllowHillshade)(/*[in]*/VARIANT_BOOL newValue);
	STDMETHOD(get_SetToGrey)(/*[out, retval]*/VARIANT_BOOL *pVal);
	STDMETHOD(put_SetToGrey)(/*[in]*/VARIANT_BOOL newValue);
	STDMETHOD(get_UseHistogram)(/*[out, retval]*/VARIANT_BOOL *pVal);
	STDMETHOD(put_UseHistogram)(/*[in]*/VARIANT_BOOL newValue);
	STDMETHOD(get_HasColorTable)(/*[out, retval]*/VARIANT_BOOL *pVal);
	STDMETHOD(get_PaletteInterpretation)(/*[out, retval]*/BSTR *pVal);
	STDMETHOD(get_BufferSize)(/*[out, retval]*/int *pVal);
	STDMETHOD(put_BufferSize)(/*[in]*/int newValue);
	STDMETHOD(get_NoBands)(/*[out, retval]*/int *pVal);
	STDMETHOD(get_ImageColorScheme)(/*[out, retval]*/PredefinedColorScheme *pVal);
	STDMETHOD(put_ImageColorScheme)(/*[in]*/PredefinedColorScheme newValue);
	STDMETHOD(get_ClearGDALCache)(VARIANT_BOOL* retVal);	
	STDMETHOD(put_ClearGDALCache)(VARIANT_BOOL newVal);	
	STDMETHOD(get_TransparencyPercent)(double* retVal);	
	STDMETHOD(put_TransparencyPercent)(double newVal);
	STDMETHOD(get_TransparencyColor2)(OLE_COLOR* retVal);	
	STDMETHOD(put_TransparencyColor2)(OLE_COLOR newVal);	
	STDMETHOD(get_DownsamplingMode)(tkInterpolationMode* retVal);	
	STDMETHOD(put_DownsamplingMode)(tkInterpolationMode newVal);	
	STDMETHOD(get_UpsamplingMode)(tkInterpolationMode* retVal);	
	STDMETHOD(put_UpsamplingMode)(tkInterpolationMode newVal);	
	STDMETHOD(get_DrawingMethod)(int* retVal);	
	STDMETHOD(put_DrawingMethod)(int newVal);	
	STDMETHOD(BuildOverviews)(tkGDALResamplingMethod ResamplingMethod, int numOverviews, SAFEARRAY* OverviewList, VARIANT_BOOL * retval);
	STDMETHOD(get_Labels)(ILabels** pVal);
	STDMETHOD(put_Labels)(ILabels* newValue);
	STDMETHOD(get_OriginalWidth)(LONG* OriginalWidth);
	STDMETHOD(get_OriginalHeight)(LONG* OriginalHeight);
	STDMETHOD(get_Extents)(IExtents** pVal);
	STDMETHOD(ProjectionToImage)(double ProjX, double ProjY, long* ImageX, long* ImageY);
	STDMETHOD(ImageToProjection)(long ImageX, long ImageY, double* ProjX, double* ProjY);
	STDMETHOD(ProjectionToBuffer)(double ProjX, double ProjY, long* BufferX, long* BufferY);
	STDMETHOD(BufferToProjection)(long BufferX, long BufferY, double* ProjX, double* ProjY);
	STDMETHOD(get_CanUseGrouping)(VARIANT_BOOL* retVal);
	STDMETHOD(put_CanUseGrouping)(VARIANT_BOOL newVal);
	STDMETHOD(get_OriginalXllCenter)( double *pVal);
	STDMETHOD(put_OriginalXllCenter)( double newVal);
	STDMETHOD(get_OriginalYllCenter)( double *pVal);
	STDMETHOD(put_OriginalYllCenter)( double newVal);
	STDMETHOD(get_OriginalDX)( double *pVal);
	STDMETHOD(put_OriginalDX)( double newVal);
	STDMETHOD(get_OriginalDY)( double *pVal);
	STDMETHOD(put_OriginalDY)( double newVal);
	STDMETHOD(GetUniqueColors)(double MaxBufferSizeMB, VARIANT* Colors, VARIANT* Frequencies, LONG* Count);
	STDMETHOD(SetNoDataValue)(double Value, VARIANT_BOOL* Result);
	STDMETHOD(get_NumOverviews)(int* retval);
	STDMETHOD(LoadBuffer)(double maxBufferSize, VARIANT_BOOL* retVal);	// loads the whole buffer
	STDMETHOD(get_SourceType)(tkImageSourceType* retVal);
	STDMETHOD(Serialize)(VARIANT_BOOL SerializePixels, BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal);
	STDMETHOD(get_Warped)(VARIANT_BOOL* retVal);
	
	STDMETHOD(get_SourceGridName)(BSTR* retVal);
	STDMETHOD(get_IsGridProxy)(VARIANT_BOOL* retVal);
	STDMETHOD(get_CustomColorScheme)( IGridColorScheme** pVal);
	STDMETHOD(put_CustomColorScheme)( IGridColorScheme* newVal);
	//STDMETHOD(get_CanUseExternalColorScheme)(VARIANT_BOOL* retVal);
	STDMETHOD(get_AllowGridRendering)(tkGridRendering* retVal);
	STDMETHOD(put_AllowGridRendering)(tkGridRendering newVal);
	STDMETHOD(get_GridRendering)(VARIANT_BOOL* retVal);
	STDMETHOD(SetTransparentColor)(OLE_COLOR color);
	STDMETHOD(get_IsRgb)(VARIANT_BOOL* retVal);
	STDMETHOD(OpenAsGrid)(IGrid** retVal);
	STDMETHOD(get_SourceFilename)(BSTR* retVal);
	STDMETHOD(get_SourceGridBandIndex)(int* retVal);
	STDMETHOD(put_SourceGridBandIndex)(int newVal);
	STDMETHOD(get_GridProxyColorScheme)(IGridColorScheme** retVal);

	//STDMETHOD(LoadBuffer)(double mapMinX, double mapMinY, double mapMaxX, double mapMaxY, double mapUnitsPerScreenPixel, VARIANT_BOOL* retVal);
	//STDMETHOD(OpenForUpdate)(BSTR ImageFileName, ImageType FileType,  VARIANT_BOOL InRam, ICallback * cBack, VARIANT_BOOL * retval);
	bool CImageClass::DeserializeCore(CPLXMLNode* node);
	CPLXMLNode* SerializeCore(VARIANT_BOOL SerializePixels, CString ElementName);

// ---------------------------------------------------------
//	Members
// ---------------------------------------------------------
public:
	int m_groupID;			// in case belong to the group of images which are to be treated as one
	DataPixels* m_pixels;	// a structure to hold values of pixels with the value other than noDataValue
	int m_pixelsCount;
	bool _canUseGrouping;
	bool _pixelsSaved;
	bool _bufferReloadIsNeeded;
	bool _imageChanged;
	ScreenBitmap* _screenBitmap;	// GDI+ bitmap on the screen
	CStringW sourceGridName;
	bool isGridProxy;

private:
	tkImageSourceType _sourceType;
	
	bool inRam;
	BSTR key;
	ICallback * globalCallback;
	long lastErrorCode;		//definition of numbers found in "ErrorCodes.h"
	//world coordinate related variables
	double dY;			//change in Y (for size of cell or pixel)
	double dX;			//change in X (for size of cell or pixel)
	double YllCenter;	//Y coordinate of lower left corner of image (world coordinate)
	double XllCenter;	//X coordinate of lower left corner of image (world coordinate)

	bool gdalImage;
	bool dataLoaded;
	bool setRGBToGrey;			//Set a color RGB image or hillshade to grey
	CStringW fileName;			//For GDALOpen

	//Image Variables
	colour * ImageData;		//array storing generic image data
	long Width;				//number of Columns in image
	long Height;			//number of rows in image
	ImageType ImgType;	    //enumeration stating type of image currently being used

	//Image Objects
	tkBitmap* _bitmapImage;
	tkRaster* _rasterImage;

	//Transparency
	OLE_COLOR transColor;
	OLE_COLOR transColor2;	// to specify range for GDI+
	VARIANT_BOOL useTransColor;

	double m_transparencyPercent;
	tkInterpolationMode m_downsamplingMode;
	tkInterpolationMode m_upsamplingMode;
	short m_drawingMethod;
	bool m_downSampling;	// the image in the buffer was produced by downsampling

	ILabels* m_labels;
	
// ---------------------------------------------------------
//	Functions
// ---------------------------------------------------------
public:
	// in-memory state of gdi plus image file (after deserialization)
	CMemoryBitmap* m_iconGdiPlus;

	bool SetDCBitsToImage(long hDC,BYTE* bits);
	unsigned char* get_ImageData();
	void put_ImageData(colour* data);
	bool get_BufferIsDownsampled();
	//int GDALProgressFunction( double dfComplete, const char *pszMessage, void *pData);
	void ClearBuffer();
	bool SaveNotNullPixels(bool forceSaving = false);
	void ClearNotNullPixels();
	void OpenImage(BSTR ImageFileName, ImageType FileType, VARIANT_BOOL InRam, ICallback *cBack, GDALAccess accessMode, bool checkForProxy, VARIANT_BOOL *retval);
	
	int get_originalBufferWidth()
	{
		if (gdalImage && _rasterImage)	return (_rasterImage->_visibleRect.right - _rasterImage->_visibleRect.left);
		else							return 0;
	}	
	int get_originalBufferHeight()
	{
		if (gdalImage && _rasterImage)	return (_rasterImage->_visibleRect.right - _rasterImage->_visibleRect.left);
		else							return 0;
	}
	void ErrorMessage(long ErrorCode);
	void LoadImageAttributesFromGridColorScheme(IGridColorScheme* scheme);

private:
	bool Resample();
	bool ReadPPM(CString ImageFile, bool InRam = true);
	bool ReadGIF(CString ImageFile);
	bool ReadJPEG(CString ImageFile);
	bool ReadBMP(const CStringW ImageFile, bool InRam);
	bool ReadRaster(const CStringW ImageFile, GDALAccess accessMode);
	//bool LoadImage(double minx, double miny, double maxx, double maxy, double mapUnitsPerScreenPixel);
	
	bool getFileType(const CStringW ImageFile, ImageType &ft);
	bool WritePPM(CString ImageFile, bool WorldFile, ICallback *cBack);
	bool WriteGIF(CString ImageFile, bool WorldFile, ICallback * cBack);
	bool WriteBMP(CString FileName, bool WriteWorldFile, ICallback *cBack);
	bool WriteGDIPlus(CString ImageFile, bool WorldFile, ImageType type, ICallback *cBack);
	bool CImageClass::CopyGDALImage(CStringW ImageFileName );

	VARIANT_BOOL WriteWorldFile(CStringW WorldFileName);
	bool ReadWorldFile(CStringW WorldFileName);
	
	//void LoadImageStandard(double mapUnitsPerScreenPixel);
	bool CImageClass::IsGdalImageAvailable();
	bool CImageClass::BuildColorMap(colour* data, int size, VARIANT* Colors, VARIANT* Frequencies,  long* count);
	bool CImageClass::SetImageBitsDCCore(HDC hdc);
	bool CImageClass::CheckForProxy();
	
};

OBJECT_ENTRY_AUTO(__uuidof(Image), CImageClass)



