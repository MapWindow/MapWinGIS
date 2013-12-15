#pragma region Include
#include "stdafx.h"
#include "Map.h"
#include "MapWinGis.h"
#include "GridColorScheme.h"
#include "Tiles.h"

#pragma endregion

// *********************************************************
//		SaveMapState()
// *********************************************************
VARIANT_BOOL CMapView::SaveMapState(LPCTSTR Filename, VARIANT_BOOL RelativePaths, VARIANT_BOOL Overwrite)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (Utility::fileExists(Filename))
	{
		if (!Overwrite)
		{
			ErrorMessage(tkCANT_CREATE_FILE);
			return VARIANT_FALSE;
		}
		else
		{
			if( remove( Filename ) != 0 )
			{
				ErrorMessage(tkCANT_DELETE_FILE);
				return VARIANT_FALSE;
			}
		}
	}

	CPLXMLNode* node = SerializeMapStateCore(RelativePaths, Filename);
	bool result = CPLSerializeXMLTreeToFile(node, Filename) != 0;
	CPLDestroyXMLNode(node);
	return result ? VARIANT_TRUE : VARIANT_FALSE;
}

// *********************************************************
//		LoadMapState()
// *********************************************************
VARIANT_BOOL CMapView::LoadMapState(LPCTSTR Filename, IDispatch* Callback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	try
	{
		CPLXMLNode* node = CPLParseXMLFile(Filename);
		if (node)
		{
			IStopExecution* cb = NULL;
			if (Callback)
			{
				Callback->QueryInterface(IID_IStopExecution, (void**)&cb);
			}
			
			bool result = DeserializeMapStateCore(node, Filename, VARIANT_TRUE, cb);
			
			if (cb) 
			{
				cb->Release();
			}
			return result ? VARIANT_TRUE : VARIANT_FALSE;
		}
		else
		{
			return VARIANT_FALSE;
		}
	}
	catch(...)
	{
		return VARIANT_FALSE;
	}
}

// ******************************************************************
//	   DeserializeMapStateCore()
// ******************************************************************
VARIANT_BOOL CMapView::DeserializeMapState(LPCTSTR State, VARIANT_BOOL LoadLayers, LPCTSTR BasePath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VARIANT_BOOL result = VARIANT_FALSE;
	CPLXMLNode* node = CPLParseXMLString(State);
	if (node)
	{
		result = DeserializeMapStateCore(node, BasePath, LoadLayers, NULL) ? VARIANT_TRUE : VARIANT_FALSE;
		CPLDestroyXMLNode(node);
	}
	return result;
}

