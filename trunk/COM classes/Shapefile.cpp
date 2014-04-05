//********************************************************************************************************
//File name: Shapefile.cpp
//Description: Implementation of the CShapefile
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
// -------------------------------------------------------------------------------------------------------
// 06/06/2007 - T Shanley (tws) - fixed some memory leaks in EditInsertShape
// 07/03/2009 - Sergei Leschinski (lsu) - caching of shape extents for faster redraw in edit mode 
//				(see http://www.mapwindow.org/phorum/read.php?5,13065).
//				New properties/methods: RefreshShapeExtents, RefreshExtents, CacheExtents.
//				Works only in edit mode (Shapefile.EditingShapes = true). Set Shapefile.CacheExtents = true 
//				to activate this functionality. When changes are made to certain shapes RefreshShapeExtents 
//				must be used to update cached information. RefreshExtents is used to update extents for 
//				a whole shapefile. shpBounds vector is used to store extents. 
// 07/23/2009 - ÷‹¥œª‘ (Neio Zhou) - 
//              QuadTree support in Edit mode, see (http://www.mapwindow.org/phorum/read.php?5,13404)
//			    use UseQTree property to switch if use the quadtree.
//              The performance is much better when use both cacheExtents = true and UseQTree = true.
//              Add Save Method to save without exiting edit mode
//              Change the bahavior of SaveAs (This beed to be discussed)
//              Change the selectShapes in Edit Mode in order to fix a bug that cannot select inserted shapes
// 08-24-2009 (sm) Fixes to performance issues with the spatial index trees as per 
//				http://www.mapwindow.org/phorum/read.php?5,13738
// 27 aug 2009 lsu Added support for selection. GetIntersection, SpatialQuery, SelectShapesAlt functions.
                       
#include "stdafx.h"
#include "Shapefile.h"
#include "Varh.h"
#include "Projections.h"		// ProjectionTools
#include "Labels.h"
#include "Charts.h"
#include "GeoProjection.h"
#include "Templates.h"
#include "TableClass.h"
#include <GeosHelper.h>
#include "ShapeValidator.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

#pragma region Properties	

// ************************************************************
//		get_EditingShapes()
// ************************************************************
STDMETHODIMP CShapefile::get_EditingShapes(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _isEditingShapes?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

// ************************************************************
//		get_LastErrorCode()
// ************************************************************
STDMETHODIMP CShapefile::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = lastErrorCode;
	lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// ************************************************************
//		get_CdlgFilter()
// ************************************************************
STDMETHODIMP CShapefile::get_CdlgFilter(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR("ESRI Shapefiles (*.shp)|*.shp");
	return S_OK;
}

// ************************************************************
//		LastInputValidation
// ************************************************************
STDMETHODIMP CShapefile::get_LastInputValidation(IShapeValidationInfo** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_inputValidation)
		_inputValidation->AddRef();
	*retVal = _inputValidation;
	return S_OK;
}

// ************************************************************
//		LastOutputValidation
// ************************************************************
STDMETHODIMP CShapefile::get_LastOutputValidation(IShapeValidationInfo** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_outputValidation)
		_outputValidation->AddRef();
	*retVal = _outputValidation;
	return S_OK;
}

// ************************************************************
//		get/put_GlobalCallback()
// ************************************************************
STDMETHODIMP CShapefile::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = globalCallback;
	if( globalCallback != NULL )
		globalCallback->AddRef();
	return S_OK;
}
STDMETHODIMP CShapefile::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&globalCallback);
	if( dbf != NULL )
		dbf->put_GlobalCallback(newVal);

	return S_OK;
}

// ************************************************************
//		StopExecution
// ************************************************************
STDMETHODIMP CShapefile::put_StopExecution(IStopExecution* stopper)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference((IDispatch*)stopper, (IDispatch**)&_stopExecution, true);
	return S_OK;
}

// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CShapefile::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(key);
	return S_OK;
}
STDMETHODIMP CShapefile::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(key);
	key = OLE2BSTR(newVal);
	return S_OK;
}

// ************************************************************
//		get/put_VisibilityExpression
// ************************************************************
STDMETHODIMP CShapefile::get_VisibilityExpression(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_expression);
	return S_OK;
}
STDMETHODIMP CShapefile::put_VisibilityExpression(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_expression);
	_expression = OLE2BSTR(newVal);
	return S_OK;
}

// *****************************************************************
//		get_NumShapes()
// *****************************************************************
STDMETHODIMP CShapefile::get_NumShapes(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _shapeData.size(); //_numShapes;
	return S_OK;
}

// **************************************************************
//		get_NumFields()
// **************************************************************
STDMETHODIMP CShapefile::get_NumFields(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( dbf != NULL )
		dbf->get_NumFields(pVal);
	else
	{	
		ErrorMessage(tkFILE_NOT_OPEN);
		*pVal = 0;
	}
	return S_OK;
}

// ************************************************************
//		get_ShapefileType()
// ************************************************************
STDMETHODIMP CShapefile::get_ShapefileType(ShpfileType *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _shpfiletype;
	return S_OK;
}

// *****************************************************************
//	   get_ErrorMsg()
// *****************************************************************
STDMETHODIMP CShapefile::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// *****************************************************************
//	   get_FileHandle()
// *****************************************************************
STDMETHODIMP CShapefile::get_FileHandle(long * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( _shpfile != NULL )
	{	
		int handle = _fileno(_shpfile);
		*pVal = _dup(handle);				
	}
	else
		*pVal = -1;

	return S_OK;
}

// **************************************************************
//	   get_Filename()
// **************************************************************
STDMETHODIMP CShapefile::get_Filename(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = W2BSTR(_shpfileName);

	return S_OK;
}

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CShapefile::ErrorMessage(long ErrorCode)
{
	lastErrorCode = ErrorCode;
	if( globalCallback != NULL) globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
	return;
}
void CShapefile::ErrorMessage(long ErrorCode, ICallback* cBack)
{
	lastErrorCode = ErrorCode;
	if( globalCallback != NULL) globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
	// in case additional callback was provided we shall use it as well
	if( cBack != NULL) cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
	return;
}

// ************************************************************
//		get_MinDrawingSize()
// ************************************************************
STDMETHODIMP CShapefile::get_MinDrawingSize(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _minDrawingSize;
	return S_OK;
}
STDMETHODIMP CShapefile::put_MinDrawingSize(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_minDrawingSize = newVal;
	return S_OK;
}

// ************************************************************
//		get_SourceType()
// ************************************************************
STDMETHODIMP CShapefile::get_SourceType(tkShapefileSourceType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_sourceType;
	return S_OK;
}

#pragma endregion

#pragma region CreateAndOpen
// *****************************************************
//		ApplyRandomDrawingOptions()
// *****************************************************
void CShapefile::ApplyRandomDrawingOptions()
{
	ShpfileType type = _shpfiletype;
	
	// setting default colors for shapefile layer
	IShapeDrawingOptions* options = NULL;
	this->get_DefaultDrawingOptions(&options);
	if (options)
	{
		//srand( (unsigned)time( NULL ));
		unsigned char r1, r2, g1, g2, b1, b2;

		if( type == SHP_POLYLINE || type == SHP_POLYLINEZ || type == SHP_POLYLINEM )
		{
			r1 = g1 = b1 = 75;	 // dark colors
			r2 = g2 = b2 = 150;
			unsigned char r = r1 + unsigned char(double(rand()/double(RAND_MAX) * (r2 - r1)));
			unsigned char g = g1 + unsigned char(double(rand()/double(RAND_MAX) * (g2 - g1)));
			unsigned char b = b1 + unsigned char(double(rand()/double(RAND_MAX) * (b2 - b1)));
			options->put_LineColor(RGB(r,g,b));
			options->put_FillColor(RGB(255,255,255));
		}
		else if( type == SHP_POLYGON || type == SHP_POLYGONZ || type == SHP_POLYGONM )
		{
			r1 = g1 = b1 = 180;	// light colors
			r2 = g2 = b2 = 230;
			unsigned char r = r1 + unsigned char(double(rand()/double(RAND_MAX) * (r2 - r1)));
			unsigned char g = g1 + unsigned char(double(rand()/double(RAND_MAX) * (g2 - g1)));
			unsigned char b = b1 + unsigned char(double(rand()/double(RAND_MAX) * (b2 - b1)));
			options->put_FillColor(RGB(r,g,b));

			// grey color for outlines
			options->put_LineColor(RGB(150,150,150));
		}
		else   // point and multipoints
		{
			r1 = g1 = b1 = 50;
			r2 = g2 = b2 = 150;
			unsigned char r = r1 + unsigned char(double(rand()/double(RAND_MAX) * (r2 - r1)));
			unsigned char g = g1 + unsigned char(double(rand()/double(RAND_MAX) * (g2 - g1)));
			unsigned char b = b1 + unsigned char(double(rand()/double(RAND_MAX) * (b2 - b1)));
			options->put_FillColor(RGB(r,g,b));

			// grey color for outlines
			options->put_LineColor(RGB(150,150,150));
		}
		options->Release();
		options = NULL;
	}

	this->get_SelectionDrawingOptions(&options);
	if (options)
	{
		if( type == SHP_POLYLINE || type == SHP_POLYLINEZ || type == SHP_POLYLINEM )
		{
			options->put_LineColor(RGB(255,255,0));
		}
		else if( type == SHP_POLYGON || type == SHP_POLYGONZ || type == SHP_POLYGONM )
		{
			options->put_FillColor(RGB(255,255,0));
			// grey color for outlines
			options->put_LineColor(RGB(150,150,150));
		}
		else   // point and multipoints
		{
			options->put_FillColor(RGB(255, 255,0));
			// grey color for outlines
			options->put_LineColor(RGB(150,150,150));
		}
		options->Release();
		options = NULL;
	}

	ILabels* labels;
	this->get_Labels(&labels);
	if (labels)
	{
		if (type == SHP_POINT || type == SHP_POINTZ || type == SHP_POINTM)
		{
			labels->put_Alignment(laCenterRight);
		}
		labels->Release();
	}
}

