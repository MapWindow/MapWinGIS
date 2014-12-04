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
#include "tkGif.h"
#include "tkJpg.h"
#include "tkRaster.h"
#include "projections.h"
#include "Templates.h"
#include "Base64.h"
#include "Labels.h"
#include "GridColorScheme.h"
#include "GridManager.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

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
	//int size = MultiByteToWideChar(CP_ACP, 0, WorldFileName.GetString(), -1, NULL, 0);
	//WCHAR* wName = new WCHAR[size];
	//MultiByteToWideChar(CP_ACP, 0, WorldFileName.GetString(), -1, wName, size);
	
	const int IOS_NOCREATE = 32; //std::ios::nocreate
	ifstream fin(WorldFileName,IOS_NOCREATE);
	
	//delete wName;

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
	return S_OK;
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
		
		_rasterImage = new tkRaster();
		if (!_globalCallback) this->put_GlobalCallback(cBack);
		_rasterImage->cBack = _globalCallback;

		_imgType = FileType;
		*retval = ReadRaster(_fileName, accessMode)?VARIANT_TRUE:VARIANT_FALSE;
		
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
	if (Utility::EndsWith(_fileName, L"_proxy.bmp") ||
		Utility::EndsWith(_fileName, L"_proxy.tif") )
	{
		CStringW legendName = _fileName + ".mwleg";
		if (Utility::FileExistsW(legendName))
		{
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
		}
	}
	return false;
}

// ********************************************************
//		ReadRaster()
// ********************************************************
// Added by Rob Cairns 5 Nov 2005
// Reading parameters of GDAL-based image; data isn't read here
bool CImageClass::ReadRaster(const CStringW ImageFile, GDALAccess accessMode)
{	
	if (! _rasterImage)
		return false;
	
	_inRam = true;	// inRam is always true for GDAL-based images
	
	if (! _rasterImage->LoadRaster(ImageFile, accessMode))
	{	
		ErrorMessage(tkCANT_OPEN_FILE);
		return false;
	}
	_fileName = ImageFile;
	
	// buffer wasn't loaded yet, so we will not set width, height, dx, etc properties; default values will be used
	
	_transColor = (int)_rasterImage->transColor;		// default is RGB(0,0,0) if no data value wasn't't set
	_transColor2 = (int)_rasterImage->transColor;

	// TODO: it's possible to add code to determine transparency by the prevailing color
	_useTransColor = _rasterImage->hasTransparency?VARIANT_TRUE:VARIANT_FALSE;

	_gdalImage = true;
	_dataLoaded = false; //not yet loaded into ImageData
	return true;
}

// *************************************************************
//	  Save()
// *************************************************************
STDMETHODIMP CImageClass::Save(BSTR ImageFileName, VARIANT_BOOL WriteWorldFile, ImageType FileType, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CStringW ImageFile = OLE2CW( ImageFileName );
	bool bWriteWorldFile = WriteWorldFile == VARIANT_TRUE;

	if(FileType == USE_FILE_EXTENSION)
	{
		if(! getFileType(ImageFile, FileType))
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
			*retval = WriteBMP( OLE2A(ImageFileName), bWriteWorldFile, cBack)?VARIANT_TRUE:VARIANT_FALSE;		// TODO: use Unicode
			break;
		case PPM_FILE:
			*retval = WritePPM(OLE2A(ImageFileName), bWriteWorldFile, cBack)?VARIANT_TRUE:VARIANT_FALSE;		// TODO: use Unicode
			break;	
		case JPEG_FILE: 
		case PNG_FILE:
		case GIF_FILE:
		case TIFF_FILE:
			if (_rasterImage)
			{
				// first let's try GDAL, it will preserve the inner structure of the image more precisely
				*retval = CopyGDALImage(OLE2CA(ImageFileName)) ? VARIANT_TRUE : VARIANT_FALSE;
				if (*retval == VARIANT_FALSE)
				{
					// if it didn't work, let's try GDIPlus
					*retval = WriteGDIPlus(OLE2A(ImageFileName), bWriteWorldFile, FileType, cBack)?VARIANT_TRUE:VARIANT_FALSE;
				}
			}
			else
			{
				// if there is no underlying GDAL dataset, Gdiplus is the only options
				*retval = WriteGDIPlus(OLE2A(ImageFileName), bWriteWorldFile, FileType, cBack)?VARIANT_TRUE:VARIANT_FALSE;
			}
			break;
		default:
			// try GDAL for any other format
			*retval = CopyGDALImage(OLE2CA(ImageFileName)) ? VARIANT_TRUE : VARIANT_FALSE;
			if (*retval == VARIANT_FALSE)
			{
				AfxMessageBox("Creating copy of dataset is unsupported by the current GDAL driver");
			}
			break;
	}
	return S_OK;
}

// ************************************************
//       CopyGDALImage()
// ************************************************
bool CImageClass::CopyGDALImage(CStringW ImageFileName )
{
	if (_rasterImage)
	{
		USES_CONVERSION;
		CStringW newName = ImageFileName;
		if (_fileName.MakeLower() == newName.MakeLower())
		{
			AfxMessageBox("Only saving in new file is supported for GDAL datasets");
		}
		else
		{
			GDALDataset* dataset = _rasterImage->get_Dataset();
			if (dataset)
			{
				GDALDriver* drv = dataset->GetDriver();
				if ( drv )
				{
					char **papszOptions = NULL;
					papszOptions = CSLSetNameValue( papszOptions, "WORLDFILE", "YES" );	  // tested for PNG files only
					
					m_globalSettings.SetGdalUtf8(true);
					GDALDataset* dst = drv->CreateCopy(Utility::ConvertToUtf8(newName), _rasterImage->get_Dataset(), 0, papszOptions, NULL, NULL);
					m_globalSettings.SetGdalUtf8(false);
					
					CSLDestroy( papszOptions );

					if (dst)
					{
						GDALClose(dst);
						return true;
					}
					else
					{
						
						return false;
					}
				}
			}
		}
	}
	return false;
}

