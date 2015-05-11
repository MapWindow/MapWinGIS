//********************************************************************************************************
//File name: Image.cpp
//Description:  Implementation of CImageClass.
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

#include "stdafx.h"
#include "Image.h"
#include "colour.h"
#include "ImageResampling.h"
#include "tkJpg.h"
#include "tkGif.h"
#include "GdalRaster.h"
#include "projections.h"
#include "Templates.h"
#include "Base64.h"
#include "Labels.h"
#include "GridColorScheme.h"
#include "GridManager.h"
#include <io.h>
#include "RasterBandHelper.h"
#include "GdalDriver.h"
#include "ImageHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// *********************************************************
//		WriteWorldFile()
// *********************************************************
VARIANT_BOOL CImageClass::WriteWorldFile(CStringW WorldFileName)
{
	USES_CONVERSION;

	//19-Oct-09 Rob Cairns: setlocale in case MapWinGIS is made locale aware again in future
	FILE* fout = _wfopen(WorldFileName, L"w");
	
	if( !fout )
	{	
		ErrorMessage(tkCANT_WRITE_WORLD_FILE);
		return VARIANT_FALSE;
	}

	fprintf(fout,"%.14f\n",_dX,setlocale(LC_ALL,"C"));		// TODO: is locale parameter valid?
	fprintf(fout,"%.14f\n",0.0);
	fprintf(fout,"%.14f\n",0.0);
	fprintf(fout,"%.14f\n",_dY*-1.0);
	
	//convert lower left to upper left pixel
	double xupLeft = _xllCenter;
	double yupLeft = _yllCenter + ( _dY*(_height-1));
	
	fprintf(fout,"%.14f\n",xupLeft);
	fprintf(fout,"%.14f\n",yupLeft);
	
	fprintf(fout,"%s\n","[tkImageCom]",setlocale(LC_ALL,""));
	fprintf(fout,"%s %s\n","ImageFile", W2A(_fileName));		// TODO: use Unicode
	fflush(fout);
	fclose(fout);
	return VARIANT_TRUE;
}

// ************************************************************
//	  ReadWorldFile()
// ************************************************************
bool CImageClass::ReadWorldFile(CStringW WorldFileName)
{
	const int IOS_NOCREATE = 32;
	ifstream fin(WorldFileName,IOS_NOCREATE);
	
	if (!fin)
		return false;

	char data[150]; // Had new data[150] that leaked.
	
	fin >> data;	// get dX
	_dX = Utility::atof_custom(data);

	fin >> data;	// read in the 0.0, do nothing with it--not needed
	fin >> data;	// read in the 0.0, do nothing with it--not needed

	fin >> data;	// get dY
	_dY = Utility::atof_custom(data) * -1;

	fin >> data;	//get XllCenter
	_xllCenter = Utility::atof_custom(data);

	fin >> data;	//get YllCenter
	_yllCenter = Utility::atof_custom(data) - (_dY * (_height - 1));
	return true;
}

// *********************************************************
//		Resource()
// *********************************************************
STDMETHODIMP CImageClass::Resource(BSTR newImgPath, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	Close(retval);
	Open(newImgPath, USE_FILE_EXTENSION, true, NULL, retval);

	return S_OK;
}

// ************************************************************
//	  Open()
// ************************************************************
STDMETHODIMP CImageClass::Open(BSTR ImageFileName, ImageType FileType, VARIANT_BOOL InRam, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	OpenImage(OLE2W(ImageFileName), FileType, InRam, cBack, GA_ReadOnly, true, retval);

	ReadProjection();

	return S_OK;
}

// ***************************************************************
//		ReadProjection()
// ***************************************************************
void CImageClass::ReadProjection()
{
	CStringW prjFilename = Utility::GetProjectionFilename(_fileName);

	VARIANT_BOOL vb;
	CComBSTR bstr(prjFilename);
	_projection->ReadFromFile(bstr, &vb);

	CString wkt;
	GdalHelper::GetProjection(_fileName, wkt);

	CComBSTR bstrProjection(wkt);
	_projection->ImportFromAutoDetect(bstrProjection, &vb);
}

// ***************************************************************
//		LoadImageAttributesFromGridColorScheme()
// ***************************************************************
void CImageClass::LoadImageAttributesFromGridColorScheme(IGridColorScheme* scheme)
{
	if (scheme)
	{
		OLE_COLOR color;
		scheme->get_NoDataColor(&color);
		this->put_TransparencyColor(color);
		this->put_TransparencyColor2(color);
		this->put_UseTransparencyColor(true);
		this->put_UpsamplingMode(tkInterpolationMode::imNone);		// we actually want to see pixels in grids
		this->put_DownsamplingMode(tkInterpolationMode::imNone);	// for performance reasons
	}
}

// ************************************************************
//	  OpenImage()
// ************************************************************
// checkForProxy = true; image is being opened by client directly and we don't know whether it proxy or not;
// checkForProxy = false; image is being opened by grid code and we already know that it is a proxy, and all the logic will be executed in grid class
void CImageClass::OpenImage(CStringW ImageFileName, ImageType FileType, VARIANT_BOOL InRam, ICallback *cBack, GDALAccess accessMode, bool checkForProxy, VARIANT_BOOL *retval)
{
	_fileName = ImageFileName;	
	_inRam = (InRam == VARIANT_TRUE)?true:false;
	
	// child classes will be deleted here
	Close(retval);

	if (*retval == VARIANT_FALSE)
		return;
	
	// figuring out extension from the path
	if(FileType == USE_FILE_EXTENSION)
	{
		if(!getFileType(_fileName, FileType))
		{
			// don't give up, we'll try to open it through GDAL
			*retval = VARIANT_FALSE;
		}
	}

	if (FileType == BITMAP_FILE)
	{
		_bitmapImage = new tkBitmap();
		if (!_globalCallback) this->put_GlobalCallback(cBack);
		_bitmapImage->globalCallback = _globalCallback;

		_imgType = BITMAP_FILE;
		*retval = ReadBMP( _fileName, _inRam)?VARIANT_TRUE:VARIANT_FALSE;
		if (*retval)
		{
			_sourceType = InRam?istInMemory:istDiskBased;
		}
	}
	else
	{
		// GDAL will determine the format automatically, so no specific code for extensions is needed.
		// Try it with GDAL - it handles new formats more quickly than
		// we can keep up with. If all of its drivers fail, retval will be false anyway.
		
		_raster = new GdalRaster();
		if (!_globalCallback) this->put_GlobalCallback(cBack);
		_raster->SetCallback(_globalCallback);

		_imgType = FileType;
		*retval = OpenGdalRaster(_fileName, accessMode)?VARIANT_TRUE:VARIANT_FALSE;
		
		if (*retval)
		{
			// setting the type (file extention); for information only?
			switch(FileType)
			{
				case GIF_FILE:	case TIFF_FILE:		case JPEG_FILE: case PPM_FILE:			
				case ECW_FILE:  case JPEG2000_FILE:	case SID_FILE:	case PNG_FILE:   
				case PNM_FILE:	case PGM_FILE:		case BIL_FILE:	case ADF_FILE:
				case GRD_FILE:	case IMG_FILE:		case ASC_FILE:	case BT_FILE:
				case MAP_FILE:	case LF2_FILE:		case KAP_FILE:	case DEM_FILE:
				{
					_imgType = FileType;
					break;
				}
				default:
				{
					_imgType = IMG_FILE;		// Use IMG_FILE as a flag (not technically accurate)
					break;
				}
			}
			_sourceType = istGDALBased;
		}
	}

	if (!*retval)
	{
		VARIANT_BOOL vb;
		this->Close(&vb);
	}

	if (*retval && checkForProxy)
	{
		CheckForProxy();
	}
}

// ********************************************************
//		CheckForProxy()
// ********************************************************
// checks if this is a proxy for some grid
bool CImageClass::CheckForProxy()
{	
	if (!Utility::EndsWith(_fileName, L"_proxy.bmp") && !Utility::EndsWith(_fileName, L"_proxy.tif"))
	{
		return false;
	}
	
	CStringW legendName = _fileName + ".mwleg";
	if (!Utility::FileExistsW(legendName))
	{
		return false;
	}
	
	CPLXMLNode* node = GdalHelper::ParseXMLFile(legendName);
			
	const char* value = CPLGetXMLValue( node, "GridName", NULL );
	CStringW nameW = Utility::ConvertFromUtf8(value);

	if (nameW.GetLength() == 0 && _fileName.GetLength() > 16) 
	{
		// there is no name; try to guess it
		//nameW = fileName.Left(fileName.GetLength() - 16);
		// TODO: how to guess extension
	}

	if (nameW.GetLength() > 0)
	{
		CStringW gridName = Utility::GetFolderFromPath(legendName)  + "\\" + nameW;
		if (Utility::FileExistsW(gridName))
		{
			this->sourceGridName = gridName;
			this->isGridProxy = true;

			VARIANT_BOOL vb;
			IGridColorScheme* scheme = NULL;
			ComHelper::CreateInstance(idGridColorScheme, (IDispatch**)&scheme);

			CComBSTR bstrName(legendName);
			CComBSTR bstrElement("GridColoringScheme");
			scheme->ReadFromFile(bstrName, bstrElement, &vb);
			if (vb)
			{
				this->LoadImageAttributesFromGridColorScheme(scheme);
			}
			scheme->Release();
			return true;
		}
	}

	return false;
}

// ********************************************************
//		OpenGdalRaster()
// ********************************************************
bool CImageClass::OpenGdalRaster(const CStringW ImageFile, GDALAccess accessMode)
{	
	if (!_raster) {
		return false;
	}
	
	// inRam is always true for GDAL-based images	
	_inRam = true;	
	
	if (!_raster->Open(ImageFile, accessMode))
	{	
		ErrorMessage(tkCANT_OPEN_FILE);
		return false;
	}

	_fileName = ImageFile;
	_gdal = true;
	_dataLoaded = false; //not yet loaded into ImageData	

	// default is RGB(0,0,0) if no data value wasn't set	
	_transColor = (int)_raster->GetTransparentColor();		
	_transColor2 = _transColor;
	_useTransColor = _raster->HasTransparency() ? VARIANT_TRUE : VARIANT_FALSE;

	// buffer wasn't loaded yet, so we will not set width, height, dx, etc properties; default values will be used

	return true;
}

// *************************************************************
//	  Save()
// *************************************************************
STDMETHODIMP CImageClass::Save(BSTR ImageFileName, VARIANT_BOOL WriteWorldFile, ImageType FileType, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CStringW newName = OLE2CW( ImageFileName );
	bool bWriteWorldFile = WriteWorldFile == VARIANT_TRUE;

	if(FileType == USE_FILE_EXTENSION)
	{
		if(! getFileType(newName, FileType))
		{	
			ErrorMessage(tkUNSUPPORTED_FILE_EXTENSION);
			*retval = VARIANT_FALSE;
			return S_OK;
		}
	}
	
	USES_CONVERSION;
	switch(FileType)
	{
		case BITMAP_FILE:
			*retval = WriteBMP(newName, bWriteWorldFile, cBack) ? VARIANT_TRUE : VARIANT_FALSE;
			break;
		case PPM_FILE:
			*retval = WritePPM(newName, bWriteWorldFile, cBack) ? VARIANT_TRUE : VARIANT_FALSE;
			break;	
		case JPEG_FILE: 
		case PNG_FILE:
		case GIF_FILE:
		case TIFF_FILE:
			{
				if (CopyGdalDataset(newName, FileType, bWriteWorldFile))
				{
					*retval = VARIANT_TRUE;
					return S_OK;
				}
				*retval = WriteGDIPlus(newName, bWriteWorldFile, FileType, cBack) ? VARIANT_TRUE : VARIANT_FALSE;
			}
			break;
		default:
			{
				if (!IsGdalImageAvailable())
					return S_OK;

				if (CopyGdalDataset(newName, FileType, bWriteWorldFile))
				{
					*retval = VARIANT_TRUE;
					return S_OK;
				}
			}
			break;
	}
	return S_OK;
}

// ************************************************
//       CopyGdalDataset()
// ************************************************
bool CImageClass::CopyGdalDataset(CStringW imageFilename, ImageType fileType, bool writeWorldFile)
{
	if (!_raster) return false;
	
	// report but don't abort the operations
	if (fileType != _imgType)
		ErrorMessage(tkGDAL_INVALID_SAVE_IMAGE_EXTENSION);

	USES_CONVERSION;
	if (_fileName.MakeLower() == imageFilename.MakeLower())
	{
		CallbackHelper::ErrorMsg("Only saving in new file is supported for GDAL datasets.");
		return false;
	}

	GDALDataset* dataset = _raster->GetDataset();
	if (!dataset) return false;

	bool result = GdalHelper::CopyDataset(dataset, imageFilename, _globalCallback, writeWorldFile);

	GdalHelper::BuildOverviewsIfNeeded(imageFilename, false, _globalCallback);		// built-in overviews

	if (!result) {
		CallbackHelper::ErrorMsg("Creating a copy of dataset is failed for the current GDAL driver.");
	}

	return result;
}