// ************************************************************
//		LoadDataFrom()
// ************************************************************
// loads shape and dbf data from disk file into in-memory mode
STDMETHODIMP CShapefile::LoadDataFrom(BSTR ShapefileName, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	USES_CONVERSION;
	*retval = VARIANT_FALSE;
	if (m_sourceType != sstInMemory)
	{
		return S_FALSE;	   // TODO: add error message; works only for in-memory shapefiles
	}
	else
	{
		if (OpenCore(OLE2CA(ShapefileName), cBack))
		{
			// loading data in-memory
			VARIANT_BOOL vb;
			_isEditingShapes = false;
			StartEditingShapes(VARIANT_TRUE, cBack, &vb);
			
			// this will trigger loading of all dbf vaues into the memory
			long numFields;
			this->get_NumFields(&numFields);
			if (numFields > 0)
			{
				CComVariant var;
				for(size_t i = 0; i < _shapeData.size(); i++)
				{
					dbf->get_CellValue(0, i, &var);
				}
			}
			
			// closing disk file despite the result success or failure
			_shpfileName = "";
			_shxfileName = "";
			_dbffileName = "";

			if( _shpfile != NULL) 
				fclose(_shpfile);
			_shpfile = NULL;
			
			if( _shxfile != NULL) 
				fclose(_shxfile);
			_shxfile = NULL;
			
			if( dbf != NULL )
				((CTableClass*)dbf)->CloseUnderlyingFile();
			
			*retval = vb;
		}
	}
	return S_OK;
}

bool CShapefile::OpenCore(CStringW tmp_shpfileName, ICallback* cBack)
{
	USES_CONVERSION;
	VARIANT_BOOL vbretval;

	// saving the provided names; 
	// from now on we must clean the class varaibles in case the operation won't succeed
	_shpfileName = tmp_shpfileName;
	_shxfileName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + L"shx";
	_dbffileName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + L"dbf";
	_prjfileName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + L"prj";

	// Chris Michaelis 12/19/2005 - Windows 98 doesn't support unicode and will thus crash and burn on _wfopen.
	OSVERSIONINFO OSversion;
	OSversion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	::GetVersionEx(&OSversion);

	if (OSversion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		// Running Windows 95, 98, ME
		_shpfile = fopen(W2A(_shpfileName), "rb");
		_shxfile = fopen(W2A(_shxfileName), "rb");
	}
	else
	{
		// Running 2k, XP, NT, or other future versions
		////Changes made to use _wfopen to support Assian character file names ---Lailin Chen 11/5/2005
		_shpfile = _wfopen( _shpfileName, L"rb");
		_shxfile = _wfopen(_shxfileName,L"rb");
	}
	
	// opening dbf
	//ASSERT(dbf == NULL);
	if (!dbf)
	{
		CoCreateInstance(CLSID_Table,NULL,CLSCTX_INPROC_SERVER,IID_ITable,(void**)&dbf);
	}
	else
	{
		VARIANT_BOOL vb;
		dbf->Close(&vb);
	}
	dbf->put_GlobalCallback(globalCallback);
	dbf->Open(W2BSTR(_dbffileName), cBack, &vbretval);

	if( _shpfile == NULL )
	{
		ErrorMessage(tkCANT_OPEN_SHP);
		this->Close(&vbretval);
	}
	else if( _shxfile == NULL )
	{	
		ErrorMessage(tkCANT_OPEN_SHX);
		this->Close(&vbretval);
	}
	else if( vbretval == VARIANT_FALSE )
	{	
		dbf->get_LastErrorCode(&lastErrorCode);
		ErrorMessage(lastErrorCode);
		this->Close(&vbretval);
	}		
	else
	{	
		if( !readShx())		// shapefile header is read here as well
		{	
			ErrorMessage(tkINVALID_SHX_FILE);
			this->Close(&vbretval);
		}
		else
		{
			//Check for supported types
			if( _shpfiletype != SHP_NULLSHAPE &&
				_shpfiletype != SHP_POINT &&
				_shpfiletype != SHP_POLYLINE &&
				_shpfiletype != SHP_POLYGON &&
				_shpfiletype != SHP_POINTZ &&
				_shpfiletype != SHP_POLYLINEZ &&
				_shpfiletype != SHP_POLYGONZ &&
				_shpfiletype != SHP_MULTIPOINT &&
				_shpfiletype != SHP_MULTIPOINTZ &&
				_shpfiletype != SHP_POLYLINEM &&
				_shpfiletype != SHP_POLYGONM &&
				_shpfiletype != SHP_POINTM &&
				_shpfiletype != SHP_MULTIPOINTM )
			{
				ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
				this->Close(&vbretval);
			}
			else
			{
				_shapeData.reserve(shpOffsets.size());
				for (size_t i = 0; i < shpOffsets.size(); i++)
				{
					_shapeData.push_back(new ShapeData());
				}
				return true;
			}
		}
	}		
	return false;
}

// ************************************************************
//		Open()
// ************************************************************
STDMETHODIMP CShapefile::Open(BSTR ShapefileName, ICallback *cBack, VARIANT_BOOL *retval)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;
	VARIANT_BOOL vbretval;

	if(globalCallback == NULL && cBack !=NULL)
	{
		globalCallback = cBack;
		globalCallback->AddRef();
	}
	
	USES_CONVERSION;
	CStringW tmp_shpfileName = OLE2CW(ShapefileName);

	if (tmp_shpfileName.GetLength() == 0)
	{
		// better to use CreateNew directly, but this call will be preserved for backward compatibility
		this->CreateNew(A2BSTR(""), _shpfiletype, &vbretval);
	}
	else if( tmp_shpfileName.GetLength() <= 3 )
	{	
		ErrorMessage(tkINVALID_FILENAME);
	}
	else
	{
		// close the opened shapefile
		this->Close(&vbretval);

		if( vbretval == VARIANT_FALSE )
		{
			// error code in the function
			return S_FALSE;
		}

		if (OpenCore(tmp_shpfileName, cBack))
		{
			if (_fastMode)
			{
				_fastMode = FALSE;
				this->put_FastMode(VARIANT_TRUE);
			}

			m_sourceType = sstDiskBased;

			// reading projection
			m_geoProjection->ReadFromFile(W2BSTR(_prjfileName), &vbretval);

			this->ApplyRandomDrawingOptions();
			UpdateLabelsPositioning();
			*retval = VARIANT_TRUE;
		}
	}
	return S_OK;
}

// *********************************************************
//		CreateNew()
// *********************************************************
STDMETHODIMP CShapefile::CreateNew(BSTR ShapefileName, ShpfileType ShapefileType, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return this->CreateNewCore(ShapefileName, ShapefileType, true ,retval);
	return S_OK;
}

// *********************************************************
//		CreateNewCore()
// *********************************************************
HRESULT CShapefile::CreateNewCore(BSTR ShapefileName, ShpfileType ShapefileType, bool applyRandomOptions, VARIANT_BOOL *retval)
{
	*retval = VARIANT_FALSE;
	VARIANT_BOOL vbretval;

	// check for supported types
	if( ShapefileType != SHP_NULLSHAPE &&
		ShapefileType != SHP_POINT &&
		ShapefileType != SHP_POLYLINE &&
		ShapefileType != SHP_POLYGON &&
		ShapefileType != SHP_POINTZ &&
		ShapefileType != SHP_POLYLINEZ &&
		ShapefileType != SHP_POLYGONZ &&
		ShapefileType != SHP_MULTIPOINT &&
		ShapefileType != SHP_MULTIPOINTZ &&
		ShapefileType != SHP_POLYLINEM &&
		ShapefileType != SHP_POLYGONM &&
		ShapefileType != SHP_MULTIPOINTM )
	{
		ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
	}
	else
	{
		USES_CONVERSION;
		CString tmp_shpfileName = OLE2CA(ShapefileName);
		
		// ----------------------------------------------
		// in memory shapefile (without name)
		// ----------------------------------------------
		if (tmp_shpfileName.GetLength() == 0)
		{
			// closing the old shapefile (error code inside the function)
			Close(&vbretval);
			
			if( vbretval == VARIANT_TRUE )
			{	
				CoCreateInstance(CLSID_Table,NULL,CLSCTX_INPROC_SERVER,IID_ITable,(void**)&dbf);
				if (!dbf)
				{
					ErrorMessage(tkCANT_COCREATE_COM_INSTANCE);
				}
				else
				{
					BSTR newdbfName = A2BSTR("");
					dbf->CreateNew(newdbfName, &vbretval);
					
					if (!vbretval)
					{
						long error;
						dbf->get_LastErrorCode(&error);
						ErrorMessage(error);
						dbf->Release();
						dbf = NULL;
					}
					else
					{
						_shpfiletype = ShapefileType;
						_isEditingShapes = true;
						m_sourceType = sstInMemory;

						if (applyRandomOptions)
							this->ApplyRandomDrawingOptions();

						*retval = VARIANT_TRUE;
					}
				}
			}
		}
		else if (tmp_shpfileName.GetLength() <= 3)
		{
			ErrorMessage(tkINVALID_FILENAME);
		}
		else
		{
			// ----------------------------------------------
			// in memory shapefile (name specified, is acceptable and available)
			// ----------------------------------------------
			CString shpName, shxName, dbfName, prjName;
			shpName = tmp_shpfileName;
			shxName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "shx";
			dbfName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "dbf";
			prjName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "prj";
			
			// new file is created, so there must not be any files with this names
			if( Utility::fileExists(shpName) != FALSE )
			{	
				ErrorMessage(tkSHP_FILE_EXISTS);
				return S_OK;
			}
			if( Utility::fileExists(shxName) != FALSE )
			{	
				ErrorMessage(tkSHX_FILE_EXISTS);
				return S_OK;
			}
			if( Utility::fileExists(dbfName) != FALSE )
			{	
				ErrorMessage(tkDBF_FILE_EXISTS);
				return S_OK;
			}
			if( Utility::fileExists(prjName) != FALSE )
			{	
				ErrorMessage(tkPRJ_FILE_EXISTS);	// lsu: probably it's ok to overwite it blindly ?
				return S_OK;
			}

			// closing the old shapefile (error code inside the function)
			this->Close(&vbretval);
			
			if( vbretval == VARIANT_TRUE )
			{	
				CoCreateInstance(CLSID_Table,NULL,CLSCTX_INPROC_SERVER,IID_ITable,(void**)&dbf);
				
				if (!dbf)
				{
					ErrorMessage(tkCANT_COCREATE_COM_INSTANCE);
				}
				else
				{
					dbf->put_GlobalCallback(globalCallback);
					
					CString newDbfName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "dbf";
					dbf->CreateNew(A2BSTR(newDbfName), &vbretval);
					
					if (!vbretval)
					{
						dbf->get_LastErrorCode(&lastErrorCode);
						ErrorMessage(lastErrorCode);
						dbf->Release();
						dbf = NULL;
					}
					else
					{
						_shpfileName = tmp_shpfileName;
						_shxfileName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "shx";
						_dbffileName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "dbf";
						_prjfileName = tmp_shpfileName.Left(tmp_shpfileName.GetLength() - 3) + "prj";
						
						_shpfiletype = ShapefileType;
						_isEditingShapes = true;
						m_sourceType = sstInMemory;
						if (applyRandomOptions)
							this->ApplyRandomDrawingOptions();

						*retval = VARIANT_TRUE;
					}
				}
			}
		}
	}
	UpdateLabelsPositioning();
	return S_OK;
}