// ******************************************************************
//	   DeserializeMapStateCore()
// ******************************************************************
bool CMapView::DeserializeMapStateCore(CPLXMLNode* node, CString ProjectName, VARIANT_BOOL LoadLayers, IStopExecution* callback)
{
	if (!node)
	{
		ErrorMessage(tkINVALID_FILE);
		return false;
	}
	
	if (_stricmp( node->pszValue, "MapWinGIS") != 0)
	{
		// it can be MW4 project file, then MapWinGis should be the first child node
		CPLXMLNode* nodeChild = node->psChild;
		while (nodeChild)
		{
			if (_stricmp( nodeChild->pszValue, "MapWinGIS") == 0)
			{
				// we got it
				node = nodeChild;
				break;
			}
			nodeChild = nodeChild->psNext;
		}
		
		// check once again, if it wasn't find - abandon it
		if (_stricmp( node->pszValue, "MapWinGIS") != 0)
		{
			ErrorMessage(tkINVALID_FILE);
			return false;
		}
	}
	
	CPLXMLNode* nodeState = CPLGetXMLNode(node, "MapState");
	CPLXMLNode* nodeLayers = CPLGetXMLNode(node, "Layers");
	CPLXMLNode* nodeTiles = CPLGetXMLNode(node, "Tiles");
	 
	if (!nodeState || !nodeLayers)
	{
		ErrorMessage(tkINVALID_FILE);
		return false;
	}

	// control options
	CString s;
	s = CPLGetXMLValue( nodeState, "BackColor", NULL );
	m_backColor = (s != "") ? (OLE_COLOR)atoi(s.GetString()) : RGB(255, 255, 255);
	
	s = CPLGetXMLValue( nodeState, "ExtentPad", NULL );
	m_extentPad = (s != "") ? atoi(s) : 0.02;

	s = CPLGetXMLValue( nodeState, "ExtentHistory", NULL );
	m_extentHistory = (s != "") ? atoi(s) : 20;

	s = CPLGetXMLValue( nodeState, "DoubleBuffer", NULL );
	m_doubleBuffer = (s != "") ? (BOOL)atoi(s) : TRUE;

	s = CPLGetXMLValue( nodeState, "SendMouseMove", NULL );
	m_sendMouseMove = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "SendMouseDown", NULL );
	m_sendMouseDown = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "SendMouseUp", NULL );
	m_sendMouseUp = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "SendSelectBoxDrag", NULL );
	m_sendSelectBoxDrag = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "SendSelectBoxFinal", NULL );
	m_sendSelectBoxFinal = (s != "") ? (BOOL)atoi(s) : FALSE;
	
	s = CPLGetXMLValue( nodeState, "SendOnDrawBackBuffer", NULL );
	m_sendOnDrawBackBuffer = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "ZoomPercent", NULL );
	m_zoomPercent = (s != "") ? (float)Utility::atof_custom(s) : 0.3;
	
	m_key = CPLGetXMLValue( nodeState, "Key", NULL );
	
	s = CPLGetXMLValue( nodeState, "CursorMode", NULL );
	m_cursorMode = (s != "") ? (short)atoi(s) : cmZoomIn;

	s = CPLGetXMLValue( nodeState, "MapCursor", NULL );
	m_mapCursor = (s != "") ? (short)atoi(s) : crsrMapDefault;

	s = CPLGetXMLValue( nodeState, "ResizeBehavior", NULL );
	rbMapResizeBehavior = (s != "") ? (tkResizeBehavior)atoi(s) : rbClassic;

	s = CPLGetXMLValue( nodeState, "TrapRightMouseDown", NULL );
	DoTrapRMouseDown = (s != "") ? (BOOL)atoi(s) : FALSE;
	
	s = CPLGetXMLValue( nodeState, "UseSeamlessPan", NULL );
	m_UseSeamlessPan = (s != "") ? (BOOL)atoi(s) : FALSE;
	
	s = CPLGetXMLValue( nodeState, "MouseWheelSpeed", NULL );
	m_MouseWheelSpeed = (s != "") ? Utility::atof_custom(s) : 0.5;

	s = CPLGetXMLValue( nodeState, "ShapeDrawingMethod", NULL );
	m_ShapeDrawingMethod = (s != "") ? (tkShapeDrawingMethod)atoi(s) : dmNewSymbology;

	s = CPLGetXMLValue( nodeState, "UnitsOfMeasure", NULL );
	m_unitsOfMeasure = (s != "") ? (tkUnitsOfMeasure)atoi(s) : umMeters;

	s = CPLGetXMLValue( nodeState, "DisableWaitCursor", NULL );
	m_DisableWaitCursor = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "RotationAngle", NULL );
	m_RotateAngle = (s != "") ? (float)Utility::atof_custom(s) : 0;

	s = CPLGetXMLValue( nodeState, "CanUseImageGrouping", NULL );
	_canUseImageGrouping = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "ShowRedrawTime", NULL );
	m_ShowRedrawTime = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "ShowVersionNumber", NULL );
	m_ShowVersionNumber = (s != "") ? (BOOL)atoi(s) : FALSE;

	if (LoadLayers)
	{
		// processing layers
		this->RemoveAllLayers();

		char* cwd = new char[4096];
		_getcwd(cwd,4096);
		
		ProjectName = Utility::GetFolderFromPath(ProjectName);
		_chdir(ProjectName);

		CPLXMLNode* nodeLayer = nodeLayers->psChild;
		while (nodeLayer)
		{
			if (_stricmp(nodeLayer->pszValue, "Layer") == 0)
			{
				int handle = DeserializeLayerCore( nodeLayer, ProjectName, callback);
				
				if (handle != -1)
				{
					s = CPLGetXMLValue( nodeState, "GridFilename", NULL );
					if (s != "")
					{
						node = CPLGetXMLNode(nodeState, "GridColorSchemeClass");
						if (node)
						{
							IGridColorScheme* scheme = NULL;
							CoCreateInstance(CLSID_GridColorScheme,NULL,CLSCTX_INPROC_SERVER,IID_IGridColorScheme,(void**)&scheme);
							if (scheme)
							{
								if (((CGridColorScheme*)scheme)->DeserializeCore(node))
								{
									this->SetGridFileName(handle, s);
									this->SetImageLayerColorScheme(handle, (IDispatch*)scheme);
								}
							}
						}
					}
				}
			}
			nodeLayer = nodeLayer->psNext;
		}
		_chdir(cwd);
	}

	// restoring tiles settings
	((CTiles*)m_tiles)->DeserializeCore(nodeTiles);

	// extents
	s = CPLGetXMLValue( nodeState, "ExtentsLeft", NULL );
	if (s != "") extents.left = Utility::atof_custom(s);

	s = CPLGetXMLValue( nodeState, "ExtentsRight", NULL );
	if (s != "") extents.right = Utility::atof_custom(s);
	
	s = CPLGetXMLValue( nodeState, "ExtentsBottom", NULL );
	if (s != "") extents.bottom = Utility::atof_custom(s);

	s = CPLGetXMLValue( nodeState, "ExtentsTop", NULL );
	if (s != "") extents.top = Utility::atof_custom(s);

	s = CPLGetXMLValue( nodeState, "ExtentsPad", NULL );
	if (s != "") m_extentPad = Utility::atof_custom(s);

	this->SetExtentsCore(extents);

	return true;
}