// **********************************************************
//	  CreateNew()
// **********************************************************
STDMETHODIMP CImageClass::CreateNew(long newWidth, long newHeight, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*retval = VARIANT_FALSE;

	if (newWidth <= 0 || newHeight <= 0)
	{
		ErrorMessage(tkINVALID_WIDTH_OR_HEIGHT);
		return S_OK;
	}

	this->Close(retval);

	if (!retval) {
		return S_OK;
	}

	try
	{
		_imageData = new colour[newWidth*newHeight];
	}
	catch (...)
	{
		ErrorMessage(tkFAILED_TO_ALLOCATE_MEMORY);
		return S_OK;
	}

	_height = newHeight;
	_width = newWidth;
	_inRam = true;
	_sourceType = istInMemory;
	*retval = VARIANT_TRUE;
	
	return S_OK;
}

// **********************************************************
//	  Close()
// **********************************************************
STDMETHODIMP CImageClass::Close(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//close the current file, making sure that memory is cleaned up correctly
	//This function assumes that the file has already been saved
	//all new data will be lost if not saved prior to calling this function
	if ( _gdal )
	{
		if (_raster)
		{
			int ref = _raster->Dereference();
			
			// Call _rasterImage->Close only if reference count exactly equals zero
			if (ref == 0)
			{
				_raster->Close();
				delete _raster;
				_raster = NULL;
			}
			else
			{
				CallbackHelper::ErrorMsg("Can't delete GDAL raster datasource because of the remaining references.");
			}
		}
	}
	else
	{
		if(  _bitmapImage )
		{
			_bitmapImage->Close();
			delete _bitmapImage;
			_bitmapImage = NULL;
		}
	}
	
	if (_labels)
	{
		_labels->Clear();
	}

	if (_imageData)
	{
		delete[] _imageData;
		_imageData = NULL;
	}

	if (_screenBitmap)
	{
		delete _screenBitmap;
		_screenBitmap = NULL;
	}

	// set default properties
	_imgType = USE_FILE_EXTENSION;
	_gdal = false;
	_dX = _dY = 1.0;
	_xllCenter = _yllCenter = 0.0;
	_width = _height = 0;

	// closing grouped image
	if (m_pixels)
	{
		delete[] m_pixels; 
		m_pixels = NULL;
	}

	m_groupID = -1;
	m_pixels = NULL;
	m_pixelsCount = 0;
	_pixelsSaved = false;

	if (_iconGdiPlus)
	{
		delete _iconGdiPlus;
		_iconGdiPlus = NULL;
	}

	_sourceType = istUninitialized;
	isGridProxy = false;
	sourceGridName = L"";

	*retval = VARIANT_TRUE;
	return S_OK;
}

// **********************************************************
//	  Clear()
// **********************************************************
STDMETHODIMP CImageClass::Clear(OLE_COLOR CanvasColor, ICallback *CBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//This function sets all pixels in the current image to white
	// -- This Deletes any data stored in the ImageData Variable
	// -- Any changes that aren't saved first are lost
	unsigned char Red = GetRValue(CanvasColor);
	unsigned char Green = GetGValue(CanvasColor);
	unsigned char Blue = GetBValue(CanvasColor);

	colour NewColor(Red, Green, Blue);
	long size = _height * _width;
	
	if (_imageData == NULL)
	{
		*retval = VARIANT_FALSE;
		ErrorMessage(tkFILE_NOT_OPEN);
	}
	else
	{
		for (long i = 0; i < size; i++)
			_imageData[i] = NewColor;
		*retval = VARIANT_TRUE;
	}
	return S_OK;
}

// **********************************************************
//	  GetRow()
// **********************************************************
STDMETHODIMP CImageClass::GetRow(long Row, long *Vals, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( Row < 0 && Row >= _height )
	{
		*retval = VARIANT_FALSE;
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}
	
	if ( _gdal && !_dataLoaded )
	{
		ErrorMessage(tkIMAGE_BUFFER_IS_EMPTY);
		return S_OK;
	}
		
	long element = Row * _width;
	for (int col = 0; col < _width; col++)
	{
		colour currentPixel;
		if (_inRam)
		{
			currentPixel = _imageData[element];
			Vals[col] = RGB(currentPixel.red, currentPixel.green, currentPixel.blue);
		}
		else
		{
			if (_imgType == BITMAP_FILE)
			{
				colour tmp;
				tmp = _bitmapImage->getValue(Row, col);
				Vals[col] = RGB(tmp.red, tmp.green, tmp.blue);
			}
			else
				Vals[col] = -1;
		}
		element++;
	}

	*retval =  VARIANT_TRUE;
	return S_OK;
}

//this function returns the value of the width of the current image
//stored in the ImageData variable
STDMETHODIMP CImageClass::get_Width(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _width;
	return S_OK;
}

//this function returns the value of the height of the current image
//stored in the ImageData variable
STDMETHODIMP CImageClass::get_Height(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _height;
	return S_OK;
}

// *****************************************************************
//		get_Value()
// *****************************************************************
STDMETHODIMP CImageClass::get_Value(long row, long col, long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if ( _gdal && !_dataLoaded )
	{
		ErrorMessage(tkIMAGE_BUFFER_IS_EMPTY);
		return S_OK;
	}

	if (row < 0 || row >= _height || col < 0 || col >= _width)
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*pVal = -1;
		return S_OK;
	}

	if (_inRam)
	{
		colour currentPixel = _imageData[row * _width + col];
		*pVal = currentPixel.ToOleColor();
		return S_OK;
	}
	
	if (_imgType == BITMAP_FILE)
	{
		colour tmp = _bitmapImage->getValue(row, col);
		*pVal = tmp.ToOleColor();
		return S_OK;
	}
	
	*pVal = -1;
	
	return S_OK;
}

// *****************************************************************
//		put_Value()
// *****************************************************************
STDMETHODIMP CImageClass::put_Value(long row, long col, long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (row < 0 || row >= _height || col < 0 || col >= _width)
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}
	
	if( _inRam )
	{
		_imageData[row * _width + col].FromOleColor(newVal);
	}
	else
	{	
		if( _imgType == BITMAP_FILE )
		{
			if (!_bitmapImage->setValue(row, col, colour(newVal)))
			{
				ErrorMessage(tkUNRECOVERABLE_ERROR);
			}
		}
		else if (_gdal) 
		{
			CallbackHelper::ErrorMsg("Writing of GDAL formats is not yet supported.");
		}	
		else
		{	
			//only BMP files can set values disk-based
			ErrorMessage(tkUNAVAILABLE_IN_DISK_MODE);
		}
	}

	return S_OK;
}

// **************************************************************
//		get_IsInRam
// **************************************************************
STDMETHODIMP CImageClass::get_IsInRam(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_inRam)	*pVal = VARIANT_TRUE;
	else		*pVal = VARIANT_FALSE;
	return S_OK;
}

// **************************************************************
//		TransparencyColor
// **************************************************************
STDMETHODIMP CImageClass::get_TransparencyColor(OLE_COLOR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _transColor;
	return S_OK;
}

STDMETHODIMP CImageClass::put_TransparencyColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (newVal != _transColor) 
	{
		_pixelsSaved = false;	// pixels saved for grouping will be invalid
		_canUseGrouping = true;
		_bufferReloadIsNeeded = true;
	}
	_transColor = newVal;
	return S_OK;
}

// **************************************************************
//		TransparencyColor2
// **************************************************************
STDMETHODIMP CImageClass::get_TransparencyColor2(OLE_COLOR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _transColor2;
	return S_OK;
}
STDMETHODIMP CImageClass::put_TransparencyColor2(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (newVal != _transColor2) 
	{
		_pixelsSaved = false;	// pixels saved for grouping will be invalid
		_canUseGrouping = true;
		_bufferReloadIsNeeded = true;
	}
	_transColor2 = newVal;
	return S_OK;
}

STDMETHODIMP CImageClass::get_UseTransparencyColor(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _useTransColor;
	return S_OK;
}

STDMETHODIMP CImageClass::put_UseTransparencyColor(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (newVal != _useTransColor) 
	{
		_pixelsSaved = false;	// pixels saved for grouping will be invalid
		_canUseGrouping = true;
		_bufferReloadIsNeeded = true;
	}
	_useTransColor = newVal;
	return S_OK;
}

STDMETHODIMP CImageClass::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

STDMETHODIMP CImageClass::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

STDMETHODIMP CImageClass::get_CdlgFilter(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	// NOTE: These need to match the grid filter for any duplicates.
	*pVal = A2BSTR( "All Supported Image Types|hdr.adf;*.asc;*.bt;*.bil;*.bmp;*.dem;*.ecw;*.img;*.gif;*.map;*.jp2;*.jpg;*.sid;*.pgm;*.pnm;*.png;*.ppm;*.vrt;*.tif;*.ntf|" \
					"ArcInfo Grid Images (hdr.adf)|hdr.adf|" \
					"ASCII Grid Images (*.asc)|*.asc|" \
					"Binary Terrain Images (*.bt)|*.bt|" \
					"BIL (ESRI HDR/BIL Images) (*.bil)|*.bil|" \
					"Bitmap Images (*.bmp)|*.bmp|" \
					"Dem Images (*.dem)|*.dem|" \
					"ECW Images (*.ecw)|*.ecw|" \
					"Erdas Imagine Images (*.img)|*.img|" \
					"GIF Images (*.gif)|*.gif|" \
					"PC Raster Images (*.map)|*.map|" \
					"JPEG2000 Images (*.jp2)|*.jp2|" \
					"JPEG Images (*.jpg)|*.jpg|" \
					"SID (MrSID Images) (*.sid)|*.sid|" \
					"PNG Images (*.png)|*.pgm;*.pnm;*.png;*.ppm|" \
					"CADRG (*.LF2)|*.lf2|" \
					"BSB/KAP (*.kap)|*.kap|" \
					"GDAL Virtual Dataset (*.vrt)|*.vrt|" \
					"National Imagery Transmission Format (*.ntf)|*.ntf|" \
					"Tagged Image File Format (*.tif)|*.tif"); 
	return S_OK;
}

// **********************************************************
//	  get_GlobalCallback()
// **********************************************************
STDMETHODIMP CImageClass::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _globalCallback;
	if( _globalCallback != NULL )
		_globalCallback->AddRef();

	return S_OK;
}
STDMETHODIMP CImageClass::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);

	if (_bitmapImage)
	{
		_bitmapImage->globalCallback = _globalCallback;
	}

	return S_OK;
}

// **********************************************************
//	  get_Key()
// **********************************************************
STDMETHODIMP CImageClass::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);

	return S_OK;
}
STDMETHODIMP CImageClass::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	::SysFreeString(_bitmapImage->key);
	_bitmapImage->key = OLE2BSTR(newVal);
	return S_OK;
}

// **********************************************************
//	  get_FileHandle()
// **********************************************************
STDMETHODIMP CImageClass::get_FileHandle(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( _imgType == BITMAP_FILE )
	{	int handle = _bitmapImage->FileHandle();
		if( handle >= 0 )
			*pVal = _dup(handle);				
		else
			*pVal = -1;
	}
	else
		*pVal = -1;	

	return S_OK;
}

// **********************************************************
//	  get_ImageType()
// **********************************************************
STDMETHODIMP CImageClass::get_ImageType(ImageType *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _imgType;

	return S_OK;
}

// **********************************************************
//	  get_Filename()
// **********************************************************
STDMETHODIMP CImageClass::get_Filename(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = W2BSTR(_fileName);
	return S_OK;
}

// **********************************************************
//	  WriteBMP()
// **********************************************************
bool CImageClass::WriteBMP(CStringW ImageFile, bool WorldFile, ICallback *cBack)
{
	if (ImageFile.GetLength() == 0) return false;

	if( !_inRam )
	{
		if (ImageFile == _fileName) return true;
		ErrorMessage(tkCOPYING_DISK_BASED_BMP_NOT_SUPPORTED);
		return false;
	}	
	
	tkBitmap bmp;
	if( cBack )
		bmp.globalCallback = cBack;

	// Use bitmap conversion
	bmp.setHeight(_height);
	bmp.setWidth(_width);

  	if (!bmp.WriteBitmap(ImageFile, _imageData))
	{	
		ErrorMessage(tkCANT_CREATE_FILE);
		return false;
	}

	_fileName = ImageFile;

	if (WorldFile)
	{
		CStringW worldFileName = Utility::ChangeExtension(ImageFile, L".bpw");
		if (!WriteWorldFile(worldFileName))
			ErrorMessage(tkCANT_WRITE_WORLD_FILE);
	}
	return true;
}