// *********************************************************
//		CreateNewWithShapeID()
// *********************************************************
STDMETHODIMP CShapefile::CreateNewWithShapeID(BSTR ShapefileName, ShpfileType ShapefileType, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	CreateNew(ShapefileName, ShapefileType, retval);
	
	if (*retval == VARIANT_TRUE)
	{
		IField * shapeIDField = NULL;
		CoCreateInstance(CLSID_Field,NULL,CLSCTX_INPROC_SERVER,IID_IField,(void**)&shapeIDField);
		
		shapeIDField->put_Name(A2BSTR("MWShapeID"));
		shapeIDField->put_Type(INTEGER_FIELD);
		shapeIDField->put_Width(10);
		shapeIDField->put_Precision(10);

		long fldIndex = 0;
		VARIANT_BOOL retVal;
		EditInsertField(shapeIDField, &fldIndex, NULL, &retVal);

		shapeIDField->Release();
	}
	return S_OK;
}
#pragma endregion

#pragma region SaveAndClose
// *****************************************************************
//		Close()
// *****************************************************************
STDMETHODIMP CShapefile::Close(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	ClearValidationList();
	ClearCachedGeometries();

	if( _isEditingShapes )
	{
		// just stop editing shapes, if the shape is in open status
		this->StopEditingShapes(VARIANT_FALSE, VARIANT_TRUE, NULL, retval);
	}
	
	// stop editing table in case only it have been edited
	VARIANT_BOOL isEditingTable = VARIANT_FALSE;
	if( dbf )
	{
		dbf->get_EditingTable(&isEditingTable);
		if (isEditingTable)
		{
			this->StopEditingTable(VARIANT_FALSE,globalCallback,retval);
			dbf->get_EditingTable(&isEditingTable);
		}
	}

	// removing shape data
	this->ReleaseMemoryShapes();
	for (unsigned int i = 0; i < _shapeData.size(); i++)
	{
		delete _shapeData[i];
	}
	_shapeData.clear();
	
	if (spatialIndexLoaded)
		IndexSearching::unloadSpatialIndex(spatialIndexID);

	m_sourceType = sstUninitialized;
	_shpfiletype = SHP_NULLSHAPE;
	this->UpdateLabelsPositioning();
	
	_shpfileName = "";
	_shxfileName = "";
	_dbffileName = "";
	
	_minX = 0.0;
	_minY = 0.0;
	_minZ = 0.0;
	_maxX = 0.0;
	_maxY = 0.0;
	_maxZ = 0.0;
	_minM = 0.0;
	_maxM = 0.0;
	
	if (_inputValidation != NULL)
	{
		_inputValidation->Release();
		_inputValidation = NULL;
	}

	if (_outputValidation != NULL)
	{
		_outputValidation->Release();
		_outputValidation = NULL;
	}

	if( _shpfile != NULL) fclose(_shpfile);
	_shpfile = NULL;
	
	if( _shxfile != NULL) fclose(_shxfile);
	_shxfile = NULL;
	
	if( dbf != NULL )
	{
		VARIANT_BOOL vbretval;
		dbf->Close(&vbretval);
		dbf->Release();
		dbf = NULL;
	}
	if (m_labels)
	{
		m_labels->Clear();
		m_labels->ClearCategories();
	}
	if (m_charts)
	{
		m_charts->Clear();
	}
	if (m_categories)
	{
		m_categories->Clear();
	}
	*retval = VARIANT_TRUE; 
	
	return S_OK;
}

// **********************************************************
//		Dump()
// **********************************************************
//Saves shapefile without reopening it in a new location
STDMETHODIMP CShapefile::Dump(BSTR ShapefileName, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;

	bool callbackIsNull = (globalCallback == NULL);
	if(globalCallback == NULL && cBack != NULL)
	{
		globalCallback = cBack;
		globalCallback->AddRef();
	}

	if (dbf == NULL || m_sourceType == sstUninitialized)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
		return S_FALSE;
	}

	// in case someone else is writing, we leave
	if (m_writing)
	{
		ErrorMessage(tkSHP_WRITE_VIOLATION);
		return S_FALSE;
	}
	
	if (!this->ValidateOutput(this, "Dump", "Shapefile", false))
		return S_FALSE;

	USES_CONVERSION;
	CString sa_shpfileName;
	sa_shpfileName = OLE2CA(ShapefileName);

	if( sa_shpfileName.GetLength() <= 3 )
	{	
		ErrorMessage(tkINVALID_FILENAME);
	}
	else
	{
		CString sa_shxfileName = sa_shpfileName.Left(sa_shpfileName.GetLength() - 3) + "shx";
		CString sa_dbffileName = sa_shpfileName.Left(sa_shpfileName.GetLength() - 3) + "dbf";
	
		// -----------------------------------------------
		// it's not allowed to rewrite the existing files
		// -----------------------------------------------
		if( Utility::fileExists(sa_shpfileName) )
		{	
			ErrorMessage(tkSHP_FILE_EXISTS);
			return S_OK;
		}
		if( Utility::fileExists(sa_shxfileName) )
		{	
			ErrorMessage(tkSHX_FILE_EXISTS);
			return S_OK;
		}
		if( Utility::fileExists(sa_dbffileName) )
		{	
			ErrorMessage(tkDBF_FILE_EXISTS);
			return S_OK;
		}

		// -----------------------------------------------
		//   checking in-memory shapes
		// -----------------------------------------------
		if( _isEditingShapes )
		{	
			if( verifyMemShapes(cBack) == FALSE )
			{	
				// error Code is set in function
				return S_OK;
			}

			// refresh extents
			VARIANT_BOOL retVal;
			this->RefreshExtents(&retVal);
		}		
		
		// -----------------------------------------------
		// opening files
		// -----------------------------------------------
		FILE * sa_shpfile = fopen( sa_shpfileName, "wb+" );		
		if( sa_shpfile == NULL )
		{	
			ErrorMessage(tkCANT_CREATE_SHP);
			return S_OK;
		}
		
		//shx
		FILE * sa_shxfile = fopen( sa_shxfileName, "wb+" );
		if( sa_shxfile == NULL )
		{	
			fclose( sa_shpfile );
			sa_shpfile = NULL;
			ErrorMessage(tkCANT_CREATE_SHX);
			return S_OK;
		}

		// ------------------------------------------------
		//	writing the files
		// ------------------------------------------------
		this->writeShp(sa_shpfile,cBack);
		this->writeShx(sa_shxfile,cBack);

		fclose(sa_shpfile);
		fclose(sa_shxfile);
		//_unlink(sa_shpfileName);
		//_unlink(sa_shxfileName);

		// ------------------------------------------------
		//	saving dbf table
		// ------------------------------------------------
		dbf->Dump(sa_dbffileName.AllocSysString(), cBack, retval);
		if( *retval == FALSE )
		{	
			dbf->get_LastErrorCode(&lastErrorCode);
			return S_OK;
		}

		// saving projection in new format
		VARIANT_BOOL vbretval;
		CStringW prjfileName = sa_shpfileName.Left(sa_shpfileName.GetLength() - 3) + L"prj";
		m_geoProjection->WriteToFile(W2BSTR(_prjfileName), &vbretval);

		*retval = VARIANT_TRUE;
	}
	
	// restoring callback
	if (callbackIsNull)
		globalCallback = NULL;
	return S_OK;
}

// **********************************************************
//		SaveAs()
// **********************************************************
// Saves shapefile to the new or the same location. Doesn't change the editing state (source type) 
STDMETHODIMP CShapefile::SaveAs(BSTR ShapefileName, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;

	bool callbackIsNull = (globalCallback == NULL);
	if(globalCallback == NULL && cBack != NULL)
	{
		globalCallback = cBack;
		globalCallback->AddRef();
	}

	if (dbf == NULL || m_sourceType == sstUninitialized)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
		return S_FALSE;
	}

	// in case someone else is writing, we leave
	if (m_writing)
	{
		ErrorMessage(tkSHP_WRITE_VIOLATION);
		return S_FALSE;
	}
	
	if (!this->ValidateOutput(this, "SaveAs", "Shapefile", false))
		return S_FALSE;

	USES_CONVERSION;
	CString sa_shpfileName;
	sa_shpfileName = OLE2CA(ShapefileName);

	if( sa_shpfileName.GetLength() <= 3 )
	{	
		ErrorMessage(tkINVALID_FILENAME);
	}
	else
	{
		CString sa_shxfileName = sa_shpfileName.Left(sa_shpfileName.GetLength() - 3) + "shx";
		CString sa_dbffileName = sa_shpfileName.Left(sa_shpfileName.GetLength() - 3) + "dbf";
	
		// -----------------------------------------------
		// it's not allowed to rewrite the existing files
		// -----------------------------------------------
		if( Utility::fileExists(sa_shpfileName) )
		{	
			ErrorMessage(tkSHP_FILE_EXISTS);
			return S_OK;
		}
		if( Utility::fileExists(sa_shxfileName) )
		{	
			ErrorMessage(tkSHX_FILE_EXISTS);
			return S_OK;
		}
		if( Utility::fileExists(sa_dbffileName) )
		{	
			ErrorMessage(tkDBF_FILE_EXISTS);
			return S_OK;
		}

		// -----------------------------------------------
		//   checking in-memory shapes
		// -----------------------------------------------
		if( _isEditingShapes )
		{	
			if( verifyMemShapes(cBack) == FALSE )
			{	
				// error Code is set in function
				return S_OK;
			}

			// refresh extents
			VARIANT_BOOL retVal;
			this->RefreshExtents(&retVal);
		}		
		
		// -----------------------------------------------
		// opening files
		// -----------------------------------------------
		FILE * sa_shpfile = fopen( sa_shpfileName, "wb+" );		
		if( sa_shpfile == NULL )
		{	
			ErrorMessage(tkCANT_CREATE_SHP);
			return S_OK;
		}
		
		//shx
		FILE * sa_shxfile = fopen( sa_shxfileName, "wb+" );
		if( sa_shxfile == NULL )
		{	
			fclose( sa_shpfile );
			sa_shpfile = NULL;
			ErrorMessage(tkCANT_CREATE_SHX);
			return S_OK;
		}

		// ------------------------------------------------
		//	writing the files
		// ------------------------------------------------
		this->writeShp(sa_shpfile,cBack);
		this->writeShx(sa_shxfile,cBack);

		fclose(sa_shpfile);
		fclose(sa_shxfile);

		sa_shpfile = fopen( sa_shpfileName, "rb" );		
		sa_shxfile = fopen( sa_shxfileName, "rb" );		

		// ------------------------------------------------
		//	saving dbf table
		// ------------------------------------------------
		dbf->SaveAs(sa_dbffileName.AllocSysString(), cBack, retval);
		if( *retval == FALSE )
		{	
			dbf->get_LastErrorCode(&lastErrorCode);
			fclose(sa_shpfile);
			fclose(sa_shxfile);
			_unlink(sa_shpfileName);
			_unlink(sa_shxfileName);
			return S_OK;
		}
		
		// -------------------------------------------------
		//	switching to the new files
		// -------------------------------------------------
		if( _shpfile != NULL )	fclose(_shpfile);
		_shpfile = sa_shpfile;
		
		if( _shxfile != NULL )	fclose(_shxfile);
		_shxfile = sa_shxfile;
	
		// update all filenames:
		_shpfileName = sa_shpfileName;	// lsu: saving of shp filename should be done before writing the projection;
		_shxfileName = sa_shxfileName;	// otherwise projection string will be written to the memory
		_dbffileName = sa_dbffileName;
		_prjfileName = sa_shpfileName.Left(sa_shpfileName.GetLength() - 3) + "prj";

		// projection will be written to the disk after this
		m_sourceType = sstDiskBased;	

		// saving projection in new format
		VARIANT_BOOL vbretval, isEmpty;
		m_geoProjection->get_IsEmpty(&isEmpty);
		if(!isEmpty)
			m_geoProjection->WriteToFile(W2BSTR(_prjfileName), &vbretval);

		if (useQTree)
			GenerateQTree();

		*retval = VARIANT_TRUE;
	}
	
	// restoring callback
	if (callbackIsNull)
		globalCallback = NULL;
	return S_OK;
}