// ************************************************************
//		SerializeMapState()
// ************************************************************
BSTR CMapView::SerializeMapState(VARIANT_BOOL RelativePaths, LPCTSTR BasePath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	CPLXMLNode* node = SerializeMapStateCore(RelativePaths, BasePath);
	if (node)
	{
		strResult = CPLSerializeXMLTree(node);
		CPLDestroyXMLNode(node);
	}
	return strResult.AllocSysString();
}

// ************************************************************
//		SerializeMapStateCore()
// ************************************************************
CPLXMLNode* CMapView::SerializeMapStateCore(VARIANT_BOOL RelativePaths, CString ProjectName)
{
	CPLXMLNode* psTree = CPLCreateXMLNode( NULL, CXT_Element, "MapWinGIS");
	if (psTree) 
	{
		// TODO: implement version autoincrement
		// ocx version			
		CString s;
		s.Format("%d.%d", _wVerMajor, _wVerMinor);
		Utility::CPLCreateXMLAttributeAndValue( psTree, "OcxVersion", s);
		Utility::CPLCreateXMLAttributeAndValue( psTree, "FileType", "MapState");
		Utility::CPLCreateXMLAttributeAndValue( psTree, "FileVersion", CPLString().Printf("%d", 0));

		// control options
		CPLXMLNode* psState = CPLCreateXMLNode( NULL, CXT_Element, "MapState");
		if (psState)
		{
			if (m_backColor != RGB(255, 255, 255)) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "BackColor", CPLString().Printf("%d", m_backColor));

			if (m_extentPad != 0.02) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentPad", CPLString().Printf("%f", m_extentPad));

			if (m_extentHistory != 20) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentHistory", CPLString().Printf("%d", m_extentHistory));
			
			if (m_doubleBuffer != TRUE) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "DoubleBuffer", CPLString().Printf("%d", m_doubleBuffer));

			if (m_sendMouseMove != FALSE) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "SendMouseMove", CPLString().Printf("%d", m_sendMouseMove));

			if (m_sendMouseDown != FALSE) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "SendMouseDown", CPLString().Printf("%d", m_sendMouseDown));
			
			if (m_sendMouseUp != FALSE) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "SendMouseUp", CPLString().Printf("%d", m_sendMouseUp));

			if (m_sendSelectBoxDrag != FALSE) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "SendSelectBoxDrag", CPLString().Printf("%d", m_sendSelectBoxDrag));

			if (m_sendSelectBoxFinal != FALSE) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "SendSelectBoxFinal", CPLString().Printf("%d", m_sendSelectBoxFinal));

			if (m_sendOnDrawBackBuffer != FALSE) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "SendOnDrawBackBuffer", CPLString().Printf("%d", m_sendOnDrawBackBuffer));

			if (m_zoomPercent != 0.3) 
				Utility::CPLCreateXMLAttributeAndValue(psState, "ZoomPercent", CPLString().Printf("%f", m_zoomPercent));

			if (m_key != "")
				Utility::CPLCreateXMLAttributeAndValue(psState, "Key", m_key);
			
			if (m_cursorMode != 0)
				Utility::CPLCreateXMLAttributeAndValue(psState, "CursorMode", CPLString().Printf("%d",(int)m_cursorMode));

			if (m_mapCursor != 0)
				Utility::CPLCreateXMLAttributeAndValue(psState, "MapCursor", CPLString().Printf("%d", (int)m_mapCursor));

			if (rbMapResizeBehavior != rbClassic)
				Utility::CPLCreateXMLAttributeAndValue(psState, "ResizeBehavior", CPLString().Printf("%d", (int)rbMapResizeBehavior));

			if (DoTrapRMouseDown != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "TrapRightMouseDown", CPLString().Printf("%d", (int)DoTrapRMouseDown));

			if (m_UseSeamlessPan != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "UseSeamlessPan", CPLString().Printf("%d", (int)m_UseSeamlessPan));

			if (m_MouseWheelSpeed != 0.5)
				Utility::CPLCreateXMLAttributeAndValue(psState, "MouseWheelSpeed", CPLString().Printf("%f", m_MouseWheelSpeed));

			if (m_ShapeDrawingMethod != dmNewSymbology)
				Utility::CPLCreateXMLAttributeAndValue(psState, "ShapeDrawingMethod", CPLString().Printf("%d", (int)m_ShapeDrawingMethod));

			if (m_unitsOfMeasure != umMeters)
				Utility::CPLCreateXMLAttributeAndValue(psState, "UnitsOfMeasure", CPLString().Printf("%d", (int)m_unitsOfMeasure));

			if (m_DisableWaitCursor != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "DisableWaitCursor", CPLString().Printf("%d", (int)m_DisableWaitCursor));

			if (m_RotateAngle != 0.0f)
				Utility::CPLCreateXMLAttributeAndValue(psState, "RotationAngle", CPLString().Printf("%f", m_RotateAngle));
			
			if (_canUseImageGrouping != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "CanUseImageGrouping", CPLString().Printf("%d", (int)_canUseImageGrouping));

			if (m_ShowRedrawTime != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "ShowRedrawTime", CPLString().Printf("%d", (int)m_ShowRedrawTime));

			if (m_ShowVersionNumber != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "ShowVersionNumber", CPLString().Printf("%d", (int)m_ShowVersionNumber));

			Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentsLeft", CPLString().Printf("%f", extents.left));
			Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentsRight", CPLString().Printf("%f", extents.right));
			Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentsBottom", CPLString().Printf("%f", extents.bottom));
			Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentsTop", CPLString().Printf("%f", extents.top));
			Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentsPad", CPLString().Printf("%f", m_extentPad));
			
			CPLAddXMLChild(psTree, psState);

			// layer options
			CPLXMLNode* psLayers = CPLCreateXMLNode( NULL, CXT_Element, "Layers");
			if (psLayers)
			{
				for(unsigned int i = 0; i < m_activeLayers.size(); i++ )
				{	
					LONG handle = m_activeLayers[i];

					if (!this->GetLayerSkipOnSaving(handle))
					{
						// getting relative name
						CComBSTR layerName;
						layerName = this->GetLayerFilename(handle);
						
						USES_CONVERSION;
						CString tempLayerName = OLE2CA(layerName);
						//if (_stricmp(tempLayerName, "") == 0)
						//{
						//	// in-memory layer, it won't be saved
						//}
						//else
						{
							CString name = OLE2CA(layerName);
							if (RelativePaths)
							{
								name = Utility::GetRelativePath(ProjectName, name);
							}
							CPLXMLNode* node = this->SerializeLayerCore(handle, name);
							if (node)
							{
								// saving grid name
								BSTR s = this->GetGridFileName(handle);
								CString gridFilename = OLE2CA(s);
								if (gridFilename != "")
								{
									LPDISPATCH obj = this->GetColorScheme(handle);
									if (obj)
									{
										IGridColorScheme* scheme = NULL;
										obj->QueryInterface(IID_IGridColorScheme,(void**)&scheme);
										if (scheme)
										{
											CPLXMLNode* nodeScheme = ((CGridColorScheme*)scheme)->SerializeCore("GridColorSchemeClass");
											if (nodeScheme)
											{
												CPLAddXMLChild(node, nodeScheme);

												// it is grid layer
												Utility::CPLCreateXMLAttributeAndValue(node, "GridFilename", gridFilename);
											}
											scheme->Release();
										}
										obj->Release();
									}
								}
								CPLAddXMLChild(psLayers, node);	
							}
						}
					}
				}
				CPLAddXMLChild(psTree, psLayers);

				// adding tiles
				CPLXMLNode* nodeTiles = ((CTiles*)m_tiles)->SerializeCore("Tiles");
				if (nodeTiles)
				{
					CPLAddXMLChild(psTree, nodeTiles);
				}
			}
		}
	}
	return psTree;
}


#pragma region Obsolete
// ************************************************************
//		GetMapState()
// ************************************************************
BSTR CMapView::GetMapState()
{
	return this->SerializeMapState(VARIANT_FALSE, "");
}

// ******************************************************************
//	   SetMapState()
// ******************************************************************
// TODO: write support for the new symbology
void CMapView::SetMapState(LPCTSTR lpszNewValue)
{
	m_mapstateMutex.Lock();	
	CString s = lpszNewValue;
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	this->DeserializeMapStateCore(node, "", VARIANT_TRUE, NULL);
	CPLDestroyXMLNode(node);
}
#pragma endregion