// **********************************************************
//	  WriteGDIPlus()
// **********************************************************
bool CImageClass::WriteGDIPlus(CStringW ImageFile, bool WorldFile, ImageType type, ICallback *cBack)
{
	if (!_inRam) 
	{
		ErrorMessage(tkGDIPLUS_SAVING_AVAILABLE_INRAM);
		return false;
	}

	if (_width == 0 || _height == 0) 
	{
		ErrorMessage(tkIMAGE_BUFFER_IS_EMPTY);
		return false;
	}

	tkJpg jpg;
	jpg.cBack = cBack;

	jpg.InitSize(_width, _height);
	memcpy(jpg.buffer, _imageData, _height*_width*3);

	Gdiplus::Status status;
	CString ext;
	switch (type)
	{
		case PNG_FILE:
			status = jpg.SaveByGdiPlus(ImageFile, L"image/png");
			ext = "pngw";
			break;
		case JPEG_FILE:
			status = jpg.SaveByGdiPlus(ImageFile, L"image/jpeg");
			ext = "jpgw";
			break;
		case TIFF_FILE:
			status = jpg.SaveByGdiPlus(ImageFile, L"image/tiff");
			ext = "tifw";
			break;
		case GIF_FILE:
			status = jpg.SaveByGdiPlus(ImageFile, L"image/gif");
			ext = "gifw";
			break;
		default:
			return false;
	}

	if (status != Gdiplus::Ok) 
	{
		CallbackHelper::ErrorMsg(Debug::Format("Saving with GDI+ has failed. Error code: %d", status));
		return false;
	}

	if(WorldFile)
	{
		USES_CONVERSION;
		CStringW WorldFileName = Utility::ChangeExtension(ImageFile, A2W(ext));
		if (!this->WriteWorldFile(WorldFileName))
			ErrorMessage(tkCANT_WRITE_WORLD_FILE);
	}
	return true;
}

// **********************************************************
//	  WritePPM()
// **********************************************************
bool CImageClass::WritePPM(CStringW ImageFile, bool WorldFile, ICallback *cBack)
{
	tkGif ppm;
	
	ppm.InitSize(_width, _height);
	memcpy(ppm.buffer, _imageData, _height*_width*3);

	if (!ppm.WritePPM(ImageFile))
		return false;
	
	_fileName = ImageFile;

	if(WorldFile)
	{
		CStringW WorldFileName = Utility::ChangeExtension(ImageFile, L".ppw");
		if (!WriteWorldFile(WorldFileName))
			ErrorMessage(tkCANT_WRITE_WORLD_FILE);
	}
	return true;
	
}

// **********************************************************
//	  getFileType()
// **********************************************************
bool CImageClass::getFileType(const CStringW ImageFile, ImageType &ft)
{
	int length = ImageFile.GetLength();
	int dotpos = ImageFile.ReverseFind('.');

	if(dotpos == -1) return false;

	USES_CONVERSION;
	CString ext = W2A(ImageFile.Right(length - dotpos - 1));

	if(ext.CompareNoCase("BMP") == 0)
	{
		ft = BITMAP_FILE;
	}
	else if(ext.CompareNoCase("GIF") == 0)
	{
		ft = GIF_FILE;
	}
	else if(ext.CompareNoCase("TIF") == 0)
	{
		ft = TIFF_FILE;
	}
	else if(ext.CompareNoCase("jpg") == 0)
	{
		ft = JPEG_FILE;
	}
	else if(ext.CompareNoCase("PPM") == 0)
	{
		ft = PPM_FILE;
	}
	else if(ext.CompareNoCase("ECW") == 0)
	{
		ft = ECW_FILE;
	}
	else if(ext.CompareNoCase("JP2") == 0)
	{
		ft = JPEG2000_FILE;
	}
	else if(ext.CompareNoCase("sid") == 0)
	{
		ft = SID_FILE;
	}
	else if(ext.CompareNoCase("png") == 0)
	{
		ft = PNG_FILE;
	}
	else if(ext.CompareNoCase("pnm") == 0)
	{
		ft = PNM_FILE;
	}
	else if(ext.CompareNoCase("pgm") == 0)
	{
		ft = PGM_FILE;
	}
	else if(ext.CompareNoCase("bil") == 0)
	{
		ft = BIL_FILE;
	}
	else if(ext.CompareNoCase("adf") == 0)
	{
		ft = ADF_FILE;
	}
	else if(ext.CompareNoCase("grd") == 0)
	{
		ft = GRD_FILE;
	}
	else if(ext.CompareNoCase("img") == 0)
	{
		ft = IMG_FILE;
	}
	else if(ext.CompareNoCase("asc") == 0)
	{
		ft = ASC_FILE;
	}
	else if(ext.CompareNoCase("bt") == 0)
	{
		ft = BT_FILE;
	}
	else if(ext.CompareNoCase("map") == 0)
	{
		ft = MAP_FILE;
	}
	else if(ext.CompareNoCase("lf2") == 0)
	{
		ft = LF2_FILE;
	}
	else if(ext.CompareNoCase("kap") == 0)
	{
		ft = KAP_FILE;
	}
	else if(ext.CompareNoCase("dem") == 0)
	{
		ft = DEM_FILE;
	}
	else if(ext.CompareNoCase("ntf") == 0)
	{
		ft = NTF_FILE;
	}
	else if(ext.CompareNoCase("vrt") == 0)
	{
		ft = VRT_FILE;
	}
	else if(ext.CompareNoCase("nc") == 0)
	{
		ft = NETCDF_FILE;
	}
	else
	{	//ErrorMessage(tkUNSUPPORTED_FILE_EXTENSION);    // no need to report error, as GDAL may still open it
		return false;
	}

	return true;
}

// **********************************************************
//	  ReadBMP()
// **********************************************************
bool CImageClass::ReadBMP(const CStringW ImageFile, bool InRam)
{	
	bool result;
	
	_inRam = InRam;
	
	if (_imageData)
	{
		delete [] _imageData;
		_imageData = NULL;
	}
		
	result = _inRam ? _bitmapImage->Open(ImageFile,_imageData): _bitmapImage->Open(ImageFile);
	if(!result)
	{
		ErrorMessage(tkCANT_OPEN_FILE);
		return false;
	}
	
	//set the ImageData sizes and types according to the bitmap file 
	//that was just opened
	_height = _bitmapImage->getHeight();
	_width = _bitmapImage->getWidth();
	_imgType = BITMAP_FILE;
	_fileName = ImageFile;

	//try to open a world file for the image (if it fails, keep going)
	int LocationOfPeriod = ImageFile.ReverseFind('.');
	int NameLength = ImageFile.GetLength();
	CStringW WorldFileName = ImageFile.Left(LocationOfPeriod);
	CStringW ext = ImageFile.Right(NameLength - LocationOfPeriod - 1);
	
	//Try all three worldfile naming conventions
	WorldFileName += "." + ext + "w";
	if (! (ReadWorldFile(WorldFileName)) )
	{
		WorldFileName = ImageFile.Left(LocationOfPeriod);
		WorldFileName += "." + ext.Left(1) + ext.Right(1) + "w";
		if (! (ReadWorldFile(WorldFileName)) )
		{
			WorldFileName = ImageFile.Left(LocationOfPeriod);
			WorldFileName += ".wld";
			ReadWorldFile(WorldFileName);
		}
	} 

	long val;
	get_Value( 0, 0, &val );					
	_transColor = val;
	_transColor2 = val;
	return true;
}

// ***********************************************************************
//		SetVisibleExtents()
// ***********************************************************************
// Added by Rob Cairns 5 Nov 2005
// Reloads the image buffer. It's time consuming routine and should be called carefully
// Called from Map.ReloadImageBuffers (should be called after every FireExtentsChanged)
STDMETHODIMP CImageClass::SetVisibleExtents(double newMinX, double newMinY,	double newMaxX, double newMaxY, long pixInView, float transPercent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		
	if (_gdal)
	{
		double mapUnitsPerScreenPixel = sqrt((newMaxX - newMinX) * (newMaxY - newMinY) / pixInView);
		Extent screenExtents = Extent(newMinX, newMaxX, newMinY, newMaxY);

		if (_raster->LoadBuffer(&_imageData, screenExtents, _fileName, _downsamplingMode, _setRGBToGrey, mapUnitsPerScreenPixel))
		{
			//Repeated here because _rasterImage->LoadImageBuffer changes width and height
			_height = _raster->GetHeight();
			_width = _raster->GetWidth();
			_dX = _raster->GetDX();
			_dY = _raster->GetDY();
			_xllCenter = _raster->GetXllCenter();
			_yllCenter = _raster->GetYllCenter();

			_dataLoaded = true;
		}
	}
	_bufferReloadIsNeeded = false;
	return S_OK;
}

// ************************************************************
//	  IsGdalImageAvailable()
// ************************************************************
bool CImageClass::IsGdalImageAvailable()
{
	if ( _gdal && _raster)
	{
		return true;
	}
	else
	{
		ErrorMessage(tkAPPLICABLE_GDAL_ONLY);
		return false;
	}
}

// ***********************************************************************
//		get_Picture()
// ***********************************************************************
STDMETHODIMP CImageClass::get_Picture(IPictureDisp **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_width <= 0 || _height <= 0)
	{
		*pVal = NULL;
		return S_OK;
	}

	HDC desktop = GetDC(GetDesktopWindow());
	HDC compatdc = CreateCompatibleDC(desktop);
	HBITMAP bmp = CreateCompatibleBitmap(desktop, _width, _height);
	HGDIOBJ oldobj = SelectObject(compatdc, bmp);
	VARIANT_BOOL vbretval;
	GetImageBitsDC((long)compatdc, &vbretval);
	DeleteDC(compatdc);
	ReleaseDC(GetDesktopWindow(), desktop);

	PICTDESC pd;
	pd.cbSizeofstruct = sizeof(PICTDESC);
	pd.picType = PICTYPE_BITMAP;
	pd.bmp.hbitmap = bmp;
	pd.bmp.hpal = NULL;

	OleCreatePictureIndirect(&pd, IID_IPictureDisp, TRUE, (void**)pVal);

	return S_OK;
}

# include <ATLCTL.H>
STDMETHODIMP CImageClass::putref_Picture(IPictureDisp *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( newVal == NULL )
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}
	
	IPicture * pic;
	short type;

	newVal->QueryInterface(IID_IPicture,(void**)&pic);
	pic->get_Type(&type);

	switch ( type )
	{
		case PICTYPE_UNINITIALIZED:
			break;

		case PICTYPE_NONE:
			break;

		default:
			//Variable Definitions
			HDC hdc, hdcold;
			SIZE size, bmpsize;
			VARIANT_BOOL vbretval;			

			//Dimensions  - Conversion
			pic->get_Width(&size.cx);
			pic->get_Height(&size.cy);
			AtlHiMetricToPixel(&size,&bmpsize);
			
			//New DC
			hdc = CreateCompatibleDC(GetDC(GetDesktopWindow()));
			
			//Change the dc of the IPicture
			pic->SelectPicture(hdc,&hdcold,NULL);

			SetImageBitsDC((long)hdc,&vbretval);

			//Reset the dc of the IPicture
			pic->SelectPicture(hdcold,NULL,NULL);

			//Delete DC
			DeleteDC(hdc);
			break;
	}
	pic->Release();
	pic = NULL;

	return S_OK;
}