// **************************************************************
//		Save()
// **************************************************************
//Neio, 2009/07/23, add "Save" Method for saving without exiting edit mode
STDMETHODIMP CShapefile::Save(ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;

	if (globalCallback == NULL && cBack != NULL)
	{
		globalCallback = cBack;
		globalCallback->AddRef();
	}
	
	// no edits were made; it doesn't make sense to save it
	if( _isEditingShapes == FALSE )
	{	
		ErrorMessage( tkSHPFILE_NOT_IN_EDIT_MODE );
		return S_OK;
	}

	if( verifyMemShapes(globalCallback) == FALSE )
	{	
		// error Code is set in function
		return S_OK;
	}
	
	if (!this->ValidateOutput(this, "Save", "Shapefile", false))
		return S_FALSE;

	// compute the extents
	VARIANT_BOOL res;
	RefreshExtents(&res);

	// -------------------------------------------------
	//	Reopen the files in the write mode
	// -------------------------------------------------
	if (_shpfile && _shxfile)
	{
		_shpfile = _wfreopen(_shpfileName,L"wb+",_shpfile);
		_shxfile = _wfreopen(_shxfileName,L"wb+",_shxfile);
	}
	else
	{
		_shpfile = _wfopen(_shpfileName,L"wb+");
		_shxfile = _wfopen(_shxfileName,L"wb+");
	}

	if( _shpfile == NULL || _shxfile == NULL )
	{	
		if( _shxfile != NULL )
		{	
			fclose( _shxfile );
			_shxfile = NULL;
			lastErrorCode = tkCANT_OPEN_SHX;
		}
		if( _shpfile != NULL )
		{	
			fclose( _shpfile );
			_shpfile = NULL;
			lastErrorCode = tkCANT_OPEN_SHP;
		}
		*retval = FALSE;
		
		ErrorMessage(lastErrorCode);
		
	}
	else
	{
		m_writing = true;

		// -------------------------------------------------
		//	Writing the files
		// -------------------------------------------------
		writeShp(_shpfile,cBack);
		writeShx(_shxfile,cBack);
		
		if (useQTree)
			GenerateQTree();

		// -------------------------------------------------
		//	Reopen the updated files
		// -------------------------------------------------
		_shpfile = _wfreopen(_shpfileName,L"rb+",_shpfile);
		_shxfile = _wfreopen(_shxfileName,L"rb+",_shxfile);
		
		if( _shpfile == NULL || _shxfile == NULL )
		{	
			if( _shxfile != NULL )
			{	
				fclose( _shxfile );
				_shxfile = NULL;
				lastErrorCode = tkCANT_OPEN_SHX;
			}
			if( _shpfile != NULL )
			{	
				fclose( _shpfile );
				_shpfile = NULL;
				lastErrorCode = tkCANT_OPEN_SHP;
			}
			*retval = FALSE;
			
			ErrorMessage( lastErrorCode );
		}
		else
		{
			//Save the table file
			dbf->Save(cBack,retval);
			
			// projection will be written to the disk after this
			m_sourceType = sstDiskBased;	

			// saving projection in new format
			VARIANT_BOOL vbretval;
			m_geoProjection->WriteToFile(W2BSTR(_prjfileName), &vbretval);

			*retval = VARIANT_TRUE;
		}
	}
	m_writing = false;
	return S_OK;
}

// ************************************************************
//		Resource()
// ************************************************************
STDMETHODIMP CShapefile::Resource(BSTR newShpPath, VARIANT_BOOL *retval)
{
	USES_CONVERSION;
	Close(retval);
	Open(newShpPath, NULL, retval);
	return S_OK;
}
#pragma endregion

#pragma region Clone
//TODO: move to shapefile helper class

// ***********************************************************************
//		CloneNoFields()
// ***********************************************************************
void CShapefile::CloneNoFields(IShapefile** retVal, bool addShapeId)
{
	CloneNoFields(retVal, _shpfiletype, addShapeId);
}

// ***********************************************************************
//		CloneNoFields()
// ***********************************************************************
void CShapefile::CloneNoFields(IShapefile** retVal, ShpfileType shpType, bool addShapeId)
{
	IShapefile* sf = NULL;
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)&sf);
	
	VARIANT_BOOL vb;
	if(addShapeId)
	{
		sf->CreateNewWithShapeID(A2BSTR(""), shpType, &vb);
	}
	else {
		sf->CreateNew(A2BSTR(""), shpType, &vb);
	}
	
	// copying the projection string
	BSTR pVal;
	this->get_Projection(&pVal);
	if (pVal != NULL)
		sf->put_Projection(pVal);

	ICallback* callback = NULL;
	this->get_GlobalCallback(&callback);
	sf->put_GlobalCallback(callback);

	*retVal = sf;
}

// ***********************************************************************
//		Clone()
// ***********************************************************************
//  Creates new shapefile with the same type and fields as existing one
STDMETHODIMP CShapefile::Clone(IShapefile** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	this->CloneCore(retVal, _shpfiletype);
	return S_OK;
}

// ***********************************************************************
//		CloneCore()
// ***********************************************************************
void CShapefile::CloneCore(IShapefile** retVal, ShpfileType shpType, bool addShapeId)
{
	CloneNoFields(retVal, shpType, addShapeId);
	VARIANT_BOOL vbretval;

	IShapefile* sf = *retVal;

	long numFields;
	this->get_NumFields(&numFields);

	for (long i = 0; i < numFields; i++)
	{
		IField * fld = NULL;
		IField * fldNew = NULL;
		this->get_Field(i,&fld);
		fld->Clone(&fldNew);
		fld->Release();
		
		sf->EditInsertField(fldNew, &i, NULL, &vbretval);
		fldNew->Release();

		if (!vbretval)
		{
			sf->Close(&vbretval);
			sf->Release(); 
			sf = NULL; 
			break;
		}
	}
	
	
}
#pragma endregion

#pragma region Extents
// ************************************************************
//		get_Extents()
// ************************************************************
STDMETHODIMP CShapefile::get_Extents(IExtents **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	IExtents * bBox = NULL;
	CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&bBox );
	
	// Extents could change because of the moving of points of a single shape
	// It's difficult to track such changes, so we still need to recalculate them 
	// here to enforce proper drawing; _fastMode mode - for those who want 
	// to call refresh extents theirselfs
	if (!_fastMode)
	{
		VARIANT_BOOL vbretval;
		this->RefreshExtents(&vbretval);
	}

	bBox->SetBounds(_minX,_minY,_minZ,_maxX,_maxY,_maxZ);
	bBox->SetMeasureBounds(_minM,_maxM);
	*pVal = bBox;

	return S_OK;
}
#pragma endregion

#pragma region AttributeTable
// ****************************************************************
//	  EditInsertField()
// ****************************************************************
STDMETHODIMP CShapefile::EditInsertField(IField *NewField, long *FieldIndex, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if(cBack == NULL && globalCallback!=NULL)
		cBack = globalCallback;	

	if( dbf != NULL )
	{
		dbf->EditInsertField(NewField,FieldIndex,cBack,retval);
	}
	else
	{	
		*retval = VARIANT_FALSE;
		lastErrorCode = tkFILE_NOT_OPEN;
		if( cBack != NULL )
			cBack->Error( OLE2BSTR(key),  A2BSTR(ErrorMsg(lastErrorCode) ) );
		return S_OK;
	}

	if( *retval == VARIANT_FALSE )
	{	
		dbf->get_LastErrorCode(&lastErrorCode);
		*retval = VARIANT_FALSE;
	}

	return S_OK;
}

// ****************************************************************
//	  EditDeleteField()
// ****************************************************************
STDMETHODIMP CShapefile::EditDeleteField(long FieldIndex, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if(globalCallback == NULL && cBack != NULL)
	{
		globalCallback = cBack;	
		globalCallback->AddRef();
	}

	if( dbf != NULL )
	{
		dbf->EditDeleteField(FieldIndex,cBack,retval);
	}
	else
	{	
		*retval = VARIANT_FALSE;
		ErrorMessage(tkFILE_NOT_OPEN);
		return S_OK;
	}

	if( *retval == VARIANT_FALSE )
	{	
		dbf->get_LastErrorCode(&lastErrorCode);
		*retval = VARIANT_FALSE;
	}

	return S_OK;
}

// ****************************************************************
//	  EditCellValue()
// ****************************************************************
STDMETHODIMP CShapefile::EditCellValue(long FieldIndex, long ShapeIndex, VARIANT NewVal, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( dbf != NULL )
	{
		dbf->EditCellValue(FieldIndex,ShapeIndex,NewVal,retval);
	}
	else
	{	
		*retval = VARIANT_FALSE;
		ErrorMessage(tkFILE_NOT_OPEN);
		return S_OK;
	}

	if( *retval == VARIANT_FALSE )
	{	
		dbf->get_LastErrorCode(&lastErrorCode);
		*retval = VARIANT_FALSE;
	}

	return S_OK;
}