// **********************************************************
//	  CreateNew()
// **********************************************************
STDMETHODIMP CImageClass::CreateNew(long NewWidth, long NewHeight, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//This function creates a new image data array
	//Check the NewWidth and NewHeight to see if they are valid numbers
	//Note that any information stored in ImageData will be lost unless 
	//it is written to a file first.

	if (NewWidth > 0 && NewHeight > 0)
	{
		this->Close(retval);
		
		if (!retval)
		{
			return S_FALSE;
		}
		
		try
		{
			_imageData = new colour[NewWidth*NewHeight];
		}
		catch(...)
		{
			ErrorMessage(tkFAILED_TO_ALLOCATE_MEMORY);
			*retval = VARIANT_FALSE;
			return S_OK;
		}
		
		_height = NewHeight;
		_width = NewWidth;
		_inRam = true;
		_sourceType = istInMemory;
		*retval = VARIANT_TRUE;
	}
	else
	{
		//can't create an array with Width <= 0 or Height <= 0
		ErrorMessage(tkINVALID_WIDTH_OR_HEIGHT);
		*retval = VARIANT_FALSE;
	}
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
	if ( _gdalImage )
	{
		if (_rasterImage)
		{
			int ref = _rasterImage->Dereference();
			
			// Call _rasterImage->Close only if reference count exactly equals zero
			if (ref == 0)
			{
				_rasterImage->Close();
				delete _rasterImage;
				_rasterImage = NULL;
			}
			else
			{
				Debug::WriteLine("Can't delete raster image because of the remaining references");
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
	_gdalImage = false;
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

	if(!( Row >= 0 && Row < _height ))
	{
		*retval = VARIANT_FALSE;
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		// lsu 18-07-2010: excluded the code for loading the buffer; it can be confusing I guess 
		// as reloading buffer can change values of Width and Height, and this call
		// can be incorporate in cycle with old values of height.
		// What is needed is to ensure the the consistency of buffer with 
		// properties like width, height, etc. If Height is equal to 0 (no buffer loaded)
		// then it's reasonable thing to receive nothing from GetRow or GetValue calls
		// Reload buffer with SetVisibleExtents and use the values then.
		// By default it's expected to have buffer of visible pixels in place after each redraw
		if ( _gdalImage && !_dataLoaded )
		{
			ErrorMessage(tkIMAGE_BUFFER_IS_EMPTY);
		}
		else
		{
			Row = _height -1 - Row;	//reverse the position of 0,0 (the origin) so that
								//it is at the top left of the image
		
			long element = Row * _width;
			for( int col = 0; col < _width; col++ )
			{
				colour currentPixel;
				if( _inRam )
				{
					currentPixel = _imageData[element];
					Vals[col] = RGB(currentPixel.red ,currentPixel.green, currentPixel.blue);
				}
				else
				{	
					if ( _imgType == BITMAP_FILE )
					{	
						colour tmp;
						tmp = _bitmapImage->getValue(Row,col);
						Vals[col] = RGB(tmp.red,tmp.green,tmp.blue);
					}
					else
						Vals[col] = -1;
				}
				element++;
			}
		}
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
	
	// lsu 18-07-2010: excluded the code for loading the buffer; it can be confusing I guess 
	// as reloading buffer can change values of Width and Height, and this call
	// can be incorporate in cycle with old values of height.
	// What is needed is to ensure the the consistency of buffer with 
	// properties like width, height, etc. If Height is equal to 0 (no buffer loaded)
	// then it's reasonable thing to receive nothing from GetRow or GetValue calls
	// Reload buffer with SetVisibleExtents and use the values then.
	// By default it's expected to have buffer of visible pixels in place after each redraw
	if ( _gdalImage && !_dataLoaded )
	{
		ErrorMessage(tkIMAGE_BUFFER_IS_EMPTY);
	}
	else
	{

		row = _height -1 - row;	//reverse the position of 0,0 (the origin) so that it is at the top left of the image
		colour currentPixel;
		
		if (( row >= 0 && row < _height ) && (col >=0 && col < _width ))
		{
			if( _inRam )
			{
				currentPixel = _imageData[row * _width + col];
				*pVal = RGB(currentPixel.red ,currentPixel.green, currentPixel.blue);
			}
			else
			{	
				if ( _imgType == BITMAP_FILE )
				{	
					colour tmp = _bitmapImage->getValue(row,col);
					*pVal = RGB(tmp.red,tmp.green,tmp.blue);
				}
				else
					*pVal = -1;
			}
		}
		else
		{	
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			*pVal = -1;
		}
	}
	return S_OK;
}

// *****************************************************************
//		put_Value()
// *****************************************************************
STDMETHODIMP CImageClass::put_Value(long row, long col, long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	row = _height -1 - row;	//reverse the position of 0,0 (the origin) so that
							//it is at the top left of the image

	if (( row >= 0 && row < _height ) && (col >=0 && col < _width ))
	{
		if( _inRam )
		{
			_imageData[row * _width + col].blue = GetBValue(newVal);
			_imageData[row * _width + col].red = GetRValue(newVal);
			_imageData[row * _width + col].green = GetGValue(newVal);
		}
		else
		{	
			if( _imgType == BITMAP_FILE )
			{
				bool setval = _bitmapImage->setValue( row, col, colour(GetRValue(newVal), GetGValue(newVal),GetBValue(newVal)));
				if ( ! setval == false )
				{	
					ErrorMessage(tkUNRECOVERABLE_ERROR);
				}
			}
			else if (_gdalImage) 
			{
				AfxMessageBox("Writing of gdal formats is not yet supported.");
			}	
			else
			{	
				//only BMP files can set values diskbased
				ErrorMessage(tkUNAVAILABLE_IN_DISK_MODE);
			}
		}
	}
	else
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);

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
		_imageChanged = true;
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
		_imageChanged = true;
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
		_imageChanged = true;
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
	//"ECW Images (*.ecw)|*.ecw|" 
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
bool CImageClass::WriteBMP(CString ImageFile, bool WorldFile, ICallback *cBack)
{
	bool result;

	_fileName = ImageFile;

	if( _inRam )
	{
		if(ImageFile.GetLength() <= 0) return false;
	
		tkBitmap bmp;
		if( cBack != NULL )
			bmp.globalCallback = cBack;

		// Use bitmap conversion
		bmp.setHeight(_height);
		bmp.setWidth(_width);
		result = bmp.WriteBitmap(ImageFile, _imageData);

		if( result == FALSE )
		{	
			ErrorMessage(tkCANT_CREATE_FILE);
			return result;
		}

		if (WorldFile)
		{
			CStringW WorldFileName = Utility::ChangeExtension(_fileName, L".bpw");
			result = WriteWorldFile(WorldFileName) == VARIANT_TRUE;
			if(!result)
			{
				ErrorMessage(tkCANT_WRITE_WORLD_FILE);
				return result;
			}
		}
	}
	else
	{
		// Technically it is -- but no action is required. putValue calls
		// the _bitmapImage object directly to put the value.
		USES_CONVERSION;
		CStringA filenameA = W2A(_fileName);			// TODO: use Unicode
		result = ImageFile == filenameA ? true : _bitmapImage->WriteDiskToDisk(filenameA,ImageFile);
	}
	return result;
}

// **********************************************************
//	  WriteGIF()
// **********************************************************
bool CImageClass::WriteGIF(CString ImageFile, bool WorldFile, ICallback *cBack)
{
	bool result;
	tkGif gif;
	gif.key = _key;
	gif.cBack = cBack;
	
	gif.InitSize(_width, _height);
	memcpy(gif.buffer, _imageData, _height*_width*3);

	result = gif.WriteGif((LPCTSTR)ImageFile);

	if(WorldFile)
	{
		CStringW WorldFileName = Utility::ChangeExtension(_fileName, L".gpw");
		result = WriteWorldFile(WorldFileName) == VARIANT_TRUE;
		if(!result)
		{
			ErrorMessage(tkCANT_WRITE_WORLD_FILE);
			return result;
		}
	}

	return result;
}

// **********************************************************
//	  WriteJPEG()
// **********************************************************
bool CImageClass::WriteGDIPlus(CString ImageFile, bool WorldFile, ImageType type, ICallback *cBack)
{
	bool result;
	tkJpg jpg;
	jpg.cBack = cBack;

	jpg.InitSize(_width, _height);
	memcpy(jpg.buffer, _imageData, _height*_width*3);

	CString ext;
	switch (type)
	{
		case PNG_FILE:
			result = jpg.SaveByGdiPlus((LPCTSTR)ImageFile, L"image/png");
			ext = ".pngw";
			break;
		case JPEG_FILE:
			result = jpg.SaveByGdiPlus((LPCTSTR)ImageFile, L"image/jpeg");
			ext = ".jpgw";
			break;
		case TIFF_FILE:
			result = jpg.SaveByGdiPlus((LPCTSTR)ImageFile, L"image/tiff");
			ext = ".tifw";
			break;
		case GIF_FILE:
			result = jpg.SaveByGdiPlus((LPCTSTR)ImageFile, L"image/gif");
			ext = ".gifw";
			break;
		default:
			return false;
	}

	if(WorldFile)
	{
		USES_CONVERSION;
		CStringW WorldFileName = Utility::ChangeExtension(_fileName, A2W(ext));
		result = this->WriteWorldFile(WorldFileName) == VARIANT_TRUE;
		if(!result)
		{
			ErrorMessage(tkCANT_WRITE_WORLD_FILE);
			return result;
		}
	}
	return result;
}

// **********************************************************
//	  WritePPM()
// **********************************************************
bool CImageClass::WritePPM(CString ImageFile, bool WorldFile, ICallback *cBack)
{
	bool result;
	tkGif ppm;
	
	ppm.InitSize(_width, _height);
	memcpy(ppm.buffer, _imageData, _height*_width*3);

	result = ppm.WritePPM((LPCTSTR)ImageFile);

	if(WorldFile)
	{
		CStringW WorldFileName = Utility::ChangeExtension(_fileName, L".ppw");
		result = WriteWorldFile(WorldFileName) == VARIANT_TRUE;
		if(!result)
		{
			ErrorMessage(tkCANT_WRITE_WORLD_FILE);
			return result;
		}
	}

	return result;
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

// **********************************************************
//	  ReadGIF()
// **********************************************************
bool CImageClass::ReadGIF(CString ImageFile)
{
	bool result;
	_inRam = true;

	if(_imageData)
		delete [] _imageData;

	tkGif gif;
	result = gif.ReadGif((LPCTSTR)ImageFile);

	if(!result)
	{
		ErrorMessage(tkCANT_OPEN_FILE);
		return false;
	}

	_height = gif.getHeight();
	_width = gif.getWidth();
	_imgType = GIF_FILE;
	USES_CONVERSION;
	_fileName = A2W(ImageFile);

	_imageData = new colour[_width*_height];
	memcpy(_imageData, gif.buffer, _width*_height*3);
	
	int LocationOfPeriod = ImageFile.ReverseFind('.');
	CString WorldFileName = ImageFile.Left(LocationOfPeriod);
	WorldFileName += ".gfw";
	ReadWorldFile(A2W(WorldFileName));				// TODO: use Unicode

	if(gif.hasTransparency)
	{
		_transColor = (int)gif.transColor;
		_useTransColor = VARIANT_TRUE;
	}
	else
	{
		long val;
		get_Value( 0, 0, &val );					
		_transColor = val;
	}

	return true;
}

// **********************************************************
//	  ReadPPM()
// **********************************************************
bool CImageClass::ReadPPM(CString ImageFile, bool InRam)
{
	bool result;
	_inRam = InRam;

	if(_imageData)
		delete [] _imageData;

	tkGif ppm;
	result = ppm.ReadPPM((LPCTSTR)ImageFile);

	if(!result)
	{
		ErrorMessage(tkCANT_OPEN_FILE);
		return false;
	}

	_height = ppm.getHeight();
	_width = ppm.getWidth();
	_imgType = PPM_FILE;
	USES_CONVERSION;
	_fileName = A2W(ImageFile);

	_imageData = new colour[_width*_height];
	memcpy(_imageData, ppm.buffer, _width*_height*3);
	
	int LocationOfPeriod = ImageFile.ReverseFind('.');
	CString WorldFileName = ImageFile.Left(LocationOfPeriod);
	WorldFileName += ".pmw";
	ReadWorldFile(A2W(WorldFileName));		// TODO: use Unicode

	long val;
	get_Value( 0, 0, &val );					
	_transColor = val;
	return true;
}

// **********************************************************
//	  ReadJPEG()
// **********************************************************
bool CImageClass::ReadJPEG(CString ImageFile)
{
	bool result;
	_inRam = true;

	if(_imageData)
		delete [] _imageData;

	tkJpg jpg;
	result = jpg.ReadJpg((LPCTSTR)ImageFile);

	if(!result)
	{
		ErrorMessage(tkCANT_OPEN_FILE);
		return false;
	}

	_height = jpg.getHeight();
	_width = jpg.getWidth();
	_imgType = JPEG_FILE;
	USES_CONVERSION;
	_fileName = A2W(ImageFile);
	_imageData = new colour[_width*_height];
	memcpy(_imageData, jpg.buffer, _width*_height*3);
	
	int LocationOfPeriod = ImageFile.ReverseFind('.');
	CString WorldFileName = ImageFile.Left(LocationOfPeriod);
	
	WorldFileName += ".jpgw";
	ReadWorldFile(A2W(WorldFileName));				// TODO: use Unicode
	
	//{
	//	dX = jpg.getDX();
	//	dY = jpg.getDY();
	//	XllCenter = jpg.getXllCenter();
	//	YllCenter = jpg.getYllCenter();
	//
	//	}

	if(jpg.hasTransparency)
	{
		_transColor = (int)jpg.transColor;
		_useTransColor = VARIANT_TRUE;
	}
	else
	{
		long val;
		get_Value( 0, 0, &val );					
		_transColor = val;
	}

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
		
	if (_gdalImage)
	{
		double mapUnitsPerScreenPixel = sqrt((newMaxX - newMinX) * (newMaxY - newMinY) / pixInView);
		if (_rasterImage->LoadBuffer(&_imageData, newMinX, newMinY, newMaxX, newMaxY, _fileName, _downsamplingMode, _setRGBToGrey, mapUnitsPerScreenPixel))
		{
			//Repeated here because _rasterImage->LoadImageBuffer changes width and height
			_height = _rasterImage->getHeight();
			_width = _rasterImage->getWidth();
			_dX = _rasterImage->getDX();
			_dY = _rasterImage->getDY();
			_xllCenter = _rasterImage->getXllCenter();
			_yllCenter = _rasterImage->getYllCenter();

			_dataLoaded = true;
		}
	}
	_bufferReloadIsNeeded = false;
	_imageChanged = false;
	return S_OK;
}

// ************************************************************
//	  IsGdalImageAvailable()
// ************************************************************
bool CImageClass::IsGdalImageAvailable()
{
	if ( _gdalImage && _rasterImage)
	{
		return true;
	}
	else
	{
		ErrorMessage(tkNOT_APPLICABLE_TO_BITMAP);
		return false;
	}
}

// ***********************************************************************
//		get_Picture()
// ***********************************************************************
STDMETHODIMP CImageClass::get_Picture(IPictureDisp **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( _width > 0 && _height > 0 )
	{	
		HDC desktop = GetDC(GetDesktopWindow());
		HDC compatdc = CreateCompatibleDC(desktop);
		HBITMAP bmp = CreateCompatibleBitmap(desktop,_width,_height);
		HGDIOBJ oldobj = SelectObject(compatdc,bmp);
		VARIANT_BOOL vbretval;
		GetImageBitsDC((long)compatdc,&vbretval);
		DeleteDC(compatdc);
		ReleaseDC(GetDesktopWindow(),desktop);

		PICTDESC pd;
		pd.cbSizeofstruct = sizeof(PICTDESC);
		pd.picType = PICTYPE_BITMAP;
		pd.bmp.hbitmap = bmp;
		pd.bmp.hpal = NULL;

		OleCreatePictureIndirect(&pd,IID_IPictureDisp,TRUE,(void**)pVal);
	}
	else
		*pVal = NULL;
	
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
	
	if (_gdalImage && !_dataLoaded)
	{
		// lsu 18-07-2010: excluded the code for loading the buffer; I prefer to separate
		// the operations of loading buffer and subsequent calls
		ErrorMessage(tkIMAGE_BUFFER_IS_EMPTY);
		*retval = VARIANT_FALSE;
	}
	else
	{
		HBITMAP hBMP;
		hBMP = (HBITMAP)GetCurrentObject((HDC)hDC,OBJ_BITMAP);
		
		if( hBMP == NULL )
		{	*retval = VARIANT_FALSE;
			return S_OK;
		}

		//Get the BITMAP Structure
		//Get the dimensions
		BITMAP bm;
		if(! GetObject(hBMP,sizeof(BITMAP),(void*)&bm) )
		{	*retval = VARIANT_FALSE;
			return S_OK;
		}

		if( bm.bmWidth != _width || bm.bmHeight != _height )
		{	*retval = VARIANT_FALSE;
			return S_OK;
		}

		long pad = _width*24;
		pad %= 32;
		if(pad != 0)
		{	pad = 32 - pad;
			pad /= 8;
		}
		
		unsigned char * bits = new unsigned char[(_width*3 + pad)*_height];	

		if( _inRam )
		{	
			long rowLength = _width*3 + pad;
			long loc = 0;
			for(int i=0; i < _height; i++ )
			{	
				memcpy(&(bits[loc]),&(_imageData[i*_width]),_width*sizeof(colour));
				loc += rowLength;
			}
		}
		else
		{
			long clr = RGB(0,0,0);
			long rowLength = _width*3 + pad;
			for(int j = 0; j < _height; j++ )
			{	
				for(int i = 0; i < _width; i++ )
				{	
					get_Value(_height-j-1,i,&clr);
					bits[j*rowLength + i*3] = GetBValue(clr);					
					bits[j*rowLength + i*3 + 1] = GetGValue(clr);
					bits[j*rowLength + i*3 + 2] = GetRValue(clr);				
				}
			}
		}
		
		BITMAPINFO bif;
		BITMAPINFOHEADER bih;
		bih.biBitCount = 24;
		bih.biWidth = _width;
		bih.biHeight = _height;
		bih.biPlanes = 1;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biCompression = 0;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;
		bih.biSizeImage = (_width*3 + pad)*_height;
		bif.bmiHeader = bih;

		SetDIBitsToDevice((HDC)hDC,0,0,_width,_height,0,0,0,_height,bits,&bif,DIB_RGB_COLORS);
			
		*retval = VARIANT_TRUE;
		delete [] bits; //--Lailin Chen 2006/4/12, Fixed a piece of memory leak.
		bits = NULL;

		#ifdef _DEBUG
		   AfxMessageBox("end CImageClass::GetImageBitsDC");
		#endif
	}

	return S_OK;
}

// ****************************************************************
//		SetDCBitsToImage
// ****************************************************************
// Used with DIBSection bits in Map::Snapshot3.
bool CImageClass::SetDCBitsToImage(long hDC,BYTE* bits)
{
	HBITMAP hBMP;
	hBMP = (HBITMAP)GetCurrentObject((HDC)hDC,OBJ_BITMAP);
	if( hBMP == NULL )
		return false;

	BITMAP bm;
	if(! GetObject(hBMP,sizeof(BITMAP),(void*)&bm) )
		return false;
	long paddedWidth = bm.bmWidthBytes;
	
	// creating resulting image
	VARIANT_BOOL vbretval;
	Close(&vbretval);
	CreateNew(bm.bmWidth,bm.bmHeight,&vbretval);
	if (!vbretval)
		return false;
	
	// copying the bytes
	long loc = 0;
	//for(int i=Height-1;i>=0;i--)
	for(int i=0;i < _height;i++)
	{	
		memcpy(&(_imageData[i*_width]),&(bits[loc]),bm.bmWidth*3);
		loc += paddedWidth;
	}
	return true;
}

bool CImageClass::SetImageBitsDCCore(HDC hDC)
{
	HBITMAP hBMP = (HBITMAP)GetCurrentObject(hDC,OBJ_BITMAP);
	if( hBMP == NULL ) return false;

	//Get the BITMAP Structure
	//Get the dimensions
	BITMAP bm;
	if(! GetObject(hBMP,sizeof(BITMAP),(void*)&bm) )
		return false;
	
	//Clean up the old tkImage and Create a new tkImage
	VARIANT_BOOL vbretval;
	Close(&vbretval);
	CreateNew(bm.bmWidth,bm.bmHeight,&vbretval);
	if (!vbretval)
		return false;

	long sizeBMP = bm.bmWidthBytes*bm.bmHeight;
	BYTE * bits = new BYTE[sizeBMP];
	GetBitmapBits(hBMP,sizeBMP,bits);

	long paddedWidth = bm.bmWidthBytes;

	register int i;	
	long loc=0;
	bool forward = false;
	if( bm.bmBitsPixel != 24 )
	{	
		long pad=(_width*24)%32;
		if(pad!=0)
		{	pad=32-pad;
			pad/=8;
		}

		BITMAPINFO bif;
		BITMAPINFOHEADER bih;
		bih.biBitCount=24;
		bih.biWidth=_width;
		bih.biHeight=_height;
		bih.biPlanes=1;
		bih.biSize=sizeof(BITMAPINFOHEADER);
		bih.biCompression=0;
		bih.biXPelsPerMeter=0;
		bih.biYPelsPerMeter=0;
		bih.biClrUsed=0;
		bih.biClrImportant=0;
		bih.biSizeImage=(_width*3+pad)*_height;
		bif.bmiHeader = bih;

		sizeBMP = (_width*3+pad)*_height;
		BYTE * diBits = new BYTE[sizeBMP];

		GetDIBits((HDC)hDC,hBMP,0,_height,diBits,&bif,DIB_RGB_COLORS);

		delete [] bits;
		bits = new BYTE[sizeBMP];
		memcpy(bits,diBits,sizeBMP);
		paddedWidth = (_width*3+pad);

		forward = true;
		delete[] diBits; //--2006/4/11 by Lailin Chen, Fixed the memory leak in snapshot function reported by Evan http://www.mapwindow.org/phorum/read.php?3,3031,3067#msg-3067   
	}

	if( forward )
	{	
		//Get Rid of the pad			
		for(i=0;i<_height;i++)
		{	memcpy(&(_imageData[i*_width]),&(bits[loc]),bm.bmWidth*3);
			loc += paddedWidth;
		}
	}
	else
	{
		//Get Rid of the pad			
		for(i=_height-1;i>=0;i--)
		{	memcpy(&(_imageData[i*_width]),&(bits[loc]),bm.bmWidth*3);
			loc += paddedWidth;
		}
	}
	delete [] bits;
	bits = NULL;
	return true;
}

// ****************************************************************
//		SetDCBitsToImage
// ****************************************************************
// Copies bits form the device context to image buffer

STDMETHODIMP CImageClass::SetImageBitsDC(long hDC, VARIANT_BOOL * retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;
	bool result = SetImageBitsDCCore((HDC)hDC);
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
// TODO: rewrite using GeoProjection class
STDMETHODIMP CImageClass::GetProjection(BSTR * Proj4)
{
	USES_CONVERSION;
	
	// If the .prj file exists, load it.
	CStringW prjFilename = Utility::GetProjectionFilename(_fileName);
	if (prjFilename != "")
	{
		char * prj4 = NULL;
		ProjectionTools * p = new ProjectionTools();
		p->GetProj4FromPRJFile( W2A(prjFilename), &prj4);		// TODO: use Unicode

		if (prj4 != NULL) 
			*Proj4 = A2BSTR(prj4);

		delete prj4;
		delete p; //added by Lailin Chen 12/30/2005
	}
	else
		*Proj4 = A2BSTR("");

	CString str = OLE2CA(*Proj4);
	if (str == "")
	{
		// Try getting it from GDAL
		GDALDataset * rasterDataset = GdalHelper::OpenDatasetW(_fileName);

		if( rasterDataset != NULL )
		{
			char * wkt = (char *)rasterDataset->GetProjectionRef();
			if (wkt != NULL && _tcslen(wkt) != 0)
			{
				OGRSpatialReferenceH  hSRS;
				hSRS = OSRNewSpatialReference(NULL);
				
				char** list = NULL;

				CSLAddString(list, wkt);
				if (OSRImportFromESRI(hSRS, list) == CE_None)
				{	
					char * pszProj4 = NULL;				
					OSRExportToProj4(hSRS, &pszProj4);
					if (pszProj4 != NULL)	
					{
						*Proj4 = A2BSTR(pszProj4);
						CPLFree(pszProj4);
					}
				} 
				else 
				{
					if( OSRImportFromWkt( hSRS, list ) == CE_None )
					{
						char * pszProj4 = NULL;
						OSRExportToProj4(hSRS, &pszProj4);
						if (pszProj4 != NULL)	
						{
							*Proj4 = A2BSTR(pszProj4);
							CPLFree(pszProj4);
						}
					}
				}
				OSRDestroySpatialReference( hSRS );
				CSLDestroy(list);
			}

			GDALClose(rasterDataset);
			rasterDataset = NULL;
		}
	}
	return S_OK;
}
STDMETHODIMP CImageClass::get_OriginalWidth(LONG* OriginalWidth)
{   // added by t shanley 04/05/2007
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if ( this->_gdalImage )  
	{
		*OriginalWidth = _rasterImage->orig_Width;
	}
	else
	{
		*OriginalWidth = _width; 
	}
	return S_OK;
}

STDMETHODIMP CImageClass::get_OriginalHeight(LONG* OriginalHeight)
{  // added by t shanley 04/05/2007
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (_gdalImage)
	{
		*OriginalHeight = _rasterImage->orig_Height;
	}
	else
	{
		*OriginalHeight = _height;
	}
	return S_OK;
}

STDMETHODIMP CImageClass::get_AllowHillshade(VARIANT_BOOL * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdalImage ? _rasterImage->allowHillshade : VARIANT_FALSE;
	return S_OK;
}


STDMETHODIMP CImageClass::put_AllowHillshade(VARIANT_BOOL newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage)
	{
		_rasterImage->allowHillshade = (newValue == VARIANT_TRUE);
		this->_imageChanged = true;
	}
	else
	{
		ErrorMsg(tkNOT_APPLICABLE_TO_BITMAP);
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
	this->_imageChanged = true;
	
	return S_OK;
}

STDMETHODIMP CImageClass::get_UseHistogram(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdalImage ? _rasterImage->useHistogram: VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CImageClass::put_UseHistogram(VARIANT_BOOL newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage)
	{
		_rasterImage->useHistogram = (newValue == VARIANT_TRUE);
		this->_imageChanged = true;
	}
	else
	{
		ErrorMsg(tkNOT_APPLICABLE_TO_BITMAP);
	}
	return S_OK;
}

STDMETHODIMP CImageClass::get_HasColorTable(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage)
	{
		_rasterImage->hasColorTable = _rasterImage->get_HasColorTable();
		*pVal = _rasterImage->hasColorTable ? VARIANT_TRUE : VARIANT_FALSE;
	}
	else
		*pVal = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CImageClass::get_PaletteInterpretation(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

		if (_gdalImage)
		{
			_rasterImage->paletteInterp = _rasterImage->get_PaletteInterpretation();
			switch (_rasterImage->paletteInterp)
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
	*pVal = _gdalImage ? _rasterImage->imageQuality: 100;
	return S_OK;
}

STDMETHODIMP CImageClass::put_BufferSize(int newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage)
	{
		_rasterImage->imageQuality = newValue;
		this->_imageChanged = true;
	}
	else
	{
		ErrorMsg(tkNOT_APPLICABLE_TO_BITMAP);
	}
	return S_OK;
}

STDMETHODIMP CImageClass::get_NoBands(int *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdalImage ? _rasterImage->get_NoBands() : NULL;
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
	if (_rasterImage)
	{
		_dX = _rasterImage->orig_dX;
		_dY = _rasterImage->orig_dY;
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
	*retVal = _gdalImage ? _rasterImage->clearGDALCache : VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CImageClass::put_ClearGDALCache(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_gdalImage)
	{
		_rasterImage->clearGDALCache = newVal?true:false;
	}
	else
		ErrorMessage(tkNOT_APPLICABLE_TO_BITMAP);
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
	this->_imageChanged = true;
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
	this->_imageChanged = true;
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
	this->_imageChanged = true;
	return S_OK;
}

// **************************************************************
//		Drawing method
// **************************************************************
STDMETHODIMP CImageClass::get_DrawingMethod(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal =_drawingMethod;
	return S_OK;
}
STDMETHODIMP CImageClass::put_DrawingMethod(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_drawingMethod = newVal;
	this->_imageChanged = true;
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
	
	int* overviewList = (int*)OverviewList->pvData;
	if (_gdalImage)
	{
		GDALDataset* dataset = _rasterImage->get_Dataset();
		if (dataset)
		{
			if (GdalHelper::BuildOverviewsCore(dataset, ResamplingMethod, overviewList, numOverviews, _globalCallback)) {
				*retval = VARIANT_TRUE;
			}
			else {
				ErrorMessage(tkUNSUPPORTED_FORMAT);
			}
		}
	}
	else
	{
		ErrorMessage(tkNOT_APPLICABLE_TO_BITMAP);
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
	CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&bBox );
	
	double minX, minY, maxX, maxY;
	if( _gdalImage )
	{
		minX = _rasterImage->orig_XllCenter - _rasterImage->orig_dX/2;
		minY = _rasterImage->orig_YllCenter - _rasterImage->orig_dY/2;
		maxX = _rasterImage->orig_XllCenter + _rasterImage->orig_dX * _rasterImage->orig_Width;
		maxY = _rasterImage->orig_YllCenter + _rasterImage->orig_dY * _rasterImage->orig_Height;
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
	
	if (!_rasterImage) return false;

	_width = _rasterImage->orig_Width;
	_height = _rasterImage->orig_Height;

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
	if (_rasterImage->LoadBufferFull(&_imageData, _fileName, 0))
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
	Utility::DisplayErrorMsg(_globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// **************************************************************
//		ProjectionToImage
// **************************************************************
// Returns image coordinates to the given map coordinates
STDMETHODIMP CImageClass::ProjectionToImage(double ProjX, double ProjY, long* ImageX, long* ImageY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_gdalImage)
	{
		*ImageX = Utility::Rint((ProjX - _rasterImage->orig_XllCenter)/_rasterImage->orig_dX);
		*ImageY = Utility::Rint((double)_rasterImage->orig_Height - 1 - (((ProjY - _rasterImage->orig_YllCenter)/_rasterImage->orig_dY)));
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
	
	if (_gdalImage)
	{
		*ProjX = _rasterImage->orig_XllCenter + (ImageX - 0.5) * _rasterImage->orig_dX;
		*ProjY = _rasterImage->orig_YllCenter + (_rasterImage->orig_Height - 1 - ImageY + 0.5) * _rasterImage->orig_dY;
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
	if (_gdalImage && _rasterImage)
		*pVal = _rasterImage->orig_XllCenter;
	else
		*pVal = _xllCenter;	// for bitmaps we'll use common values
	return S_OK;

}
STDMETHODIMP CImageClass::put_OriginalXllCenter( double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_gdalImage && _rasterImage)
	{
		_rasterImage->orig_XllCenter = newVal;
		_rasterImage->RefreshExtents();
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
	if (_gdalImage && _rasterImage)
		*pVal = _rasterImage->orig_YllCenter;
	else
		*pVal = _yllCenter;	// for bitmaps we'll use common values
	return S_OK;
}
STDMETHODIMP CImageClass::put_OriginalYllCenter( double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_gdalImage && _rasterImage)
	{		
		_rasterImage->orig_YllCenter = newVal;
		_rasterImage->RefreshExtents();
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

	if (_gdalImage && _rasterImage)
		*pVal = _rasterImage->orig_dX;
	else
		*pVal = _dX;	// for bitmaps we'll use common values
	
	return S_OK;
}
STDMETHODIMP CImageClass::put_OriginalDX( double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if ( newVal > 0.0)
	{
		if (_gdalImage && _rasterImage)
		{
			_rasterImage->orig_dX = newVal;
			_rasterImage->RefreshExtents();
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
	if (_gdalImage && _rasterImage)
		*pVal = _rasterImage->orig_dY;
	else
		*pVal = _dY;	// for bitmaps we'll use common values
	return S_OK;
}
STDMETHODIMP CImageClass::put_OriginalDY( double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if ( newVal > 0.0)
	{
		if (_gdalImage && _rasterImage)
		{
			_rasterImage->orig_dY = newVal;
			_rasterImage->RefreshExtents();
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
//	 get/put_OriginalYllCenter
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
	if (_gdalImage)
	{
		// do nothing: it doesn't make sense to change parameters of buffer
		ErrorMessage(tkNOT_APPLICABLE_TO_GDAL);
	}
	else
		_yllCenter = newVal;
	return S_OK;
}

// **************************************************************
//	 get/put_OriginalXllCenter
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
	if (_gdalImage)
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
	if (_gdalImage)
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
	if (_gdalImage)
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
	if ( _gdalImage )
	{
		// TODO: try to use GDAL-based histogram if possible
		
		// we'll open dataset second type, to keep an existing buffer untouched
		tkRaster* raster = new tkRaster();
		if ( raster->LoadRaster(_fileName) )
		{
			if (raster->LoadBufferFull(&data, _fileName, MaxBufferSizeMB))
			{
				if (data)
				{
					long size = raster->width * raster->height;
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
	if (size != 0)
	{
		std::map<OLE_COLOR, long> myMap;				// color as key and frequency as value
		
		// building list of colors and frequneces
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
	else 
		return false;
}

// **************************************************************
//		SetNoDataValue()
// **************************************************************
STDMETHODIMP CImageClass::SetNoDataValue(double Value, VARIANT_BOOL* Result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())	
	if (_gdalImage && _rasterImage)
	{
		GDALDataset* dataset = _rasterImage->get_Dataset();
		*Result = (VARIANT_BOOL)_rasterImage->SetNoDataValue(Value);
	}
	else
		ErrorMessage(tkNOT_APPLICABLE_TO_BITMAP);
	
	return S_OK;
}

// **************************************************************
//		get_NumOverviews()
// **************************************************************
STDMETHODIMP CImageClass::get_NumOverviews(int* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())		
	*retval = 0;
	if (_gdalImage)
	{
		GDALRasterBand* band = _rasterImage->get_RasterBand(1);
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
			*retVal = _rasterImage->LoadBufferFull(&(this->_imageData), this->_fileName, maxBufferSize);
			this->_height = _rasterImage->height;
			this->_width = _rasterImage->width;
			this->_dataLoaded = true;
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
{	// added by Rob Cairns 1-Jun-09
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage && _rasterImage )
	{
		*pVal = _rasterImage->orig_XllCenter;
	}
	else
	{
		*pVal = _xllCenter;;
	}
	return S_OK;
}

STDMETHODIMP CImageClass::GetOriginalYllCenter(double *pVal)
{	// added by Rob Cairns 1-Jun-09
	AFX_MANAGE_STATE(AfxGetStaticModuleState()) 
	if (_gdalImage && _rasterImage )
	{
		*pVal = _rasterImage->orig_YllCenter;
	}
	else
		*pVal = _yllCenter;
	return S_OK;
}

STDMETHODIMP CImageClass::GetOriginal_dX(double *pVal)
{	// added by Rob Cairns 1-Jun-09
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage && _rasterImage )
		*pVal = _rasterImage->orig_dX;
	else
		*pVal = _dX;
	return S_OK;
}

STDMETHODIMP CImageClass::GetOriginal_dY(double *pVal)
{	// added by Rob Cairns 1-Jun-09
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage && _rasterImage )
		*pVal = _rasterImage->orig_dY;
	else
		*pVal = _dY;
	return S_OK;
}

STDMETHODIMP CImageClass::GetOriginalHeight(long *pVal)
{	// added by Rob Cairns 1-Jun-09
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (_gdalImage && _rasterImage)
		*pVal = _rasterImage->orig_Height;
	else
		*pVal = _height; 

	return S_OK;
}
STDMETHODIMP CImageClass::GetOriginalWidth(long *pVal)
{	// added by Rob Cairns 1-Jun-09
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (_gdalImage)
		*pVal = _rasterImage->orig_Width;
	else
		*pVal = _width; 

	return S_OK;
}

// ********************************************************
//     get_Warped()
// ********************************************************
STDMETHODIMP CImageClass::get_Warped(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _gdalImage ? _rasterImage->warped : VARIANT_FALSE;
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

	if (_gdalImage)
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
				
				if (_gdalImage)
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
					this->_sourceType = istGDIPlus;
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
	if (_gdalImage)
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
		_rasterImage->ApplyPredefinedColorScheme(colors);
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
	*retVal = OLE2BSTR(isGridProxy ? this->sourceGridName: this->_fileName);
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
	*retVal = _gdalImage ? _rasterImage->WillBeRenderedAsGrid() : VARIANT_FALSE;
	return S_OK;
}

// ********************************************************
//     AllowGridRendering
// ********************************************************
STDMETHODIMP CImageClass::get_AllowGridRendering(tkGridRendering * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _gdalImage ? _rasterImage->allowAsGrid : tkGridRendering::grForGridsOnly;
	return S_OK;
}
STDMETHODIMP CImageClass::put_AllowGridRendering(tkGridRendering newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage)
	{
		if (_rasterImage->allowAsGrid != newValue)
		{
			_rasterImage->allowAsGrid = newValue;
			this->_imageChanged = true;
		}
	}
	else
	{
		ErrorMsg(tkNOT_APPLICABLE_TO_BITMAP);
	}
	return S_OK;
}

// *********************************************************
//		_pushSchemetkRaster()
// *********************************************************
STDMETHODIMP CImageClass::_pushSchemetkRaster(IGridColorScheme * cScheme, VARIANT_BOOL * retval)
{
	if (_gdalImage && _rasterImage )
	{
		if ( cScheme )
		{
			_rasterImage->ApplyCustomColorScheme(cScheme);
			_imageChanged = true;
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
		ErrorMessage(tkNOT_APPLICABLE_TO_BITMAP);
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
	*pVal = _gdalImage ? _rasterImage->GetDefaultColors() : FallLeaves;
	return S_OK;
}
STDMETHODIMP CImageClass::put_ImageColorScheme(PredefinedColorScheme newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage)
	{
		_rasterImage->ApplyPredefinedColorScheme(newValue);
		this->_imageChanged = true;
	}
	return S_OK;
}

// ********************************************************
//     ImageColorScheme2
// ********************************************************
STDMETHODIMP CImageClass::get_CustomColorScheme(IGridColorScheme** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage && _rasterImage )
	{
		IGridColorScheme* scheme = _rasterImage->get_CustomColorScheme();
		if (scheme)	scheme->AddRef();
		(*retVal) = scheme;
	}
	else
	{
		(*retVal) = NULL;
		ErrorMessage(tkNOT_APPLICABLE_TO_BITMAP);
	}
	return S_OK;
}
STDMETHODIMP CImageClass::put_CustomColorScheme(IGridColorScheme* newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	VARIANT_BOOL vb;
	this->_pushSchemetkRaster(newValue, &vb);
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
	if (_gdalImage)
	{
		*retVal = _rasterImage->IsRgb() ? VARIANT_TRUE: VARIANT_FALSE;
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
	CStringW filename = isGridProxy ? this->sourceGridName : this->_fileName;
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
	*pVal = _gdalImage ? _rasterImage->activeBandIndex : -1;
	return S_OK;
}
STDMETHODIMP CImageClass::put_SourceGridBandIndex(int newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_gdalImage)
	{
		if (!_rasterImage->SetActiveBandIndex(newValue))
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
		else
			this->_imageChanged = true;
	}
	return S_OK;
}