// *****************************************************************
//		GetImageBitsDC()
// *****************************************************************
STDMETHODIMP CImageClass::GetImageBitsDC(long hDC, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (_gdal && !_dataLoaded)
	{
		// excluded the code for loading the buffer; I prefer to separate
		// the operations of loading buffer and subsequent calls
		ErrorMessage(tkIMAGE_BUFFER_IS_EMPTY);
		*retval = VARIANT_FALSE;
		return S_OK;
	}
	
	HBITMAP hBMP;
	hBMP = (HBITMAP)GetCurrentObject((HDC)hDC,OBJ_BITMAP);
		
	if( hBMP == NULL )
	{	
		*retval = VARIANT_FALSE;
		return S_OK;
	}

	BITMAP bm;
	if(! GetObject(hBMP,sizeof(BITMAP),(void*)&bm) )
	{	
		*retval = VARIANT_FALSE;
		return S_OK;
	}

	if( bm.bmWidth != _width || bm.bmHeight != _height )
	{	
		*retval = VARIANT_FALSE;
		return S_OK;
	}

	int pad = ImageHelper::GetRowBytePad(_width, _bitsPerPixel);

	int bytesPerPixel = _bitsPerPixel / 8;		

	unsigned char * bits = new unsigned char[(_width * bytesPerPixel + pad) * _height];

	if( _inRam )
	{	
		long rowLength = _width * bytesPerPixel + pad;
		long loc = 0;
		for(int i = _height; i >= 0; i-- )
		{	
			memcpy(&(bits[loc]),&(_imageData[i*_width]),_width*sizeof(colour));
			loc += rowLength;
		}
	}
	else
	{
		long clr = RGB(0,0,0);
		long rowLength = _width * bytesPerPixel + pad;

		for(int j = 0; j < _height; j++ )
		{	
			for(int i = 0; i < _width; i++ )
			{	
				// can be optimize; why doing it per-byte
				get_Value(_height-j-1, i, &clr);
				bits[j*rowLength + i * 3] = GetBValue(clr);					
				bits[j*rowLength + i * 3 + 1] = GetGValue(clr);
				bits[j*rowLength + i * 3 + 2] = GetRValue(clr);				
				bits[j*rowLength + i * 3 + 3] = GET_ALPHA(clr);
			}
		}
	}
		
	BITMAPINFO bif;
	BITMAPINFOHEADER bih;
	bih.biBitCount = _bitsPerPixel;
	bih.biWidth = _width;
	bih.biHeight = _height;
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biCompression = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	bih.biSizeImage = (_width * bytesPerPixel + pad) * _height;
	bif.bmiHeader = bih;

	SetDIBitsToDevice((HDC)hDC,0,0,_width,_height,0,0,0,_height,bits,&bif,DIB_RGB_COLORS);
			
	*retval = VARIANT_TRUE;
	delete [] bits; 

	return S_OK;
}

// ****************************************************************
//		SetDCBitsToImage
// ****************************************************************
// Used with DIBSection bits in Map::Snapshot3.
bool CImageClass::SetDCBitsToImage(long hDC, BYTE* bits)
{
	HBITMAP hBMP = (HBITMAP)GetCurrentObject((HDC)hDC, OBJ_BITMAP);
	if (hBMP == NULL) {
		return false;
	}

	BITMAP bm;
	if (!GetObject(hBMP, sizeof(BITMAP), (void*)&bm)) {
		return false;
	}

	// creating resulting image	
	VARIANT_BOOL vb;
	CreateNew(bm.bmWidth, bm.bmHeight, &vb);
	if (!vb) return false;
		
	// copying the bytes
	long loc = 0;
	long paddedWidth = bm.bmWidthBytes;

	// TODO: alpha byte should be inserted for each pixel
	for(int i = 0; i < _height; i++)
	{	
		memcpy(&(_imageData[i*_width]), &(bits[loc]), bm.bmWidth * 3);
		loc += paddedWidth;
	}

	return true;
}

// ****************************************************************
//		DCBitsToImageBuffer
// ****************************************************************
bool CImageClass::DCBitsToImageBuffer(HDC hDC)
{
	HBITMAP hBMP = (HBITMAP)GetCurrentObject(hDC,OBJ_BITMAP);
	if( hBMP == NULL ) return false;

	BITMAP bm;
	if (!GetObject(hBMP, sizeof(BITMAP), (void*)&bm)) {
		return false;
	}
	
	VARIANT_BOOL vb;
	CreateNew(bm.bmWidth,bm.bmHeight,&vb);
	if (!vb) {
		return false;
	}
	
	int bitsPerPixel = 32;
	int bytesPerPixel = bitsPerPixel / 8;
	
	if (bm.bmBitsPixel == bitsPerPixel)
	{
		DCBitsToImageBuffer32(hBMP, bm, bytesPerPixel);
	}
	else
	{
		DCBitsToImageBufferWithPadding(hDC, hBMP, bitsPerPixel);
	}
	
	return true;
}

// ****************************************************************
//		DCBitsToImageBufferWithPadding
// ****************************************************************
void CImageClass::DCBitsToImageBufferWithPadding(HDC hdc, HBITMAP hBMP, int bitsPerPixel)
{
	int bytesPerPixel = bitsPerPixel * 8;

	int pad = ImageHelper::GetRowBytePad(_width, bitsPerPixel);
	long sizeBMP = (_width* bytesPerPixel + pad)*_height;

	BITMAPINFO bif;
	BITMAPINFOHEADER bih;
	bih.biBitCount = bitsPerPixel;
	bih.biWidth = _width;
	bih.biHeight = _height;
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biCompression = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	bih.biSizeImage = sizeBMP;
	bif.bmiHeader = bih;

	BYTE * bits = new BYTE[sizeBMP];

	GetDIBits((HDC)hdc, hBMP, 0, _height, bits, &bif, DIB_RGB_COLORS);

	long paddedWidth = _width * bytesPerPixel + pad;

	long loc = 0;
	for (int i = 0; i < _height; i++)
	{
		memcpy(&(_imageData[i*_width]), &(bits[loc]), _width * bytesPerPixel);
		loc += paddedWidth;
	}

	delete[] bits;
}

// ****************************************************************
//		DCBitsToImageBuffer32
// ****************************************************************
void CImageClass::DCBitsToImageBuffer32(HBITMAP hBMP, BITMAP& bm, int bytesPerPixel)
{
	long sizeBMP = bm.bmWidthBytes * bm.bmHeight;
	BYTE * bits = new BYTE[sizeBMP];
	GetBitmapBits(hBMP, sizeBMP, bits);

	long loc = 0;
	long paddedWidth = bm.bmWidthBytes;

	for (int i = 0; i <= _height; i++)
	{
		memcpy(&(_imageData[i * _width]), &(bits[loc]), bm.bmWidth * bytesPerPixel);
		loc += paddedWidth;
	}
}

// ****************************************************************
//		SetDCBitsToImage
// ****************************************************************
// Copies bits form the device context to image buffer
STDMETHODIMP CImageClass::SetImageBitsDC(long hDC, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;
	bool result = DCBitsToImageBuffer((HDC)hDC);
	*retval = result ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ****************************************************************
//		SetProjection
// ****************************************************************
// Chris Michaelis June 27 2006 for Ted Dunsford
STDMETHODIMP CImageClass::SetProjection(BSTR Proj4, VARIANT_BOOL * retval)
{
	USES_CONVERSION;

	// TODO: use GeoProjection class

	char * projection = W2A(Proj4);

	// Attempt to write it to the .prj file
	*retval = VARIANT_FALSE;
	try
	{
		CStringW projectionFilename = Utility::GetProjectionFilename(_fileName);
		if (projectionFilename != "")
		{
			FILE * prjFile = NULL;
			prjFile = _wfopen(projectionFilename, L"wb");
			if (prjFile)
			{
				char * wkt;
				ProjectionTools * p = new ProjectionTools();
				p->ToESRIWKTFromProj4(&wkt, projection);

				fprintf(prjFile, "%s", wkt);
				fclose(prjFile);
				prjFile = NULL;
				delete p; //added by Lailin Chen 12/30/2005
				*retval = VARIANT_TRUE;
			}
		}
	}
	catch(...)
	{
	}

	return S_OK;
}

// ****************************************************************
//		GetProjection
// ****************************************************************
STDMETHODIMP CImageClass::GetProjection(BSTR * Proj4)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_projection->ExportToProj4(Proj4);
	return S_OK;
}

STDMETHODIMP CImageClass::get_OriginalWidth(LONG* OriginalWidth)
{   
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*OriginalWidth = _gdal ? _raster->GetOrigWidth() : _width;
	return S_OK;
}

STDMETHODIMP CImageClass::get_OriginalHeight(LONG* OriginalHeight)
{  
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*OriginalHeight = _gdal ? _raster->GetOrigHeight() : _height;
	return S_OK;
}

STDMETHODIMP CImageClass::get_AllowHillshade(VARIANT_BOOL * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdal ? _raster->GetAllowHillshade() : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CImageClass::put_AllowHillshade(VARIANT_BOOL newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdal)
	{
		_raster->SetAllowHillshade(newValue == VARIANT_TRUE);
		_bufferReloadIsNeeded = true;
	}
	else
	{
		ErrorMsg(tkAPPLICABLE_GDAL_ONLY);
	}
	return S_OK;
}

STDMETHODIMP CImageClass::get_SetToGrey(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _setRGBToGrey;
	return S_OK;
}

STDMETHODIMP CImageClass::put_SetToGrey(VARIANT_BOOL newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_setRGBToGrey = (newValue == VARIANT_TRUE);
	_bufferReloadIsNeeded = true;
	
	return S_OK;
}

STDMETHODIMP CImageClass::get_UseHistogram(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdal ? _raster->GetUseHistogram(): VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CImageClass::put_UseHistogram(VARIANT_BOOL newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdal)
	{
		_raster->SetUseHistogram(newValue == VARIANT_TRUE);
		_bufferReloadIsNeeded = true;
	}
	else
	{
		ErrorMsg(tkAPPLICABLE_GDAL_ONLY);
	}
	return S_OK;
}

STDMETHODIMP CImageClass::get_HasColorTable(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdal)
	{
		*pVal = _raster->GetHasColorTable() ? VARIANT_TRUE : VARIANT_FALSE;
	}
	else
		*pVal = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CImageClass::get_PaletteInterpretation(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if (_gdal)
	{
		switch (_raster->GetPaletteInterpretation())
		{
			case GPI_Gray:
				*pVal = A2BSTR("Grayscale"); break;
			case GPI_RGB:
				*pVal = A2BSTR("Red, Green, Blue and Alpha"); break;
			case GPI_CMYK:
				*pVal = A2BSTR("Cyan, Magenta, Yellow and Black"); break;
			case GPI_HLS:
				*pVal = A2BSTR("Hue, Lightness and Saturation"); break;
			default:
				*pVal = A2BSTR("Unknown"); break;
		}
	}
	else
		*pVal = A2BSTR("Unknown");

	return S_OK;
}

STDMETHODIMP CImageClass::get_BufferSize(int *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdal ? _raster->GetImageQuality() : 100;
	return S_OK;
}

STDMETHODIMP CImageClass::put_BufferSize(int newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdal)
	{
		_raster->SetImageQuality(newValue);
		_bufferReloadIsNeeded = true;
	}
	else
	{
		ErrorMsg(tkAPPLICABLE_GDAL_ONLY);
	}
	return S_OK;
}

STDMETHODIMP CImageClass::get_NoBands(int *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdal ? _raster->GetNoBands() : NULL;
	return S_OK;
}


// ****************************************************************
//		get_ImageData
// ****************************************************************
unsigned char* CImageClass::get_ImageData()
{
	return _imageData ? reinterpret_cast<unsigned char*>(_imageData) : NULL;
}

void CImageClass::put_ImageData(colour* data)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_imageData)
	{
		delete[] _imageData;
		_imageData = NULL;
	}
	_imageData = data;
	return;
}

// **************************************************************
//		ClearBuffer
// **************************************************************
void CImageClass::ClearBuffer()
{
	if( _imageData )
	{
		delete[] _imageData;
		_imageData = NULL;
	}

	_imageData = new colour[1];
	_height = 1;
	_width = 1;

	if (_raster)
	{
		_dX = _raster->GetOrigDx();
		_dY = _raster->GetOrigDy();
	}
	else
	{
		_dX = _dY = 1;
	}
}

// **************************************************************
//		ClearGDALCache
// **************************************************************
STDMETHODIMP CImageClass::get_ClearGDALCache(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _gdal ? _raster->GetClearGdalCache() : VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CImageClass::put_ClearGDALCache(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_gdal)
	{
		_raster->SetClearGdalCache(newVal ? true : false);
	}
	else
		ErrorMessage(tkAPPLICABLE_GDAL_ONLY);
	return S_OK;
}

// **************************************************************
//		TransparencyPercent
// **************************************************************
STDMETHODIMP CImageClass::get_TransparencyPercent(double* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal =  _transparencyPercent;
	return S_OK;
}

STDMETHODIMP CImageClass::put_TransparencyPercent(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal < 0.0)	newVal = 0.0;
	if (newVal > 1.0)	newVal = 1.0;
	_transparencyPercent = newVal;
	_bufferReloadIsNeeded = true;
	return S_OK;
}

// **************************************************************
//		DownsamplingMode
// **************************************************************
STDMETHODIMP CImageClass::get_DownsamplingMode(tkInterpolationMode* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _downsamplingMode;
	return S_OK;
}
STDMETHODIMP CImageClass::put_DownsamplingMode(tkInterpolationMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_downsamplingMode = newVal;
	_bufferReloadIsNeeded = true;
	return S_OK;
}

// **************************************************************
//		UpsamplingMode
// **************************************************************
STDMETHODIMP CImageClass::get_UpsamplingMode(tkInterpolationMode* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _upsamplingMode;
	return S_OK;
}
STDMETHODIMP CImageClass::put_UpsamplingMode(tkInterpolationMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_upsamplingMode = newVal;
	_bufferReloadIsNeeded = true;
	return S_OK;
}