// ****************************************************************
//	  StartEditingTable()
// ****************************************************************
STDMETHODIMP CShapefile::StartEditingTable(ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if(globalCallback == NULL && cBack != NULL)
	{
		globalCallback = cBack;
		globalCallback->AddRef();
	}

	if( dbf != NULL )
	{
		dbf->StartEditingTable(cBack,retval);
	}
	else
	{	
		*retval = VARIANT_FALSE;
		ErrorMessage(tkFILE_NOT_OPEN);
		return S_OK;
	}

	if( *retval == VARIANT_FALSE )
	{	
		dbf->get_LastErrorCode(&lastErrorCode);
		*retval = VARIANT_FALSE;
	}

	return S_OK;
}

// ****************************************************************
//	  StopEditingTable()
// ****************************************************************
STDMETHODIMP CShapefile::StopEditingTable(VARIANT_BOOL ApplyChanges, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if(globalCallback == NULL && cBack!=NULL)
	{
		globalCallback = cBack;
		globalCallback->AddRef();
	}

	if( dbf != NULL )
	{
		dbf->StopEditingTable(ApplyChanges,cBack,retval);
	}
	else
	{	
		*retval = VARIANT_FALSE;
		ErrorMessage(tkFILE_NOT_OPEN);
		return S_OK;
	}

	if( *retval == FALSE )
	{	
		dbf->get_LastErrorCode(&lastErrorCode);
		*retval = VARIANT_FALSE;
	}

	return S_OK;
}

// *****************************************************************
//	   get_Field()
// *****************************************************************
STDMETHODIMP CShapefile::get_Field(long FieldIndex, IField **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( dbf != NULL )
	{
		dbf->get_Field(FieldIndex,pVal);	
		if(*pVal != NULL)
		{
			// we need to report error from field class, and will use callback from this class for it
			ICallback* cBack = NULL;
			if ((*pVal)->get_GlobalCallback(&cBack) == NULL && this->globalCallback != NULL)
				(*pVal)->put_GlobalCallback(globalCallback);
			
			if (cBack != NULL)
				cBack->Release();	// we put a reference in field class so must release it here
		}
	}
	else
	{	
		ErrorMessage(tkFILE_NOT_OPEN);
		return S_OK;
	}

	return S_OK;
}

// *****************************************************************
//	   get_FieldByName()
// *****************************************************************
STDMETHODIMP CShapefile::get_FieldByName(BSTR Fieldname, IField **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	
    long max;
	BSTR Testname;
	CString strTestname;
	CString strFieldname;
	IField *testVal;
	
	dbf->get_NumFields(&max);
	if( dbf != NULL )
	{
        if( _tcslen( OLE2CA(Fieldname) ) > 0 )
		{
			strFieldname = OLE2A(Fieldname);
		}
		else
		{
			ErrorMessage(tkZERO_LENGTH_STRING);
		}

		for (int fld=0; fld < max; fld++)
		{
			dbf->get_Field(fld,&testVal);
			testVal->get_Name(&Testname);
			strTestname = OLE2A(Testname);
			if( strTestname.CompareNoCase(strFieldname) == 0)
			{
				*pVal = testVal;
				return S_OK;
			}
			else
			{
				testVal->Release();
			}
		}
	}
	else
	{	
		lastErrorCode = tkFILE_NOT_OPEN;
		if( globalCallback != NULL )
			globalCallback->Error( OLE2BSTR(key),  A2BSTR(ErrorMsg(lastErrorCode) ) );
		return S_OK;
	}
	
	// we did not have a file error, but we also didn't match the name
	pVal = NULL;
	return S_OK;	
}

// *****************************************************************
//	   FindNewShapeID()
// *****************************************************************
long CShapefile::FindNewShapeID(long FieldIndex)
{
    if (dbf == NULL)
	{
		return 0;
	}
	else
	{
		long rt = -1;
		long lo = 0;

		int size = _shapeData.size();
		for (int i = 0; i < size - 1; i++)
		{
			VARIANT pVal;
			VariantInit(&pVal);
			dbf->get_CellValue(FieldIndex, i, &pVal);
			lVal(pVal, lo);
			VariantClear(&pVal);
			if (lo > rt) rt = lo;
		}
		return rt + 1;
	}
}

// *****************************************************************
//	   get_CellValue()
// *****************************************************************
STDMETHODIMP CShapefile::get_CellValue(long FieldIndex, long ShapeIndex, VARIANT *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( dbf != NULL )
	{
		dbf->get_CellValue(FieldIndex,ShapeIndex,pVal);
	}
	else
	{	
		ErrorMessage(tkFILE_NOT_OPEN);
		return S_OK;
	}

	return S_OK;
}

// *****************************************************************
//	   get_EditingTable()
// *****************************************************************
STDMETHODIMP CShapefile::get_EditingTable(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( dbf != NULL )
	{
		dbf->get_EditingTable(pVal);
	}
	else
	{	
		*pVal = VARIANT_FALSE;
		ErrorMessage(tkFILE_NOT_OPEN);
		return S_OK;
	}

	return S_OK;
}

// *************************************************************
//		get_Table()
// *************************************************************
STDMETHODIMP CShapefile::get_Table(ITable** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = dbf;
	if ( dbf )
	{
		dbf->AddRef();
	}
	return S_OK;
}

//*********************************************************************
//*						UniqueFieldNames()				              
//*********************************************************************
// Makes name of fields in dbf table unique.	 
// In case of duplicated names adds _# to them	 
bool CShapefile::UniqueFieldNames(IShapefile* sf)
{
	VARIANT_BOOL editing;
	USES_CONVERSION;

	// Do we need edit mode for editing of the field names?
	// Yes we do, shapelib doesn't allow it otherwise ;)
	sf->get_EditingShapes(&editing);	
	if (!editing) 
		return false;
	
	long numFields;
	sf->get_NumFields(&numFields);
	
	set<CString> fields;

	for(long i = 0; i< numFields; i++)
	{
		BSTR name;
		IField* fld;
		sf->get_Field(i, &fld);
		fld->get_Name(&name);

		if (fields.find(OLE2CA(name)) == fields.end())
		{
			fields.insert(OLE2CA(name));
		}
		else
		{	
			bool found = false;
			for(int j =0; !found ;j++)
			{
				CString temp = OLE2CA(name);
				temp.AppendFormat("_%d", j);
				if (fields.find(temp) == fields.end())
				{	
					fields.insert(temp);
					name = temp.AllocSysString();
					fld->put_Name(name);
					found = true;
				}
			}
		}
		fld->Release();
	}
	fields.clear();
	return true;
}
#pragma endregion

#pragma region DrawingOptions
// *************************************************************
//		get_ShapeCategory()
// *************************************************************
STDMETHODIMP CShapefile::get_ShapeCategory(long ShapeIndex, long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size()) //_numShapes)
	{	
		*pVal = -1;
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
		*pVal = _shapeData[ShapeIndex]->category; 
	return S_OK;
}
STDMETHODIMP CShapefile::put_ShapeCategory(long ShapeIndex, long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size()) //_numShapes )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
		_shapeData[ShapeIndex]->category = (int)newVal;

	return S_OK;
}

// *************************************************************
//		get_ShapeCategory2()
// *************************************************************
STDMETHODIMP CShapefile::put_ShapeCategory2(long ShapeIndex, BSTR categoryName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		int index;
		m_categories->get_CategoryIndexByName(categoryName, &index);
		if (index == -1)
		{
			ErrorMessage(tkCATEGORY_WASNT_FOUND);
		}
		else
		{
			_shapeData[ShapeIndex]->category = (int)index;
		}
	}
	return S_OK;
}

STDMETHODIMP CShapefile::get_ShapeCategory2(long ShapeIndex, BSTR* categoryName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*categoryName = A2BSTR("");
	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		int index = _shapeData[ShapeIndex]->category;
		long count;
		m_categories->get_Count(&count);
		if (index >= 0 && index < count)
		{
			IShapefileCategory* ct;
			m_categories->get_Item(index, &ct);
			ct->get_Name(categoryName);
			ct->Release();
		}
		else
		{
			ErrorMessage(tkCATEGORY_WASNT_FOUND);
		}
	}
	return S_OK;
}

// *************************************************************
//		put_ShapeCategory3()
// *************************************************************
STDMETHODIMP CShapefile::put_ShapeCategory3(long ShapeIndex, IShapefileCategory* category)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		int index;
		m_categories->get_CategoryIndex(category, &index);
		if (index == -1)
		{
			ErrorMessage(tkCATEGORY_WASNT_FOUND);
		}
		else
		{
			_shapeData[ShapeIndex]->category = (int)index;
		}
	}
	return S_OK;
}

STDMETHODIMP CShapefile::get_ShapeCategory3(long ShapeIndex, IShapefileCategory** category)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*category = NULL;
	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		int index = _shapeData[ShapeIndex]->category;
		long count;
		m_categories->get_Count(&count);
		if (index >= 0 && index < count)
		{
			IShapefileCategory* ct;
			m_categories->get_Item(index, &ct);
			*category = ct;		// ref was added in the get_Item
		}
		else
		{
			ErrorMessage(tkCATEGORY_WASNT_FOUND);
		}
	}
	return S_OK;
}

// *******************************************************************
//  	SelectionDrawingOptions()
// *******************************************************************
//  Returns and sets parameters used to draw selection for the shapefile.
STDMETHODIMP CShapefile::get_SelectionDrawingOptions(IShapeDrawingOptions** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_selectDrawOpt;
	if (m_selectDrawOpt)
		m_selectDrawOpt->AddRef();
	return S_OK;
}
STDMETHODIMP CShapefile::put_SelectionDrawingOptions(IShapeDrawingOptions* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!newVal)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE); 
	}
	else
	{
		Utility::put_ComReference(newVal, (IDispatch**)&m_selectDrawOpt, false);
	}
	return S_OK;
}

// *******************************************************************
//		DeafultDrawingOptions()				          
// *******************************************************************
// Returns and sets parameters used to draw shapefile by default.
STDMETHODIMP CShapefile::get_DefaultDrawingOptions(IShapeDrawingOptions** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_defaultDrawOpt;
	if (m_defaultDrawOpt)
		m_defaultDrawOpt->AddRef();
	return S_OK;
}
STDMETHODIMP CShapefile::put_DefaultDrawingOptions(IShapeDrawingOptions* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (!newVal)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE); 
	}
	else
	{
		Utility::put_ComReference(newVal, (IDispatch**)&m_defaultDrawOpt);
	}
	return S_OK;
}

