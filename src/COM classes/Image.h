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
		_globalCallback = NULL;
		_imageData = NULL;
		
		_drawingMethod = 0;
		_drawingMethod |= idmNewWithResampling;
		_drawingMethod |= idmGDIPlusDrawing;
		_drawingMethod |= idmGDIPlusResampling;

		_dX = _dY = 1.0;
		_xllCenter = _yllCenter = 0.0;
		_width = _height = 0;

		_lastErrorCode = tkNO_ERROR;

		_key = SysAllocString(L"");

		ComHelper::CreateInstance(idLabels, (IDispatch**)&_labels);
		ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_projection);
		
		_bitmapImage = NULL;
		_rasterImage = NULL;
		
		_imgType = USE_FILE_EXTENSION;
		
		_gdalImage = false;

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

		_iconGdiPlus = NULL;

		SetDefaults();
		
		gReferenceCounter.AddRef(tkInterface::idImage);
	}
	
	void CImageClass::SetDefaults()
	{
		_downsamplingMode = m_globalSettings.imageDownsamplingMode;	// imNone
		_upsamplingMode = m_globalSettings.imageUpsamplingMode;		// imBilinear

		_transparencyPercent = 1.0;

		_setRGBToGrey = false;
		_downSampling = false;

		_transColor = RGB(0,0,0);
		_transColor2 = RGB(0,0,0);
		_useTransColor = VARIANT_FALSE;

		_canUseGrouping = true;
	}

	// destructor
	CImageClass::~CImageClass()
	{	
		VARIANT_BOOL rt;
		this->Close(&rt);
		
		::SysFreeString(_key);

		if( _globalCallback )
			_globalCallback->Release();

		if (_labels)
			_labels->Release();

		if (_projection)
			_projection->Release();

		if (_screenBitmap)
		{
			delete _screenBitmap;
			_screenBitmap = NULL;
		}
		
		if (_iconGdiPlus)
		{
			delete _iconGdiPlus;
			_iconGdiPlus = NULL;
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
	STDMETHOD(get_GeoProjection)(IGeoProjection** pVal);
	STDMETHOD(get_IsEmpty)(VARIANT_BOOL* pVal);

private:
	tkImageSourceType _sourceType;
	
	bool _inRam;
	BSTR _key;
	ICallback * _globalCallback;
	long _lastErrorCode;

	//world coordinate related variables
	double _dY;			//change in Y (for size of cell or pixel)
	double _dX;			//change in X (for size of cell or pixel)
	double _yllCenter;	//Y coordinate of lower left corner of image (world coordinate)
	double _xllCenter;	//X coordinate of lower left corner of image (world coordinate)

	bool _gdalImage;
	bool _dataLoaded;
	bool _setRGBToGrey;			//Set a color RGB image or hillshade to gray
	CStringW _fileName;			//For GDALOpen

	//Image Variables
	colour * _imageData;		//array storing generic image data
	long _width;				//number of Columns in image
	long _height;			//number of rows in image
	ImageType _imgType;	    //enumeration stating type of image currently being used

	//Image Objects
	tkBitmap* _bitmapImage;
	tkRaster* _rasterImage;

	//Transparency
	OLE_COLOR _transColor;
	OLE_COLOR _transColor2;	// to specify range for GDI+
	VARIANT_BOOL _useTransColor;

	double _transparencyPercent;
	tkInterpolationMode _downsamplingMode;
	tkInterpolationMode _upsamplingMode;
	short _drawingMethod;
	bool _downSampling;	// the image in the buffer was produced by downsampling

	ILabels* _labels;
	IGeoProjection* _projection;

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
	CMemoryBitmap* _iconGdiPlus;	// in-memory state of GDI+ image file (after deserialization)

private:
	bool Resample();
	bool ReadPPM(CString ImageFile, bool InRam = true);
	bool ReadGIF(CString ImageFile);
	bool ReadJPEG(CString ImageFile);
	bool ReadBMP(const CStringW ImageFile, bool InRam);
	bool ReadRaster(const CStringW ImageFile, GDALAccess accessMode);

	bool getFileType(const CStringW ImageFile, ImageType &ft);
	bool WritePPM(CStringW ImageFile, bool WorldFile, ICallback *cBack);
	bool WriteGIF(CString ImageFile, bool WorldFile, ICallback * cBack);
	bool WriteBMP(CStringW FileName, bool WriteWorldFile, ICallback *cBack);
	bool WriteGDIPlus(CStringW ImageFile, bool WorldFile, ImageType type, ICallback *cBack);

	VARIANT_BOOL WriteWorldFile(CStringW WorldFileName);
	bool ReadWorldFile(CStringW WorldFileName);

	bool IsGdalImageAvailable();
	bool BuildColorMap(colour* data, int size, VARIANT* Colors, VARIANT* Frequencies, long* count);
	bool SetImageBitsDCCore(HDC hdc);
	bool CheckForProxy();
	void ReadProjection();
	bool CopyGdalDataset(CStringW imageFilename, ImageType fileType, bool writeWorldFile);

public:
	bool DeserializeCore(CPLXMLNode* node);
	CPLXMLNode* SerializeCore(VARIANT_BOOL SerializePixels, CString ElementName);

	bool SetDCBitsToImage(long hDC,BYTE* bits);
	unsigned char* get_ImageData();
	void put_ImageData(colour* data);
	bool get_BufferIsDownsampled();

	void ClearBuffer();
	bool SaveNotNullPixels(bool forceSaving = false);
	void ClearNotNullPixels();
	void OpenImage(CStringW ImageFileName, ImageType FileType, VARIANT_BOOL InRam, ICallback *cBack, GDALAccess accessMode, bool checkForProxy, VARIANT_BOOL *retval);
	
	int get_originalBufferWidth()
	{
		if (_gdalImage && _rasterImage)	return (_rasterImage->visibleRect.right - _rasterImage->visibleRect.left);
		else							return 0;
	}	
	int get_originalBufferHeight()
	{
		if (_gdalImage && _rasterImage)	return (_rasterImage->visibleRect.right - _rasterImage->visibleRect.left);
		else							return 0;
	}
	void ErrorMessage(long ErrorCode);
	void LoadImageAttributesFromGridColorScheme(IGridColorScheme* scheme);
	
};
OBJECT_ENTRY_AUTO(__uuidof(Image), CImageClass)