// **************************************************************
//		Drawing method
// **************************************************************
STDMETHODIMP CImageClass::get_DrawingMethod(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = 0;
	return S_OK;
}
STDMETHODIMP CImageClass::put_DrawingMethod(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

bool CImageClass::get_BufferIsDownsampled()
{
	return _downSampling;
}

// **************************************************************
//		BuildOverviews
// **************************************************************
STDMETHODIMP CImageClass::BuildOverviews (tkGDALResamplingMethod ResamplingMethod, int numOverviews, SAFEARRAY* OverviewList, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = VARIANT_FALSE;

	int count = (int)OverviewList->rgsabound[0].cElements;
	if (count < 0 || count != numOverviews)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}

	if (!_gdal)
	{
		ErrorMessage(tkAPPLICABLE_GDAL_ONLY);
		return S_OK;
	}

	int* overviewList = (int*)OverviewList->pvData;
	GDALDataset* dataset = _raster->GetDataset();
	if (dataset)
	{
		if (GdalHelper::BuildOverviewsCore(dataset, ResamplingMethod, overviewList, numOverviews, _globalCallback)) {
			*retval = VARIANT_TRUE;
		}
		else {
			ErrorMessage(tkUNSUPPORTED_FORMAT);
		}
	}
	
	return S_OK;
}

/***********************************************************************/
/*			get_Labels/putLabels()
/***********************************************************************/
//  Returns reference to Labels class
 STDMETHODIMP CImageClass::get_Labels(ILabels** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _labels;
	if (_labels != NULL)
		_labels->AddRef();
	return S_OK;
};

STDMETHODIMP CImageClass::put_Labels(ILabels* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal == NULL)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}
	
	ComHelper::SetRef(newVal, (IDispatch**)&_labels, false);
	return S_OK;
};

// ********************************************************************
//		get_Extents()
// ********************************************************************
STDMETHODIMP CImageClass::get_Extents(IExtents** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	IExtents * bBox = NULL;
	ComHelper::CreateExtents(&bBox);
	
	double minX, minY, maxX, maxY;
	if( _gdal )
	{
		minX = _raster->GetOrigXllCenter() - _raster->GetOrigDx()/2;
		minY = _raster->GetOrigYllCenter() - _raster->GetOrigDy()/2;
		maxX = _raster->GetOrigXllCenter() + _raster->GetOrigDx() * _raster->GetOrigWidth();
		maxY = _raster->GetOrigYllCenter() + _raster->GetOrigDy() * _raster->GetOrigHeight();
	}
	else
	{
		minX = _xllCenter - _dX/2;
		minY = _yllCenter - _dY/2;
		maxX = _xllCenter + _dX * _width;
		maxY = _yllCenter + _dY * _height;
	}

	bBox->SetBounds(minX,minY,0,maxX,maxY,0);
	bBox->SetMeasureBounds(0,0);
	*pVal = bBox;
	return S_OK;
}

// ******************************************************************
//	   SaveNotNullPixels()
// ******************************************************************
void CImageClass::ClearNotNullPixels()
{
	if ( m_pixels )
	{
		delete[] m_pixels; 
		m_pixels = NULL;
		m_pixelsCount = 0;
	}
}

// ******************************************************************
//	   SaveNotNullPixels()
// ******************************************************************
// Saves all pixels with values different from traspColor to the m_pixels array
bool CImageClass::SaveNotNullPixels(bool forceSaving)
{
	ClearNotNullPixels();
	
	if (! _canUseGrouping && !forceSaving)
		return false;

	if (_imgType == BITMAP_FILE || _imgType == USE_FILE_EXTENSION)
		return false;
	
	if (!_raster) return false;

	_width = _raster->GetOrigWidth();
	_height = _raster->GetOrigHeight();

	if (_width <= 0 || _height <= 0 || (_width * _height > 1048576.0 * 100.0 / 3.0 ))	// 100 MB
		return false;
	
	// the maximum part of non-transparent pixels
	double part = 0.1;
	if (forceSaving)
		part = 1.0;

	DataPixels* tmpData = new DataPixels[(int)((double)_width * (double)_height * part) + _width];	// we don't know how many pixels would be, but not greater than this value
	colour* val;
	bool result = false;

	// perhaps it makes sense to limit the size of buffer
	if (_raster->LoadBufferFull(&_imageData, _fileName, 0))
	{
		unsigned char red = GetRValue(_transColor);
		unsigned char green = GetGValue(_transColor);
		unsigned char blue = GetBValue(_transColor);

		int count = 0;
		int maxPixels = (int)((double)_width * (double)_height * part);	// this method will work only when pixles are scarce, therefore we set 10% as maximum 
		
		// there is single transparent color
		if (_transColor == _transColor2)
		{
			for (int i = 0; i < _height; i++)
			{
				int offset = i * _width;
				for (int j = 0; j < _width; j++)
				{
					val = _imageData + offset + j;

					if ( val->red != red || 
						 val->green != green || 
						 val->blue != blue )
					{
						tmpData[count].value = *val;
						tmpData[count].position = offset + j;
						count++;
					}
				}
				if (count > maxPixels)
					break;
			}
		}
		else
		{
			// there is a range of transparent colors, saving data pixels will be slower
			unsigned char minRed, minGreen, minBlue, maxRed, maxGreen, maxBlue;
			minRed = MIN(GetRValue(_transColor),GetRValue(_transColor2));
			minGreen = MIN(GetGValue(_transColor),GetGValue(_transColor2));
			minBlue = MIN(GetBValue(_transColor),GetBValue(_transColor2));
			maxRed = MAX(GetRValue(_transColor),GetRValue(_transColor2));
			maxGreen = MAX(GetGValue(_transColor),GetGValue(_transColor2));
			maxBlue = MAX(GetBValue(_transColor),GetBValue(_transColor2));

			for (int i = 0; i < _height; i++)
			{
				int offset = i * _width;
				for (int j = 0; j < _width; j++)
				{
					val = _imageData + offset + j;

					if ( val->red >= minRed && val->red <= maxRed &&
						 val->green >= minGreen && val->green <= maxGreen &&
						 val->blue >= minBlue && val->blue <= maxBlue)
					{
						tmpData[count].value = *val;
						tmpData[count].position = offset + j;
						count++;
					}
				}
				if (count > maxPixels)
					break;
			}
		}

		// copying pixels to the permanent structure
		if ( count < maxPixels)	
		{
			if (count > 0)
			{
				m_pixels = new DataPixels[count];
				memcpy(m_pixels, tmpData, sizeof(DataPixels) * count);
			}
			m_pixelsCount = count;
			_pixelsSaved = true;
			result = true;
		}
		else
		{
			m_pixelsCount = 0;
			result = false;
		}
		
		// cleaning
		if( _imageData )
		{
			delete[] _imageData;
			_imageData = NULL;
		}
		_width = 0;
		_height = 0;			
	}
	
	delete[] tmpData;
	return result;
}

// ********************************************************************
//						ErrorMessage()						         
// ********************************************************************
void CImageClass::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("Image", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// **************************************************************
//		ProjectionToImage
// **************************************************************
// Returns image coordinates to the given map coordinates
STDMETHODIMP CImageClass::ProjectionToImage(double ProjX, double ProjY, long* ImageX, long* ImageY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_gdal)
	{
		*ImageX = Utility::Rint((ProjX - _raster->GetOrigXllCenter())/_raster->GetOrigDx());
		*ImageY = Utility::Rint((double)_raster->GetOrigHeight() - 1 - (((ProjY - _raster->GetOrigYllCenter())/_raster->GetOrigDy())));
	}
	else
	{
		*ImageX = Utility::Rint((ProjX - _xllCenter)/_dX);
		*ImageY = Utility::Rint((double)_height - 1 - ((ProjY - _yllCenter)/_dY));
	}
	return S_OK;
}

// **************************************************************
//		ImageToProjection
// **************************************************************
// Returns map coordinates to the given image coordinates
STDMETHODIMP CImageClass::ImageToProjection(long ImageX, long ImageY, double* ProjX, double* ProjY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (_gdal)
	{
		*ProjX = _raster->GetOrigXllCenter() + (ImageX - 0.5) * _raster->GetOrigDx();
		*ProjY = _raster->GetOrigYllCenter() + (_raster->GetOrigHeight() - 1 - ImageY + 0.5) * _raster->GetOrigDy();
	}
	else
	{
		*ProjX = _xllCenter + (ImageX - 0.5) * _dX;
		*ProjY = _yllCenter + (_height - 1 - ImageY + 0.5) * _dY;
	}
	return S_OK;
}

// **************************************************************
//		ProjectionToBuffer
// **************************************************************
// Returns image coordinates to the given map coordinates
STDMETHODIMP CImageClass::ProjectionToBuffer(double ProjX, double ProjY, long* BufferX, long* BufferY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*BufferX = Utility::Rint((ProjX - _xllCenter)/_dX);
	*BufferY = Utility::Rint((double)_height - 1 - ((ProjY - _yllCenter)/_dY));
	return S_OK;
}

// **************************************************************
//		ImageToProjection
// **************************************************************
// Returns map coordinates to the given image coordinates
STDMETHODIMP CImageClass::BufferToProjection(long BufferX, long BufferY, double* ProjX, double* ProjY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*ProjX = _xllCenter + (BufferX - 0.5) * _dX;
	*ProjY = _yllCenter + (_height - 1 - BufferY + 0.5) * _dY;
	return S_OK;
}

// **************************************************************
//		get/put_CanUseGrouping()
// **************************************************************
STDMETHODIMP CImageClass::get_CanUseGrouping (VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal =  (VARIANT_BOOL)_canUseGrouping;
	return S_OK;
}
STDMETHODIMP CImageClass::put_CanUseGrouping(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_canUseGrouping = newVal?true:false;
	return S_OK;
}

// --------------------------------------------------------------
//					POSITION AND SCALE
// --------------------------------------------------------------
/* Following scheme is used. 
   
   'Original' properties return:
   - parameters of the whole image. They change position (XllCetner,
   YllCenter) and scale (DX, DY) of both bitmap and GDAL-based images.
   
   'Common' properties return:
   - parameters of the whole image for bitmaps; the changes
   can be done here as well; according to logic I would better forbid
   the changes for consistency with GDAL images, but this would break
   a lot of code; perhaps it makes sense to return error code urging
   the developer to use 'original' properties
   - parameters of image buffer for GDAL-based images; no 
   changes are allowed in this case.
*/

// --------------------------------------------------------------
//		ORIGINAL PROPERTIES - PARAMETERS OF THE WHOLE IMAGE
// --------------------------------------------------------------

// **************************************************************
//	 get/put_OriginalXllCenter
// **************************************************************
STDMETHODIMP CImageClass::get_OriginalXllCenter( double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _gdal && _raster ? _raster->GetOrigXllCenter() : _xllCenter;
	return S_OK;
}

STDMETHODIMP CImageClass::put_OriginalXllCenter( double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_gdal && _raster)
	{
		_raster->SetOrigXllCenter(newVal);
		_raster->RefreshExtents();
	}
	else
		_xllCenter = newVal;	// for bitmaps we'll use common values
	return S_OK;
}

// **************************************************************
//	 get/put_OriginalYllCenter
// **************************************************************
STDMETHODIMP CImageClass::get_OriginalYllCenter( double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _gdal && _raster ? _raster->GetOrigYllCenter() : _yllCenter;
	return S_OK;
}

STDMETHODIMP CImageClass::put_OriginalYllCenter( double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_gdal && _raster)
	{		
		_raster->SetOrigYllCenter(newVal);
		_raster->RefreshExtents();
	}
	else
		_yllCenter = newVal;	// for bitmaps we'll use common values
	return S_OK;
}

// **************************************************************
//	 get/put_OriginalDX
// **************************************************************
STDMETHODIMP CImageClass::get_OriginalDX( double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _gdal && _raster ? _raster->GetOrigDx() : _dX;
	return S_OK;
}

STDMETHODIMP CImageClass::put_OriginalDX( double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if ( newVal > 0.0)
	{
		if (_gdal && _raster)
		{
			_raster->SetOrigDx(newVal);
			_raster->RefreshExtents();
		}
		else
			_dX = newVal;	// for bitmaps we'll use common values
	}
	else
		ErrorMessage(tkINVALID_DX);
	return S_OK;
}

// **************************************************************
//	 get/put_OriginalDY
// **************************************************************
STDMETHODIMP CImageClass::get_OriginalDY( double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _gdal && _raster ? _raster->GetOrigDy() : _dY;
	return S_OK;
}