/***********************************************************************/
/*		put_ReferenceToCategories
/***********************************************************************/
void CShapefile::put_ReferenceToCategories(bool bNullReference)
{
	if (m_categories == NULL) return;
	CShapefileCategories* coCategories = static_cast<CShapefileCategories*>(m_categories);
	//CShapefile* coShapefile = static_cast<CShapefile*>(this);
	if (!bNullReference)
		coCategories->put_ParentShapefile(this);
	else
		coCategories->put_ParentShapefile(NULL);
};

/***********************************************************************/
/*		get/put_Categories
/***********************************************************************/
STDMETHODIMP CShapefile::get_Categories(IShapefileCategories** pVal)
{
	*pVal = m_categories;
	if (m_categories != NULL)
		m_categories->AddRef();
	return S_OK;
}
STDMETHODIMP CShapefile::put_Categories(IShapefileCategories* newVal)
{
	if (Utility::put_ComReference((IDispatch*)newVal, (IDispatch**) &m_categories, false))	
	{
		((CShapefileCategories*)m_categories)->put_ParentShapefile(this);
	}
	return S_OK;
}
#pragma endregion

// ********************************************************************
//		get_SelectionColor
// ********************************************************************
STDMETHODIMP CShapefile::get_SelectionColor(OLE_COLOR* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = _selectionColor;
	return S_OK;
}
STDMETHODIMP CShapefile::put_SelectionColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_selectionColor = newVal;
	return S_OK;
}

// ********************************************************************
//		get_SelectionTransparency
// ********************************************************************
STDMETHODIMP CShapefile::get_SelectionTransparency (BYTE* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = _selectionTransparency;
	return S_OK;
}
STDMETHODIMP CShapefile::put_SelectionTransparency (BYTE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal > 255) newVal = 255;
	if (newVal < 0) newVal = 0;
	_selectionTransparency = newVal;
	return S_OK;
}

// ********************************************************************
//		get_SelectionAppearance
// ********************************************************************
STDMETHODIMP CShapefile::get_SelectionAppearance(tkSelectionAppearance* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = _selectionAppearance;
	return S_OK;
}
STDMETHODIMP CShapefile::put_SelectionAppearance(tkSelectionAppearance newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_selectionAppearance = newVal;
	return S_OK;
}

// ********************************************************************
//		get_PointCollisionMode
// ********************************************************************
STDMETHODIMP CShapefile::get_CollisionMode(tkCollisionMode* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = _collisionMode;
	return S_OK;
}
STDMETHODIMP CShapefile::put_CollisionMode(tkCollisionMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_collisionMode = newVal;
	return S_OK;
}

#pragma region "Seialization"
// ********************************************************
//     Serialize()
// ********************************************************
STDMETHODIMP CShapefile::Serialize(VARIANT_BOOL SaveSelection, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	CPLXMLNode* psTree = this->SerializeCore(VARIANT_TRUE, "ShapefileClass");
	if (!psTree)
	{
		*retVal = A2BSTR("");
	}
	else
	{
		CString str = CPLSerializeXMLTree(psTree);
		CPLDestroyXMLNode(psTree);
		*retVal = A2BSTR(str);
	}
	return S_OK;
}

// ********************************************************
//     SerializeCore()
// ********************************************************
 CPLXMLNode* CShapefile::SerializeCore(VARIANT_BOOL SaveSelection, CString ElementName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	CPLXMLNode* psTree = CPLCreateXMLNode( NULL, CXT_Element, ElementName );
	
	if (psTree)
	{
		CString s =  OLE2CA(_expression);
		if (s != "")
			Utility::CPLCreateXMLAttributeAndValue(psTree, "VisibilityExpression", s);

		if (_fastMode != FALSE)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "FastMode", CPLString().Printf("%d", (int)_fastMode));

		if (useQTree != FALSE)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "UseQTree", CPLString().Printf("%d", (int)useQTree));

		if (_collisionMode != LocalList )
			Utility::CPLCreateXMLAttributeAndValue(psTree, "CollisionMode", CPLString().Printf("%d", (int)_collisionMode));

		if (_selectionAppearance != saSelectionColor)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "SelectionAppearance", CPLString().Printf("%d", (int)_selectionAppearance));

		if (_selectionColor != RGB(255, 255, 0))
			Utility::CPLCreateXMLAttributeAndValue(psTree, "SelectionColor", CPLString().Printf("%d", (int)_selectionColor));

		if (_selectionTransparency != 180)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "SelectionTransparency", CPLString().Printf("%d", (int)_selectionTransparency));

		if (_minDrawingSize != 1)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MinDrawingSize", CPLString().Printf("%d", _minDrawingSize));

		// for in-memory shapefiles only
		if (m_sourceType == sstInMemory)
			Utility::CPLCreateXMLAttributeAndValue(psTree, "ShpType", CPLString().Printf("%d", (int)this->_shpfiletype));

		// drawing options
		CPLXMLNode* node = ((CShapeDrawingOptions*)m_defaultDrawOpt)->SerializeCore("DefaultDrawingOptions");
		if (node)
		{
			CPLAddXMLChild(psTree, node);
		}
		
		if(_selectionAppearance == saDrawingOptions)
		{
			CPLXMLNode* node = ((CShapeDrawingOptions*)m_selectDrawOpt)->SerializeCore("SelectionDrawingOptions");
			if (node)
			{
				CPLAddXMLChild(psTree, node);
			}
		}

		// categories
		node = ((CShapefileCategories*)m_categories)->SerializeCore("ShapefileCategoriesClass");
		if (node)
		{
			CPLAddXMLChild(psTree, node);
		}

		// labels
		CPLXMLNode* psLabels = ((CLabels*)m_labels)->SerializeCore("LabelsClass");
		if (psLabels)
		{
			CPLAddXMLChild(psTree, psLabels);
		}

		// charts
		CPLXMLNode* psCharts = ((CCharts*)m_charts)->SerializeCore("ChartsClass");
		if (psCharts)
		{
			CPLAddXMLChild(psTree, psCharts);
		}

		// ----------------------------------------------------
		// selection
		// ----------------------------------------------------
		long numSelected;
		this->get_NumSelected(&numSelected);
		
		if (numSelected > 0 && SaveSelection)
		{
			char* selection = new char[_shapeData.size() + 1];
			selection[_shapeData.size()] = '\0';
			for (unsigned int i = 0; i < _shapeData.size(); i++)
			{
				selection[i] = _shapeData[i]->selected ? '1' : '0';
			}
		
			CPLXMLNode* nodeSelection = CPLCreateXMLElementAndValue(psTree, "Selection", selection);
			if (nodeSelection)
			{
				Utility::CPLCreateXMLAttributeAndValue(nodeSelection, "TotalCount", CPLString().Printf("%d", _shapeData.size()));
				Utility::CPLCreateXMLAttributeAndValue(nodeSelection, "SelectedCount", CPLString().Printf("%d", numSelected));
			}
			delete[] selection;
		}

		// ----------------------------------------------------
		// serialization of category indices
		// ----------------------------------------------------
		bool serializeCategories;
		for(size_t i = 0; i < _shapeData.size(); i++) {
			if (_shapeData[i]->category != -1) {
				serializeCategories = true;
			}
		}

		if (serializeCategories) 
		{
			s = "";
			// doing it with CString is ugly of course, better to allocate a buffer
			CString temp;
			for(size_t i = 0; i < _shapeData.size(); i++) {
				temp.Format("%d,", _shapeData[i]->category);
				s +=  temp;
			}
		}
		
		// when there are no indices assigned, write an empty node with Count = 0;
		// to signal, that categories must not be applied automatically (behavior for older versions)
		CPLXMLNode* nodeCats = CPLCreateXMLElementAndValue(psTree, "CategoryIndices", s.GetBuffer());
		if (nodeCats) {
			Utility::CPLCreateXMLAttributeAndValue(nodeCats, "Count", CPLString().Printf("%d", serializeCategories ? _shapeData.size() : 0));
		}

		// ----------------------------------------------------
		// table
		// ----------------------------------------------------
		if (dbf) {
			CPLXMLNode* psTable = ((CTableClass*)dbf)->SerializeCore("TableClass");
			if (psTable) {
				CPLAddXMLChild(psTree, psTable);
			}
		}
	}
	return psTree;
}

// ********************************************************
//     Deserialize()
// ********************************************************
STDMETHODIMP CShapefile::Deserialize(VARIANT_BOOL LoadSelection, BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	CString s = OLE2CA(newVal);
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		CPLXMLNode* nodeSf = CPLGetXMLNode(node, "=ShapefileClass");
		if (nodeSf)
		{
			this->DeserializeCore(VARIANT_TRUE, nodeSf);
		}
		CPLDestroyXMLNode(node);
	}
	return S_OK;
}