STDMETHODIMP CImageClass::put_OriginalDY( double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if ( newVal > 0.0)
	{
		if (_gdal && _raster)
		{
			_raster->SetOrigDy(newVal);
			_raster->RefreshExtents();
		}
		else
			_dY = newVal;	// for bitmaps we'll use common values
	}
	else
		ErrorMessage(tkINVALID_DY);
	return S_OK;
}

// --------------------------------------------------------------
//		COMMON PROPERTIES - PARAMETERS OF THE IMAGE BUFFER
// --------------------------------------------------------------

// **************************************************************
//	 get/put_YllCenter
// **************************************************************
STDMETHODIMP CImageClass::get_YllCenter(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _yllCenter;
	return S_OK;
}
STDMETHODIMP CImageClass::put_YllCenter(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdal)
	{
		// do nothing: it doesn't make sense to change parameters of buffer
		ErrorMessage(tkNOT_APPLICABLE_TO_GDAL);
	}
	else
		_yllCenter = newVal;
	return S_OK;
}

// **************************************************************
//	 get/put_XllCenter
// **************************************************************
STDMETHODIMP CImageClass::get_XllCenter(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _xllCenter;
	return S_OK;
}

STDMETHODIMP CImageClass::put_XllCenter(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdal)
	{
		// do nothing: it doesn't make sense to change parameters of buffer
		ErrorMessage(tkNOT_APPLICABLE_TO_GDAL);
	}
	else
		_xllCenter = newVal;
	return S_OK;
}

// **************************************************************
//	 get/put_dY
// **************************************************************
STDMETHODIMP CImageClass::get_dY(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _dY;
	return S_OK;
}

STDMETHODIMP CImageClass::put_dY(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdal)
	{
		// do nothing: it doesn't make sense to change parameters of buffer
		ErrorMessage(tkNOT_APPLICABLE_TO_GDAL);
	}
	else
	{
		if( newVal > 0.0 )
		{	
			_dY = newVal;
		}
		else
			ErrorMessage(tkINVALID_DY);
	}
	return S_OK;
}

// **************************************************************
//	 get/put_dX
// **************************************************************
STDMETHODIMP CImageClass::get_dX(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _dX;
	return S_OK;
}

STDMETHODIMP CImageClass::put_dX(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdal)
	{
		// do nothing: it doesn't make sense to change parameters of buffer
		ErrorMessage(tkNOT_APPLICABLE_TO_GDAL);
	}
	else
	{
		if( newVal > 0.0 )
		{	
			_dX = newVal;
		}
		else
			ErrorMessage(tkINVALID_DX);
	}
	return S_OK;
}

// **************************************************************
//	 GetUniqueColors
// **************************************************************
STDMETHODIMP CImageClass::GetUniqueColors (double MaxBufferSizeMB, VARIANT* Colors, VARIANT* Frequencies, LONG* Count)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	colour* data = NULL;
	if ( _gdal )
	{
		// TODO: opening for the second time isn't good enough; reconsider
		
		// we'll open dataset second time, to keep an existing buffer untouched
		GdalRaster* raster = new GdalRaster();
		if ( raster->Open(_fileName) )
		{
			if (raster->LoadBufferFull(&data, _fileName, MaxBufferSizeMB))
			{
				if (data)
				{
					long size = raster->GetWidth() * raster->GetHeight();
					BuildColorMap(data, size, Colors, Frequencies, Count);
					delete[] data;
					data = NULL;
				}
			}
			
			// deleting temporary raster
			raster->Close();
			delete raster; 
			raster = NULL;
		}
	}
	else
	{
		if ( _inRam )
		{
			// all data is here, we need to build histogram only
			BuildColorMap(_imageData, _width * _height, Colors, Frequencies, Count);
		}
		else
		{
			try
			{
				data = new colour[_width * _height];
			}
			catch(...)
			{
				ErrorMessage(tkFAILED_TO_ALLOCATE_MEMORY);
				return S_OK;
			}
			
			// we must read the values form disk
 			for (int j = 0; j < _height; j++)
			{
				for (int i = 0; i < _width; i++)
				{
					data[j * _width + i] = _bitmapImage->getValue(j,i);
				}
			}
			
			BuildColorMap(data, _width * _height, Colors, Frequencies, Count);
			delete[] data;
		}
	}
	return S_OK;
}

// **********************************************************
//		BuildColorMap()
// **********************************************************
bool CImageClass::BuildColorMap(colour* data, int size, VARIANT* Colors, VARIANT* Frequencies, long* count)
{
	*count = 0;
	
	if (size == 0) return false;
	
	std::map<OLE_COLOR, long> myMap;				// color as key and frequency as value
		
	// building list of colors and frequencies
	for (int i = 0; i < size; i++)
	{
		colour* p = data + i;
		OLE_COLOR clr = RGB(p->red, p->green, p->blue);
			
		if (myMap.find(clr) != myMap.end())
			myMap[clr] += 1;
		else									
			myMap[clr] = 1;
	}
		
	// sorting by frequency
	std::multimap <long, OLE_COLOR> sortedMap;
	std::map <OLE_COLOR, long>::iterator p = myMap.begin();
	while(p != myMap.end())
	{
		pair<long, OLE_COLOR> myPair(p->second, p->first);	
		sortedMap.insert(myPair);
		++p;
	}

	// returning result
	std::vector<OLE_COLOR> colors;
	std::vector<long> frequences;
	colors.resize(sortedMap.size()); 
	frequences.resize(sortedMap.size());

	std::multimap <long, OLE_COLOR>::reverse_iterator pp = sortedMap.rbegin();
	int i = 0;
	while(pp != sortedMap.rend())
	{
		colors[i] = pp->second;
		frequences[i] = pp->first;
		++pp; ++i;
	}
	*count = sortedMap.size();
		
	// converting to safe arrays
	return (Templates::Vector2SafeArray( &colors, VT_UI4, Colors) && Templates::Vector2SafeArray(&frequences, VT_I4, Frequencies));
	
}

// **************************************************************
//		SetNoDataValue()
// **************************************************************
STDMETHODIMP CImageClass::SetNoDataValue(double Value, VARIANT_BOOL* Result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())	
	if (_gdal && _raster)
	{
		GDALDataset* dataset = _raster->GetDataset();
		*Result = (VARIANT_BOOL)_raster->SetNoDataValue(Value);
	}
	else
		ErrorMessage(tkAPPLICABLE_GDAL_ONLY);
	
	return S_OK;
}

// **************************************************************
//		get_NumOverviews()
// **************************************************************
STDMETHODIMP CImageClass::get_NumOverviews(int* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())		

	*retval = 0;

	if (_gdal)
	{
		GDALRasterBand* band = _raster->GetBand(1);
		if (band)
		{
			 *retval = band->GetOverviewCount();
		}
	}

	return S_OK;
}

// **************************************************************
//		LoadBuffer()
// **************************************************************
// loads the whole buffer
STDMETHODIMP CImageClass::LoadBuffer(double maxBufferSize, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())		
	
	*retVal = VARIANT_FALSE;

	if (_sourceType == istGDIPlus)
	{
		if (_iconGdiPlus && _iconGdiPlus->m_bitmap)
		{
			Gdiplus::Bitmap* bmp = _iconGdiPlus->m_bitmap;
			int width = bmp->GetWidth();
			int height = bmp->GetHeight();

			if (width > 0 && height > 0)
			{
				_imageData = new colour[width * height];
				
				for ( int j = 0; j < height ; j++)
				{
					for (int i = 0; i < width; i++)
					{
						Gdiplus::Color pixel;
						bmp->GetPixel(i, j, &pixel); 
						colour clr(pixel.GetR(), pixel.GetG(), pixel.GetB());
  						memcpy(_imageData + ((height - 1 - j) * width + i), &clr, sizeof(colour));
					}
				}
			}
		}
	}
	else
	{
		if (this->IsGdalImageAvailable())
		{
			*retVal = _raster->LoadBufferFull(&(_imageData), _fileName, maxBufferSize);
			_height = _raster->GetWidth();
			_width = _raster->GetHeight();
			_dataLoaded = true;
		}
	}

	return S_OK;
}

// **************************************************************
//		get_SourceType()
// **************************************************************
STDMETHODIMP CImageClass::get_SourceType (tkImageSourceType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())		
	*retVal = _sourceType;
	return S_OK;
}

#pragma region Obsolete

// ***************************************************
//		Deprecated methods. Use 'Original' properties instead
// ***************************************************
STDMETHODIMP CImageClass::GetOriginalXllCenter(double *pVal)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdal && _raster ? _raster->GetOrigXllCenter() : *pVal = _xllCenter;
	return S_OK;
}

STDMETHODIMP CImageClass::GetOriginalYllCenter(double *pVal)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState()) 
	*pVal = _gdal && _raster ? _raster->GetOrigYllCenter() : _yllCenter;
	return S_OK;
}

STDMETHODIMP CImageClass::GetOriginal_dX(double *pVal)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdal && _raster ? _raster->GetOrigDx() : *pVal = _dX;
	return S_OK;
}

STDMETHODIMP CImageClass::GetOriginal_dY(double *pVal)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdal && _raster ? _raster->GetOrigDy() : *pVal = _dY;
	return S_OK;
}

STDMETHODIMP CImageClass::GetOriginalHeight(long *pVal)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdal && _raster ? _raster->GetOrigHeight() : _height;
	return S_OK;
}
STDMETHODIMP CImageClass::GetOriginalWidth(long *pVal)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdal ? _raster->GetOrigWidth() : _width;
	return S_OK;
}

// ********************************************************
//     get_Warped()
// ********************************************************
STDMETHODIMP CImageClass::get_Warped(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _gdal ? _raster->IsWarped() : VARIANT_FALSE;
	return S_OK;
}
#pragma endregion

#pragma region "Serialization"

// ********************************************************
//     Serialize()
// ********************************************************
STDMETHODIMP CImageClass::Serialize(VARIANT_BOOL SerializePixels, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CPLXMLNode* psTree = this->SerializeCore(SerializePixels, "ImageClass");
	Utility::SerializeAndDestroyXmlTree(psTree, retVal);
	return S_OK;
}

// ********************************************************
//     Serialize()
// ********************************************************
CPLXMLNode* CImageClass::SerializeCore(VARIANT_BOOL SerializePixels, CString ElementName)
{
	USES_CONVERSION;

	// checking the size of the buffer
	long fullWidth, fullHeight;
	this->get_OriginalWidth(&fullWidth);
	this->get_OriginalHeight(&fullHeight);

	if (SerializePixels && (fullWidth * fullHeight > 200000) )
	{
		ErrorMessage(tkICON_OR_TEXTURE_TOO_BIG);
		return NULL;		
	}

	CPLXMLNode* psTree = CPLCreateXMLNode( NULL, CXT_Element, ElementName);
	
	// properties
	if (_setRGBToGrey != false)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "SetToGrey", CPLString().Printf("%d", (int)_setRGBToGrey));
	if (_transColor != RGB(0,0,0))
		Utility::CPLCreateXMLAttributeAndValue(psTree, "TransparencyColor", CPLString().Printf("%d", _transColor));
	if (_transColor != _transColor2)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "TransparencyColor2", CPLString().Printf("%d", _transColor2));
	if (_useTransColor != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseTransparencyColor", CPLString().Printf("%d", (int)_useTransColor));
	if (_transparencyPercent != 1.0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "TransparencyPercent", CPLString().Printf("%f", _transparencyPercent));
	if (_downsamplingMode != imNone)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "DownsamplingMode", CPLString().Printf("%d", (int)_downsamplingMode));
	if (_upsamplingMode != imBilinear)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UpsamplingMode", CPLString().Printf("%d", (int)_upsamplingMode));
	
	PredefinedColorScheme colors;
	get_ImageColorScheme(&colors);
	if ( colors != FallLeaves)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ImageColorScheme", CPLString().Printf("%d", (int)colors));

	if (_gdal)
	{
		// GDAL only properties
		tkGridRendering allowExtScheme;
		this->get_AllowGridRendering(&allowExtScheme);
		if (allowExtScheme != tkGridRendering::grForGridsOnly)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "AllowGridRendering", CPLString().Printf("%d", (int)allowExtScheme));

		VARIANT_BOOL allowHillshade;
		this->get_AllowHillshade(&allowHillshade);
		if (allowHillshade != VARIANT_FALSE)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "AllowHillshade", CPLString().Printf("%d", (int)allowHillshade));

		int bufferSize;
		this->get_BufferSize(&bufferSize);
		if (bufferSize != 100.0)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "BufferSize", CPLString().Printf("%d", bufferSize));

		VARIANT_BOOL clearCache;
		this->get_ClearGDALCache(&clearCache);
		if (clearCache != VARIANT_FALSE)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "ClearGdalCache", CPLString().Printf("%d", (int)clearCache));

		int bandIndex;
		this->get_SourceGridBandIndex(&bandIndex);
		if (bandIndex != -1)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "SourceGridBandIndex", CPLString().Printf("%d", bandIndex));
		
		VARIANT_BOOL useHistogram;
		this->get_UseHistogram(&useHistogram);
		if (useHistogram != VARIANT_FALSE)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "UseHistogram", CPLString().Printf("%d", (int)useHistogram));
	}

	if (SerializePixels)	// check if the buffer is loaded
	{
		if (_sourceType == istGDIPlus)
		{
			// it's in-memory bitmap
			std::string s = _iconGdiPlus->SerializeToBase64String();
			CPLXMLNode* psBuffer = CPLCreateXMLElementAndValue(psTree, "ImageBuffer", s.c_str());
			Utility::CPLCreateXMLAttributeAndValue(psBuffer, "GdiPlusBitmap", CPLString().Printf("%d", (int)true));
		}
		else
		{
			bool useGDIPlus = false;
			if (_imgType == JPEG_FILE || _imgType == PNG_FILE || _imgType == GIF_FILE)
			{
				CComBSTR filename;
				this->get_Filename(&filename);
				USES_CONVERSION;
				long size = Utility::GetFileSize(OLE2CA(filename));
				if (size < (long)(0x1 << 20))
				{
					useGDIPlus = true;
				}
			}
			
			if (useGDIPlus)
			{
				// it will be serialized as GdiPlus and loaded as in-memory bitmap the next time
				CComBSTR filename;
				this->get_Filename(&filename);
				USES_CONVERSION;
				
				unsigned char* buffer = NULL;
				int size = Utility::ReadFileToBuffer(OLE2W(filename), &buffer);

				if (size > 0)
				{
					std::string s = base64_encode(buffer, size);
					
					CPLXMLNode* psBuffer = CPLCreateXMLElementAndValue(psTree, "ImageBuffer", s.c_str());
					Utility::CPLCreateXMLAttributeAndValue(psBuffer, "GdiPlusBitmap", CPLString().Printf("%d", (int)true));
					delete[] buffer;
				}
			}
			else
			{
				// only buffer will be serialized
				long bufferWidth, bufferHeight;
				
				if (_gdal)
				{
					this->get_Width(&bufferWidth);
					this->get_Height(&bufferHeight);

					if (bufferWidth != fullWidth || 
						bufferHeight != fullHeight)
					{
						VARIANT_BOOL vbretval;
						this->LoadBuffer(1, &vbretval);
					}
				}

				this->get_Width(&bufferWidth);
				this->get_Height(&bufferHeight);
				
				unsigned char* data = reinterpret_cast<unsigned char*>(_imageData);
				std::string s = base64_encode(data, bufferWidth * bufferHeight * 3);
				CPLXMLNode* psBuffer = CPLCreateXMLElementAndValue(psTree, "ImageBuffer", s.c_str());

				Utility::CPLCreateXMLAttributeAndValue(psBuffer, "GdiPlusBitmap", CPLString().Printf("%d", (int)false));
				Utility::CPLCreateXMLAttributeAndValue(psBuffer, "Width", CPLString().Printf("%d", bufferWidth));
				Utility::CPLCreateXMLAttributeAndValue(psBuffer, "Height", CPLString().Printf("%d", bufferHeight));
			}
		}
	}

	// labels
	if (!SerializePixels)	// if pixels are serialized, then it's icon or a texture;
							// it's obvious that no labels can be there
	{
		CPLXMLNode* psLabels = ((CLabels*)_labels)->SerializeCore("LabelsClass");
		if (psLabels)
		{
			CPLAddXMLChild(psTree, psLabels);
		}
	}
	return psTree;
}

// ********************************************************
//     DeserializeCore()
// ********************************************************
bool CImageClass::DeserializeCore(CPLXMLNode* node)
{
	SetDefaults();
	
	CString s;
	CPLXMLNode* nodeBuffer = CPLGetXMLNode(node, "ImageBuffer");
	if (nodeBuffer)
	{
		bool gdiPlus = false;
		s = CPLGetXMLValue( nodeBuffer, "GdiPlusBitmap", NULL );
		if (s != "") gdiPlus = atoi(s) == 0 ? false : true;
		
		if (gdiPlus)
		{
			std::string str = CPLGetXMLValue(nodeBuffer, "=ImageBuffer", NULL);
			if (str.size() != 0)
			{
				VARIANT_BOOL vbretval;

				CMemoryBitmap* bmp = new CMemoryBitmap();
				if (bmp->LoadFromBase64String(str))
				{
					this->CreateNew(bmp->get_Width(), bmp->get_Height(), &vbretval);
					_sourceType = istGDIPlus;
					_iconGdiPlus = bmp;
					this->LoadBuffer(1, &vbretval);
				}
				else
				{
					delete bmp;
					this->Close(&vbretval);
				}
			}
		}
		else
		{
			long width = 0, height = 0;
			s = CPLGetXMLValue( nodeBuffer, "Width", NULL );
			if (s != "") width = atoi(s);

			s = CPLGetXMLValue( nodeBuffer, "Height", NULL );
			if (s != "") height = atoi(s);
			
			if (width > 0 && height > 0 && 
				width * height < 200000)
			{
				std::string str = CPLGetXMLValue(nodeBuffer, "=ImageBuffer", NULL);
				if (str.size() != 0)
				{
					// restoring buffer
					VARIANT_BOOL vbretval;
					this->Close(&vbretval);
					
					this->CreateNew(width, height, &vbretval);
					_sourceType = istGDIPlus;
					str = base64_decode(str);
					const char* data = str.c_str();

					memcpy(_imageData, data, sizeof(unsigned char) * width * height * 3);
				}
			}
		}
	}

	s = CPLGetXMLValue( node, "SetToGrey", "0" );
	if (s != "") _setRGBToGrey = atoi(s) == 0 ? false : true;

	s = CPLGetXMLValue( node, "TransparencyColor", NULL );
	if (s != "") _transColor = (OLE_COLOR)atoi(s);

	s = CPLGetXMLValue( node, "TransparencyColor2", NULL );
	_transColor2 = s != "" ? (OLE_COLOR)atoi(s) : _transColor ;

	s = CPLGetXMLValue( node, "UseTransparencyColor", NULL );
	if (s != "") _useTransColor = (VARIANT_BOOL)atoi(s);

	s = CPLGetXMLValue( node, "TransparencyPercent", NULL );
	if (s != "") _transparencyPercent = Utility::atof_custom(s);

	s = CPLGetXMLValue( node, "DownsamplingMode", NULL );
	if (s != "") _downsamplingMode = (tkInterpolationMode)atoi(s);

	s = CPLGetXMLValue( node, "UpsamplingMode", NULL );
	if (s != "") _upsamplingMode = (tkInterpolationMode)atoi(s);

	// Labels
	CPLXMLNode* psChild = CPLGetXMLNode(node, "LabelsClass");
	if (psChild)
	{
		((CLabels*)_labels)->DeserializeCore(psChild);
	}

	// GridColorScheme
	if (_gdal)
	{
		tkGridRendering allowColorScheme;
		s = CPLGetXMLValue( node, "AllowGridRendering", "1" );		// 1 = grForGridsOnly
		if (s != "") allowColorScheme = (tkGridRendering)atoi(s);
		this->put_AllowGridRendering(allowColorScheme);

		VARIANT_BOOL allowHillshade;
		s = CPLGetXMLValue( node, "AllowHillshade", "1" );
		if (s != "") allowHillshade = (VARIANT_BOOL)atoi(s);
		this->put_AllowHillshade(allowHillshade);

		int bufferSize;
		s = CPLGetXMLValue( node, "BufferSize", "100" );
		if (s != "") bufferSize = atoi(s);
		this->put_BufferSize(bufferSize);

		VARIANT_BOOL clearCache;
		s = CPLGetXMLValue( node, "ClearGdalCache", "1" );
		if (s != "") clearCache = (VARIANT_BOOL)atoi(s);
		this->put_ClearGDALCache(clearCache);

		int bandIndex;
		s = CPLGetXMLValue( node, "SourceGridBandIndex", "1" );
		if (s != "") bandIndex = atoi(s);
		this->put_SourceGridBandIndex(bandIndex);

		VARIANT_BOOL useHistogram;
		s = CPLGetXMLValue( node, "UseHistogram", "1" );
		if (s != "") useHistogram = (VARIANT_BOOL)atoi(s);
		this->put_UseHistogram(useHistogram);

		PredefinedColorScheme colors;
		s = CPLGetXMLValue( node, "ImageColorScheme", "0" );
		if (s != "") colors = (PredefinedColorScheme)atoi(s);
		_raster->ApplyPredefinedColorScheme(colors);
	}
	return true;
}

// ********************************************************
//     Deserialize()
// ********************************************************
STDMETHODIMP CImageClass::Deserialize(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	CString s = OLE2CA(newVal);
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		CPLXMLNode* nodeImage = CPLGetXMLNode(node, "=ImageClass");
		if (nodeImage)
		{
			this->DeserializeCore(nodeImage);
		}
		CPLDestroyXMLNode(node);
	}
	return S_OK;
}
#pragma endregion

// ********************************************************
//     get_SourceGridName()
// ********************************************************
STDMETHODIMP CImageClass::get_SourceGridName(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = OLE2BSTR(this->sourceGridName);
	return S_OK;
}

// ********************************************************
//     get_SourceFilename()
// ********************************************************
STDMETHODIMP CImageClass::get_SourceFilename(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = OLE2BSTR(isGridProxy ? this->sourceGridName: _fileName);
	return S_OK;
}

// ********************************************************
//     get_IsGridProxy()
// ********************************************************
STDMETHODIMP CImageClass::get_IsGridProxy(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = isGridProxy ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

// ********************************************************
//     get_GridProxyColorScheme()
// ********************************************************
STDMETHODIMP CImageClass::get_GridProxyColorScheme(IGridColorScheme** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = NULL;
	if (isGridProxy)
	{
		CStringW legendName = GridManager::GetProxyLegendName(sourceGridName);
		if (Utility::FileExistsW(legendName))
		{
			IGridColorScheme* scheme = NULL;
			ComHelper::CreateInstance(idGridColorScheme, (IDispatch**)&scheme);
			VARIANT_BOOL vb;
			CComBSTR bstrName(legendName);
			CComBSTR bstrElement("GridColoringScheme");

			scheme->ReadFromFile(bstrName, bstrElement, &vb);
			if (!vb) {
				scheme->Release();
			}
			else {
				*retVal = scheme;
			}
		}
	}

	return S_OK;
}

// ********************************************************
//     get_GridRendering()
// ********************************************************
STDMETHODIMP CImageClass::get_GridRendering(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = _gdal ? _raster->WillBeRenderedAsGrid() : VARIANT_FALSE;

	return S_OK;
}

// ********************************************************
//     AllowGridRendering
// ********************************************************
STDMETHODIMP CImageClass::get_AllowGridRendering(tkGridRendering * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _gdal ? _raster->GetAllowAsGrid() : tkGridRendering::grForGridsOnly;

	return S_OK;
}

STDMETHODIMP CImageClass::put_AllowGridRendering(tkGridRendering newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_gdal)
	{
		if (_raster->GetAllowAsGrid() != newValue)
		{
			_raster->SetAllowAsGrid(newValue);
			_bufferReloadIsNeeded = true;
		}
	}
	else
	{
		ErrorMsg(tkAPPLICABLE_GDAL_ONLY);
	}

	return S_OK;
}

// *********************************************************
//		_pushSchemetkRaster()
// *********************************************************
STDMETHODIMP CImageClass::_pushSchemetkRaster(IGridColorScheme * cScheme, VARIANT_BOOL * retval)
{
	if (_gdal && _raster )
	{
		if ( cScheme )
		{
			_raster->ApplyCustomColorScheme(cScheme);
			_bufferReloadIsNeeded = true;
			*retval = VARIANT_TRUE;
		}
		else
		{
			ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
			*retval = VARIANT_FALSE;
		}
	}
	else
	{
		ErrorMessage(tkAPPLICABLE_GDAL_ONLY);
		*retval = VARIANT_FALSE;
	}
	return S_OK;
}

// ********************************************************
//     ImageColorScheme
// ********************************************************
STDMETHODIMP CImageClass::get_ImageColorScheme(PredefinedColorScheme * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _gdal ? _raster->GetDefaultColors() : FallLeaves;

	return S_OK;
}

STDMETHODIMP CImageClass::put_ImageColorScheme(PredefinedColorScheme newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_gdal)
	{
		_raster->ApplyPredefinedColorScheme(newValue);
		_bufferReloadIsNeeded = true;
	}

	return S_OK;
}