// ********************************************************
//     DeserializeCore()
// ********************************************************
bool CShapefile::DeserializeCore(VARIANT_BOOL LoadSelection, CPLXMLNode* node)
{
	USES_CONVERSION;

	if (!node )
		return false;

	CString s;
	s = CPLGetXMLValue( node, "VisibilityExpression", NULL );
	_expression = A2BSTR(s);
	
	s = CPLGetXMLValue( node, "FastMode", NULL );
	bool fastMode = (s != "") ? atoi(s.GetString()) == 0 ? false : true : false;
	this->put_FastMode(fastMode);

	s = CPLGetXMLValue( node, "UseQTree", NULL );
	useQTree = (s != "") ? (BOOL)atoi(s.GetString()) : FALSE;

	s = CPLGetXMLValue( node, "CollisionMode", NULL );
	_collisionMode = (s != "") ? (tkCollisionMode)atoi(s.GetString()) : LocalList;

	s = CPLGetXMLValue( node, "SelectionAppearance", NULL );
	_selectionAppearance = (s != "") ? (tkSelectionAppearance)atoi(s.GetString()): saSelectionColor;

	s = CPLGetXMLValue( node, "SelectionColor", NULL );
	_selectionColor = (s != "") ? (OLE_COLOR)atoi(s.GetString()) : RGB(255, 255, 0);

	s = CPLGetXMLValue( node, "SelectionTransparency", NULL );
	_selectionTransparency = (s != "") ? (unsigned char)atoi(s.GetString()) : 180;

	s = CPLGetXMLValue( node, "MinDrawingSize", NULL );
	_minDrawingSize = (s != "") ? atoi(s.GetString()) : 1;

	if (m_sourceType == sstInMemory)
	{
		s = CPLGetXMLValue( node, "ShpType", NULL );
		_shpfiletype = (s != "") ? (ShpfileType)atoi(s.GetString()) : ShpfileType::SHP_NULLSHAPE;
	}

	// drawing options
	CPLXMLNode* psChild = CPLGetXMLNode(node, "DefaultDrawingOptions");
	if (psChild)
	{
		IShapeDrawingOptions* options = NULL;
		this->get_DefaultDrawingOptions(&options);
		((CShapeDrawingOptions*)options)->DeserializeCore(psChild);
		options->Release();
	}

	if (_selectionAppearance == saDrawingOptions)
	{
		CPLXMLNode* psChild = CPLGetXMLNode(node, "SelectionDrawingOptions");
		if (psChild)
		{
			IShapeDrawingOptions* options = NULL;
			this->get_SelectionDrawingOptions(&options);
			((CShapeDrawingOptions*)options)->DeserializeCore(psChild);
			options->Release();
		}
	}
	
	// Categories
	psChild = CPLGetXMLNode(node, "ShapefileCategoriesClass");
	if (psChild)
	{
		((CShapefileCategories*)m_categories)->DeserializeCore(psChild, false);
	}

	// category indices
	bool hasIndices = false;
	CPLXMLNode* nodeCats = CPLGetXMLNode(node, "CategoryIndices");

	if (nodeCats)
	{
		CString indices = CPLGetXMLValue(nodeCats, "=CategoryIndices", "");
		if (indices.GetLength() > 0)
		{
			s = CPLGetXMLValue(nodeCats, "Count", "0");
			long savedCount = atoi(s);
			int foundCount = 0;
			char* buffer = indices.GetBuffer();
			for (int i = 0; i < indices.GetLength(); i++) {
				if (buffer[i] == ',') {
					foundCount++;
				}
			}
			
			if (foundCount == savedCount && foundCount == _shapeData.size())		
			{
				int size = _shapeData.size();
				int pos = 0, count = 0;
				CString ct;
				ct = indices.Tokenize(",", pos);
				while (ct.GetLength() != 0 && count < size)
				{
					_shapeData[count]->category = atoi(ct);
					ct = indices.Tokenize(",", pos);
					count++;
				};
				hasIndices = true;
			}
		}
	}
	else
	{
		// for older versions of file without indices apply previously loaded cats
		((CShapefileCategories*)m_categories)->ApplyExpressions();
	}

	// Labels
	psChild = CPLGetXMLNode(node, "LabelsClass");
	if (psChild)
	{
		((CLabels*)m_labels)->DeserializeCore(psChild);
	}

	// Charts
	psChild = CPLGetXMLNode(node, "ChartsClass");
	if (psChild)
	{
		((CCharts*)m_charts)->DeserializeCore(psChild);
	}
	
	// selection
	CPLXMLNode* nodeSelection = CPLGetXMLNode(node, "Selection");
	if (nodeSelection && LoadSelection)
	{
		this->SelectNone();

		s = CPLGetXMLValue(nodeSelection, "TotalCount", "0");
		long count = atoi(s);
		s = CPLGetXMLValue(nodeSelection, "=Selection", "");
		if (s.GetLength() == count && s.GetLength() == _shapeData.size())
		{
			char* selection = s.GetBuffer();
			for (unsigned int i = 0; i < _shapeData.size(); i++)
			{
				if (selection[i] == '1')
				{
					_shapeData[i]->selected =  true;
				}
			}
		}
	}
	

	// table
	if (dbf) 
	{
		psChild = CPLGetXMLNode(node, "TableClass");
		if (psChild)
		{
			((CTableClass*)dbf)->DeserializeCore(psChild);
		}
	}
	return true;
}
#pragma endregion

// ******************************************************* 
//		UpdateLabelsPositioning()
// ******************************************************* 
// Should be called after change of shapefile type (CreateNew, Open, Resource, Close)
void CShapefile::UpdateLabelsPositioning()
{
	ShpfileType type = Utility::ShapeTypeConvert2D(_shpfiletype);
	
	if (m_labels)
	{
		if (type == SHP_POINT || type == SHP_MULTIPOINT)
		{
			m_labels->put_Positioning(lpCenter);
		}
		else if (type == SHP_POLYLINE)
		{
			m_labels->put_Positioning(lpLongestSegement);
		}
		else if (type == SHP_POLYGON)
		{
			m_labels->put_Positioning(lpCentroid);
		}
		else
		{
			m_labels->put_Positioning(lpNone);
		}
	}
}

#pragma region Projection
// *****************************************************************
//		get_Projection()
// *****************************************************************
STDMETHODIMP CShapefile::get_Projection(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	m_geoProjection->ExportToProj4(pVal);
	return S_OK;
}

// *****************************************************************
//		put_Projection()
// *****************************************************************
STDMETHODIMP CShapefile::put_Projection(BSTR proj4Projection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	
	VARIANT_BOOL vbretval;
	m_geoProjection->ImportFromProj4(proj4Projection, &vbretval);
	if (vbretval)
	{
		m_geoProjection->WriteToFile(W2BSTR(_prjfileName), &vbretval);
	}
	return S_OK;
}

// *****************************************************************
//		get_GeoProjection()
// *****************************************************************
STDMETHODIMP CShapefile::get_GeoProjection(IGeoProjection** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_geoProjection)
		m_geoProjection->AddRef();

	*retVal = m_geoProjection;
	return S_OK;
}

// *****************************************************************
//		put_GeoProjection()
// *****************************************************************
STDMETHODIMP CShapefile::put_GeoProjection(IGeoProjection* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	Utility::put_ComReference((IDispatch*)pVal, (IDispatch**)&m_geoProjection, false);
	if (_prjfileName.GetLength() != 0)
	{
		VARIANT_BOOL vbretval;
		m_geoProjection->WriteToFile(W2BSTR(_prjfileName), &vbretval);
	}
	return S_OK;
}

// *****************************************************************
//		Reproject()
// *****************************************************************
STDMETHODIMP CShapefile::Reproject(IGeoProjection* newProjection, LONG* reprojectedCount, IShapefile** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (!this->ReprojectCore(newProjection, reprojectedCount, retVal, false))
	{
		*retVal = NULL;
		return S_FALSE;
	}
	else
	{
		return S_OK;
	}
}

// *****************************************************************
//		ReprojectInPlace()
// *****************************************************************
STDMETHODIMP CShapefile::ReprojectInPlace(IGeoProjection* newProjection, LONG* reprojectedCount, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!_isEditingShapes)
	{
		ErrorMessage(tkSHPFILE_NOT_IN_EDIT_MODE);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		if (this->ReprojectCore(newProjection, reprojectedCount, NULL, true))
		{
			// spatial index must be deleted, as it became useful all the same
			CString name = _shpfileName.Left(_shpfileName.GetLength() - 3) + "mwd";
			if (Utility::fileExists(name))
			{
				remove(name);
			}
			name = _shpfileName.Left(_shpfileName.GetLength() - 3) + "mwx";
			if (Utility::fileExists(name))
			{
				remove(name);
			}
			
			// update qtree
			if (useQTree)
				GenerateQTree();

			VARIANT_BOOL vbretval;
			this->RefreshExtents(&vbretval);
			*retVal = VARIANT_TRUE;
			return S_OK;
		}
		else
		{
			*retVal = NULL;
			return S_FALSE;
		}
	}
	return S_OK;
}

// *****************************************************************
//		ReprojectCore()
// *****************************************************************
bool CShapefile::ReprojectCore(IGeoProjection* newProjection, LONG* reprojectedCount, IShapefile** retVal, bool reprojectInPlace)
{
	// ------------------------------------------------------
	//   Validation
	// ------------------------------------------------------
	if (!newProjection)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return false;
	}

	VARIANT_BOOL isEmpty1, isEmpty2;
	newProjection->get_IsEmpty(&isEmpty1);
	m_geoProjection->get_IsEmpty(&isEmpty2);
	if (isEmpty1 || isEmpty2)
	{
		ErrorMessage(tkPROJECTION_NOT_INITIALIZED);
		return false;
	}
	
	if (!ValidateInput(this, "Reproject/ReprojectInPlace", "this", VARIANT_FALSE))
		return false;
	
	m_globalSettings.gdalErrorMessage = "";
	OGRSpatialReference* projSource = ((CGeoProjection*)m_geoProjection)->get_SpatialReference();
	OGRSpatialReference* projTarget = ((CGeoProjection*)newProjection)->get_SpatialReference();

	OGRCoordinateTransformation* transf = OGRCreateCoordinateTransformation( projSource, projTarget );
	if (!transf)
	{
		m_globalSettings.gdalErrorMessage = CPLGetLastErrorMsg();
		ErrorMessage(tkFAILED_TO_REPROJECT);
		return false;
	}

	// ------------------------------------------------------
	//   Creating output
	// ------------------------------------------------------
	if (!reprojectInPlace)
		this->Clone(retVal);

	// ------------------------------------------------------
	//   Processing
	// ------------------------------------------------------
	CComVariant var;
	long numShapes = _shapeData.size();
	long count = 0;

	long numFields, percent = 0;
	this->get_NumFields(&numFields);
	
	VARIANT_BOOL vb = VARIANT_FALSE;
	*reprojectedCount = 0;

	for (long i = 0; i < numShapes; i++)
	{
		Utility::DisplayProgress(globalCallback, i, numShapes, "Reprojecting...", key, percent);

		IShape* shp = NULL;
		this->GetValidatedShape(i, &shp);
		if (!shp) continue;
		
		if (!reprojectInPlace)
		{
			IShape* shpNew = NULL;
			shp->Clone(&shpNew);
			shp->Release();
			shp = shpNew;
		}
		
		if (shp)
		{
			long numPoints;
			shp->get_NumPoints(&numPoints);
			
			if(numPoints > 0)
			{
				double* x = new double[numPoints];
				double* y = new double[numPoints];

				// extracting coordinates
				for (long j = 0; j < numPoints; j++)
				{
					shp->get_XY(j, x + j, y + j, &vb);
				}

				// will work faster after embedding to the CShape class
				BOOL res = transf->Transform( numPoints, x, y);
				if (!res)
				{
					if (m_globalSettings.gdalErrorMessage == "")
						m_globalSettings.gdalErrorMessage = CPLGetLastErrorMsg();
				}
				else
				{
					// saving updated coordinates
					for (long j = 0; j < numPoints; j++)
					{
						shp->put_XY(j, x[j], y[j], &vb);
					}

					if (!reprojectInPlace)
					{
						(*retVal)->get_NumShapes(&count);
						(*retVal)->EditInsertShape(shp, &count, &vb);
						
						// copying attributes
						for (long j = 0; j < numFields; j++)
						{
							this->get_CellValue(j, i, &var);
							(*retVal)->EditCellValue(j, i, var, &vb);
						}
					}
					(*reprojectedCount)++;
				}
				delete[] x; delete[] y;
			}
			shp->Release();	
		}
	}
	
	if (transf)
	{
		OGRCoordinateTransformation::DestroyCT(transf);
		transf = NULL;
	}

	// setting new projection
	if (reprojectInPlace)
	{
		m_geoProjection->CopyFrom(newProjection, &vb);
	}
	else
	{
		IGeoProjection* proj = NULL;
		(*retVal)->get_GeoProjection(&proj);
		if (proj)
		{
			proj->CopyFrom(newProjection, &vb);
			proj->Release();
		}
	}
	
	// -------------------------------------- 
	//	  Output validation
	// -------------------------------------- 
	Utility::DisplayProgressCompleted(globalCallback, key);
	ClearValidationList();
	if (!reprojectInPlace)
	{
		this->ValidateOutput(retVal, "Reproject/ReprojectInPlace", "Shapefile", false);
	}
	else
	{
		this->ValidateOutput(this, "Reproject/ReprojectInPlace", "Shapefile", false);
	}

	// it's critical to set correct projection, so false will be returned if it wasn't done
	return vb ? true : false;
}
#pragma endregion