// ********************************************************
//     ImageColorScheme2
// ********************************************************
STDMETHODIMP CImageClass::get_CustomColorScheme(IGridColorScheme** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_gdal && _raster )
	{
		IGridColorScheme* scheme = _raster->GetCustomColorScheme();
		if (scheme)	scheme->AddRef();
		(*retVal) = scheme;
	}
	else
	{
		(*retVal) = NULL;
		ErrorMessage(tkAPPLICABLE_GDAL_ONLY);
	}

	return S_OK;
}

STDMETHODIMP CImageClass::put_CustomColorScheme(IGridColorScheme* newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VARIANT_BOOL vb;
	_pushSchemetkRaster(newValue, &vb);

	return S_OK;
}

// ********************************************************
//     SetTransparentColor
// ********************************************************
STDMETHODIMP CImageClass::SetTransparentColor(OLE_COLOR color)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	put_TransparencyColor(color);
	put_TransparencyColor2(color);
	put_UseTransparencyColor(VARIANT_TRUE);
	return S_OK;
}

// ********************************************************
//     get_IsRgb
// ********************************************************
STDMETHODIMP CImageClass::get_IsRgb(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_gdal)
	{
		*retVal = GdalHelper::IsRgb(_raster->GetDataset()) ? VARIANT_TRUE: VARIANT_FALSE;
	}
	else
		*retVal = VARIANT_TRUE;

	return S_OK;
}
	
// ********************************************************
//     OpenAsGrid
// ********************************************************
STDMETHODIMP CImageClass::OpenAsGrid(IGrid** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	CStringW filename = isGridProxy ? this->sourceGridName : _fileName;

	if (Utility::FileExistsW(filename))
	{
		ComHelper::CreateInstance(tkInterface::idGrid, (IDispatch**)retVal);
		VARIANT_BOOL vb;
		USES_CONVERSION;

		CComBSTR bstr(filename);
		(*retVal)->Open(bstr, GridDataType::UnknownDataType, true, GridFileType::UseExtension, _globalCallback, &vb);
		if (!vb)
		{
			(*retVal)->Release();
			(*retVal) = NULL;
		}
	}
	return S_OK;
}

// ********************************************************
//     SourceGridBandIndex
// ********************************************************
STDMETHODIMP CImageClass::get_SourceGridBandIndex(int * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _gdal ? _raster->GetActiveBandIndex() : -1;

	return S_OK;
}

STDMETHODIMP CImageClass::put_SourceGridBandIndex(int newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (!_gdal) return S_OK;
	
	if (!_raster->SetActiveBandIndex(newValue)) 
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else 
	{
		_bufferReloadIsNeeded = true;
	}
	
	return S_OK;
}

// ********************************************************
//     GeoProjection
// ********************************************************
STDMETHODIMP CImageClass::get_GeoProjection(IGeoProjection** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (_projection)
		_projection->AddRef();

	*pVal = _projection;
	return S_OK;
}

// ********************************************************
//     get_IsEmpty
// ********************************************************
STDMETHODIMP CImageClass::get_IsEmpty(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _sourceType == istUninitialized ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ********************************************************
//     GetOriginalBufferWidth
// ********************************************************
int CImageClass::GetOriginalBufferWidth()
{
	if (_gdal && _raster) 
	{
		return (_raster->GetVisibleRect().right - _raster->GetVisibleRect().left);
	}

	return 0;
}

// ********************************************************
//     GetOriginalBufferHeight
// ********************************************************
int CImageClass::GetOriginalBufferHeight()
{
	if (_gdal && _raster)	 
	{
		return (_raster->GetVisibleRect().right - _raster->GetVisibleRect().left);
	}
	
	return 0;
}

// ********************************************************
//     get_Band
// ********************************************************
STDMETHODIMP CImageClass::get_Band(long bandIndex, IGdalRasterBand** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = NULL;

	if (!_gdal)
	{
		ErrorMessage(tkAPPLICABLE_GDAL_ONLY);
		return S_OK;
	}

	//they are 1-based internally, let them remain so
	if (bandIndex < 1 || bandIndex > _raster->GetNoBands())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	GDALRasterBand* band = _raster->GetBand(bandIndex);
	if (!band)
	{
		CallbackHelper::AssertionFailed("CImageClass::get_Band: not null band was expected at this point.");
		return S_OK;
	}

	ComHelper::CreateInstance(idGdalRasterBand, (IDispatch**)retVal);
	
	RasterBandHelper::Cast(*retVal)->InjectBand(band);

	return S_OK;
}

// ********************************************************
//     get_PalleteInterpretation2
// ********************************************************
STDMETHODIMP CImageClass::get_PaletteInterpretation2(tkPaletteInterpretation* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (_gdal)
	{
		*pVal = (tkPaletteInterpretation)_raster->GetPaletteInterpretation();
	}
	else
	{
		*pVal = piRGB;
	}

	return S_OK;
}

// ********************************************************
//     get_ActiveBand
// ********************************************************
STDMETHODIMP CImageClass::get_ActiveBand(IGdalRasterBand** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = NULL;

	if (_raster != NULL)
	{
		long bandIndex = _raster->GetActiveBandIndex();
		get_Band(bandIndex, pVal);
	}
	else
	{
		ErrorMessage(tkAPPLICABLE_GDAL_ONLY);
	}

	return S_OK;
}

// ********************************************************
//     Brightness
// ********************************************************
STDMETHODIMP CImageClass::get_Brightness(FLOAT* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _brightness;

	return S_OK;
}

STDMETHODIMP CImageClass::put_Brightness(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal < -1.0f) newVal = -1.0f;
	if (newVal > 1.0f) newVal = 1.0f;

	_brightness = newVal;

	return S_OK;
}

// ********************************************************
//     Contrast
// ********************************************************
STDMETHODIMP CImageClass::get_Contrast(FLOAT* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _contrast;

	return S_OK;
}

STDMETHODIMP CImageClass::put_Contrast(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal < 0.0f) newVal = 0.0f;
	if (newVal > 4.0f) newVal = 4.0f;

	_contrast = newVal;

	return S_OK;
}


// ********************************************************
//     Saturation
// ********************************************************
STDMETHODIMP CImageClass::get_Saturation(FLOAT* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _saturation;

	return S_OK;
}

STDMETHODIMP CImageClass::put_Saturation(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal < 0.0f) newVal = 0.0f;
	if (newVal > 3.0f) newVal = 3.0f;

	_saturation = newVal;

	return S_OK;
}

// ********************************************************
//     Hue
// ********************************************************
STDMETHODIMP CImageClass::get_Hue(FLOAT* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _hue;

	return S_OK;
}

STDMETHODIMP CImageClass::put_Hue(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal < -180.0f) newVal = -180.0f;
	if (newVal > 180.0f) newVal = 180.0f;

	_hue = newVal;

	return S_OK;
}

// ********************************************************
//     Gamma
// ********************************************************
STDMETHODIMP CImageClass::get_Gamma(FLOAT* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _gamma;

	return S_OK;
}

STDMETHODIMP CImageClass::put_Gamma(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal < 0.0f) newVal = 0.0f;
	if (newVal > 4.0f) newVal = 4.0f;

	_gamma = newVal;

	return S_OK;
}

// ********************************************************
//     ColorizeIntensity
// ********************************************************
STDMETHODIMP CImageClass::get_ColorizeIntensity(FLOAT* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _colorizeIntensity;

	return S_OK;
}

STDMETHODIMP CImageClass::put_ColorizeIntensity(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal < 0.0f) newVal = 0.0f;
	if (newVal > 1.0f) newVal = 1.0f;

	_colorizeIntensity = newVal;

	return S_OK;
}

// ********************************************************
//     ColorizeColor
// ********************************************************
STDMETHODIMP CImageClass::get_ColorizeColor(OLE_COLOR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _colorizeColor;

	return S_OK;
}

STDMETHODIMP CImageClass::put_ColorizeColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_colorizeColor = newVal;

	return S_OK;
}

// ********************************************************
//     GetColorMatrix
// ********************************************************
Gdiplus::ColorMatrix CImageClass::GetColorMatrix()
{
	Gdiplus::ColorMatrix m = ImageHelper::CreateMatrix(_contrast, _brightness, _saturation, 
				_hue, _colorizeIntensity, _colorizeColor, _setRGBToGrey, _transparencyPercent);
	return m;
}

// ********************************************************
//     ClearOverviews
// ********************************************************
STDMETHODIMP CImageClass::ClearOverviews(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (!_gdal)
	{
		*retVal = VARIANT_TRUE;
		return S_OK;
	}
	
	bool result = GdalHelper::ClearOverviews(_raster->GetDataset());

	*retVal = result ? VARIANT_TRUE : VARIANT_FALSE;
	
	return S_OK;
}

// ********************************************************
//     get_GdalDriver
// ********************************************************
STDMETHODIMP CImageClass::get_GdalDriver(IGdalDriver** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = NULL;

	if (!_gdal)
	{
		ErrorMessage(tkAPPLICABLE_GDAL_ONLY);
		return S_OK;
	}

	GDALDataset* ds = _raster->GetDataset();
	if (ds)
	{
		GDALDriver* driver = ds->GetDriver();
		ComHelper::CreateInstance(idGdalDriver, (IDispatch**)pVal);
		((CGdalDriver*)*pVal)->Inject(driver);
	}

	return S_OK;
}

// ********************************************************
//     RedBandIndex
// ********************************************************
STDMETHODIMP CImageClass::get_RedBandIndex(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = static_cast<long>(GetRgbBandIndex(BandChannelRed));

	return S_OK;
}

STDMETHODIMP CImageClass::put_RedBandIndex(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SetRgbBandIndex(BandChannelRed, newVal);

	return S_OK;
}

// ********************************************************
//     GreenBandIndex
// ********************************************************
STDMETHODIMP CImageClass::get_GreenBandIndex(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = static_cast<long>(GetRgbBandIndex(BandChannelGreen));

	return S_OK;
}

STDMETHODIMP CImageClass::put_GreenBandIndex(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SetRgbBandIndex(BandChannelGreen, newVal);

	return S_OK;
}

// ********************************************************
//     BlueBandIndex
// ********************************************************
STDMETHODIMP CImageClass::get_BlueBandIndex(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = static_cast<long>(GetRgbBandIndex(BandChannelBlue));

	return S_OK;
}

STDMETHODIMP CImageClass::put_BlueBandIndex(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SetRgbBandIndex(BandChannelBlue, newVal);

	return S_OK;
}

// ********************************************************
//     UseRgbBandMapping
// ********************************************************
STDMETHODIMP CImageClass::get_UseRgbBandMapping(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (_raster)
	{
		*pVal = _raster->GetUseRgbBandMapping() ? VARIANT_TRUE : VARIANT_FALSE;;
	}
	else
	{
		*pVal = VARIANT_FALSE;
	}

	return S_OK;
}

STDMETHODIMP CImageClass::put_UseRgbBandMapping(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (_raster)
	{
		_raster->SetUseRgbBandMapping(newVal ? true : false);
	}
	else
	{
		CallbackHelper::ErrorMsg("CImageClass::put_UseRgbBandMapping is applicable for GDAL images only.");
	}

	return S_OK;
}

// ********************************************************
//     GetRgbBandIndex
// ********************************************************
int CImageClass::GetRgbBandIndex(BandChannel channel)
{
	if (_raster)
	{
		int index = _raster->GetRgbBandIndex(channel);
		return static_cast<long>(index);
	}

	return (int)channel;
}

// ********************************************************
//     GetRgbBandIndex
// ********************************************************
void CImageClass::SetRgbBandIndex(BandChannel channel, int bandIndex)
{
	if (_raster)
	{
		_raster->SetRgbBandIndex(channel, bandIndex);
	}
	else
	{
		CallbackHelper::ErrorMsg("SetRgbBandIndex is applicable for GDAL images only.");
	}
}

// ********************************************************
//     ForceSingleBandRendering
// ********************************************************
STDMETHODIMP CImageClass::get_ForceSingleBandRendering(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (_raster)
	{
		*pVal = _raster->GetForceSingleBandRendering();
	}

	return S_OK;
}

STDMETHODIMP CImageClass::put_ForceSingleBandRendering(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (_raster)
	{
		_raster->SetForceSingleBandRendering(newVal ? true : false);
	}

	return S_OK;
}

// ********************************************************
//     GetBufferReloadIsNeeded
// ********************************************************
bool CImageClass::GetBufferReloadIsNeeded()
{
	if (_bufferReloadIsNeeded)  {
		return true;
	}

	if (_imgType != BITMAP_FILE && (_width == 0 || _height == 0))
	{
		CallbackHelper::AssertionFailed("Buffer width or height is 0 after buffer reload.");
	}

	return false;
}