// *****************************************************************
//		FixUpShapes()
// *****************************************************************
STDMETHODIMP CShapefile::FixUpShapes(IShapefile** retVal, VARIANT_BOOL* fixed)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*fixed = VARIANT_FALSE;
	this->Clone(retVal);
	
	if (*retVal)
	{
		long numFields;
		this->get_NumFields(&numFields);
		long percent = 0;
		int numShapes = _shapeData.size();
		*fixed = VARIANT_TRUE;

		for (int i = 0; i < numShapes; i++)
		{
			Utility::DisplayProgress(globalCallback, i, numShapes, "Fixing...", key, percent);
			
			IShape* shp = NULL;
			this->get_Shape(i, &shp);
			if (!shp)
				continue;

			IShape* shpNew = NULL;
			shp->FixUp(&shpNew);
			shp->Release();

			// failed to fix the shape? skip it.
			if (!shpNew)
				continue;
			
			long shapeIndex = 0;
			(*retVal)->get_NumShapes(&shapeIndex);

			VARIANT_BOOL vbretval = VARIANT_FALSE;
			(*retVal)->EditInsertShape(shpNew, &shapeIndex, &vbretval);
			
			if (vbretval)
			{
				// copy attributes
				CComVariant var;
				for (int iFld = 0; iFld < numFields; iFld++)
				{	
					this->get_CellValue(iFld, i, &var);
					(*retVal)->EditCellValue(iFld, shapeIndex, var, &vbretval);
				}
			}
		}
	}

	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));

	return S_OK;
}


// *********************************************************
//		GetRelatedShapes()
// *********************************************************
STDMETHODIMP CShapefile::GetRelatedShapes(long referenceIndex, tkSpatialRelation relation, VARIANT* resultArray, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = VARIANT_FALSE;
	if (referenceIndex < 0 || referenceIndex > (long)_shapeData.size())	
	{
		this->ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_FALSE;
	}

	IShape* shp = NULL;
	this->get_Shape(referenceIndex, &shp);
	if (shp)
	{
		this->GetRelatedShapeCore(shp, referenceIndex, relation, resultArray, retval);
		shp->Release();
	}
	return S_OK;
}

// *********************************************************
//		GetRelatedShapes2()
// *********************************************************
STDMETHODIMP CShapefile::GetRelatedShapes2(IShape* referenceShape, tkSpatialRelation relation, VARIANT* resultArray, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = VARIANT_FALSE;
	if (!referenceShape)	
	{
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_FALSE;
	}
	
	this->GetRelatedShapeCore(referenceShape, -1, relation, resultArray, retval);
	return S_OK;
}

// *********************************************************
//		GetRelatedShapeCore()
// *********************************************************
void CShapefile::GetRelatedShapeCore(IShape* referenceShape, long referenceIndex, tkSpatialRelation relation, VARIANT* resultArray, VARIANT_BOOL* retval)
{
	if (relation == srDisjoint)	// TODO: implement
		return;				
	
	this->ReadGeosGeometries(VARIANT_TRUE);

	// turns on the quad tree
	VARIANT_BOOL useQTree = VARIANT_FALSE;
	this->get_UseQTree(&useQTree);
	if (!useQTree) this->put_UseQTree(VARIANT_TRUE);

	double xMin, xMax, yMin, yMax;
	if(((CShape*)referenceShape)->get_ExtentsXY(xMin, yMin, xMax, yMax))
	{
		QTreeExtent query(xMin, xMax, yMax, yMin);
		std::vector<int> shapes = this->m_qtree->GetNodes(query);
		std::vector<int> arr;
		
		GEOSGeom geomBase = NULL;
		if (referenceIndex > 0)
		{
			geomBase = _shapeData[referenceIndex]->geosGeom;
		}
		else
		{
			geomBase = GeometryConverter::Shape2GEOSGeom(referenceShape);
		}

		if (geomBase)
		{
			for (size_t i = 0; i < shapes.size(); i++)
			{
				if (i == referenceIndex)
					continue;				// it doesn't make sense to compare the shape with itself

				GEOSGeom geom = _shapeData[shapes[i]]->geosGeom;
				if (geom != NULL)
				{
					char res = 0;
					switch (relation)
					{
						case srContains:	res = GeosHelper::Contains(geomBase, geom); break;
						case srCrosses:		res = GeosHelper::Crosses(geomBase, geom); break;
						case srEquals:		res = GeosHelper::Equals(geomBase, geom); break;
						case srIntersects:	res = GeosHelper::Intersects(geomBase, geom); break;
						case srOverlaps:	res = GeosHelper::Overlaps(geomBase, geom); break;
						case srTouches:		res = GeosHelper::Touches(geomBase, geom); break;
						case srWithin:		res = GeosHelper::Within(geomBase, geom); break;
					}
					if (res)
					{
						arr.push_back(shapes[i]);
					}
				}
			}
			
			if (referenceIndex == -1)
				GeosHelper::DestroyGeometry(geomBase);	// the geometry was created in this function so it must be destroyed
		}

		*retval = Templates::Vector2SafeArray(&arr, VT_I4, resultArray);
	}

	// Don't clear the list here as function may be called in a loop
	//this->ClearCachedGeometries();
}

// ***************************************************
//		get_HotTracking
// ***************************************************
STDMETHODIMP CShapefile::get_HotTracking(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_hotTracking;
	return S_OK;
}

STDMETHODIMP CShapefile::put_HotTracking(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_hotTracking = newVal;
	return S_OK;
}

// *****************************************************************
//		EditAddField()
// *****************************************************************
STDMETHODIMP CShapefile::EditAddField(BSTR name, FieldType type, int precision, int width, long *fieldIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!this->dbf)
	{
		this->ErrorMessage(tkDBF_FILE_DOES_NOT_EXIST);
	}
	else
	{
		dbf->EditAddField(name, type, precision, width, fieldIndex);
	}
	return S_OK;
}

// *****************************************************************
//		EditAddShape()
// *****************************************************************
STDMETHODIMP CShapefile::EditAddShape(IShape* shape, long* shapeIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VARIANT_BOOL retval;
	*shapeIndex = _shapeData.size();
	this->EditInsertShape(shape, shapeIndex, &retval);
	if (retval == VARIANT_FALSE)
		*shapeIndex = -1;
	return S_OK;
}

// *****************************************************************
//		getClosestPoint()
// *****************************************************************
bool CShapefile::getClosestPoint(double x, double y, double maxDistance, std::vector<long>& ids, long* shapeIndex, long* pointIndex, double& dist)
{
	VARIANT_BOOL vb;
	double minDist = DBL_MAX;
	for (size_t i = 0; i < ids.size(); i++)
	{
		IShape* shp = NULL;
		this->get_Shape(ids[i], &shp);
		if (shp)
		{
			long numPoints;
			shp->get_NumPoints(&numPoints);
			double xPnt, yPnt;
			for (long j = 0; j < numPoints; j++)
			{
				shp->get_XY(j, &xPnt, &yPnt, &vb);
				double dist = sqrt(pow(x - xPnt, 2.0) + pow(y - yPnt, 2.0));
				if (dist < minDist && dist < maxDistance)
				{
					minDist = dist;
					*shapeIndex = ids[i];
					*pointIndex = j;
				}
			}
			shp->Release();
		}
	}
	return minDist < maxDistance;
}

// *****************************************************************
//		GetClosestVertex()
// *****************************************************************
STDMETHODIMP CShapefile::GetClosestVertex(double x, double y, double maxDistance, long* shapeIndex, long* pointIndex, double* distance, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	*retVal = VARIANT_FALSE;
	*shapeIndex = -1;
	*pointIndex = -1;

	double dist = 0.0;
	bool result= false;
	if (maxDistance <= 0.0)
	{
		// search through all shapefile
		std::vector<long> ids;
		for (size_t i = 0; i < _shapeData.size(); i++) {
			ids.push_back(i);
		}
		result = this->getClosestPoint(x, y, maxDistance, ids, shapeIndex, pointIndex, dist);
	}
	else 
	{
		std::vector<long> ids;
		Extent box(x - maxDistance, x + maxDistance, y - maxDistance, y + maxDistance);
		if (this->SelectShapesCore(box, 0.0, SelectMode::INTERSECTION, ids))
		{
			result = getClosestPoint(x, y, maxDistance, ids, shapeIndex, pointIndex, dist);
		}
	}
	*retVal = result ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

// *****************************************************************
//		HasInvalidShapes()
// *****************************************************************
STDMETHODIMP CShapefile::HasInvalidShapes(VARIANT_BOOL* result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*result = VARIANT_FALSE;
	int numShapes = _shapeData.size();

	for (int i = 0; i < numShapes; i++)
	{
		IShape* shp = NULL;
		this->get_Shape(i, &shp);

		if (!shp)
		{
			*result = VARIANT_TRUE;
			break;
		}

		VARIANT_BOOL retval = VARIANT_TRUE;
		shp->get_IsValid(&retval);
		if (retval == VARIANT_FALSE)
		{
			*result = VARIANT_TRUE;
			break;			
		}
	}

	return S_OK;
}