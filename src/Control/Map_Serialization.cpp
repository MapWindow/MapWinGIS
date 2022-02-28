#include "stdafx.h"
#include "Map.h"
#include "Tiles.h"
#include "MeasuringHelper.h"

// *********************************************************
//		SaveMapState()
// *********************************************************
VARIANT_BOOL CMapView::SaveMapState(LPCTSTR filename, VARIANT_BOOL RelativePaths, VARIANT_BOOL Overwrite)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	CStringW nameW = A2W(filename);		// TODO: use Unicode
	if (Utility::FileExistsW(nameW))
	{
		if (!Overwrite)
		{
			ErrorMessage(tkCANT_CREATE_FILE);
			return VARIANT_FALSE;
		}
		else
		{
			if (!Utility::RemoveFile(nameW))
			{
				ErrorMessage(tkCANT_DELETE_FILE);
				return VARIANT_FALSE;
			}
		}
	}

	CPLXMLNode* node = SerializeMapStateCore(RelativePaths, nameW);
	bool result = GdalHelper::SerializeXMLTreeToFile(node, nameW) != 0;
	CPLDestroyXMLNode(node);
	return result ? VARIANT_TRUE : VARIANT_FALSE;
}

// *********************************************************
//		LoadMapState()
// *********************************************************
VARIANT_BOOL CMapView::LoadMapState(LPCTSTR Filename, LPDISPATCH Callback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	try {
		USES_CONVERSION;
		CStringW nameW = A2W(Filename);		// TODO: use Unicode
		CPLXMLNode* root = GdalHelper::ParseXMLFile(nameW);
		if (root)
		{
			IStopExecution* cb = NULL;
			if (Callback)
			{
				Callback->QueryInterface(IID_IStopExecution, (void**)&cb);
			}

			bool result = DeserializeMapStateCore(root, nameW, VARIANT_TRUE, cb);

			if (cb)
			{
				cb->Release();
			}

			CPLDestroyXMLNode(root);

			return result ? VARIANT_TRUE : VARIANT_FALSE;
		}
	}
	catch (...) {
		CallbackHelper::ErrorMsg("Exception during LoadMapState.");
	}
	return VARIANT_FALSE;
}

// ******************************************************************
//	   DeserializeMapState()
// ******************************************************************
VARIANT_BOOL CMapView::DeserializeMapState(LPCTSTR state, VARIANT_BOOL loadLayers, LPCTSTR basePath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VARIANT_BOOL result = VARIANT_FALSE;
	CPLXMLNode* node = CPLParseXMLString(state);
	if (node)
	{
		USES_CONVERSION;
		result = DeserializeMapStateCore(node, A2W(basePath), loadLayers, NULL) ? VARIANT_TRUE : VARIANT_FALSE;		// TODO: use Unicode
		CPLDestroyXMLNode(node);
	}
	return result;
}

// ******************************************************************
//	   DeserializeMapStateCore()
// ******************************************************************
bool CMapView::DeserializeMapStateCore(CPLXMLNode* node, CStringW ProjectName, VARIANT_BOOL LoadLayers, IStopExecution* callback)
{
	if (!node)
	{
		ErrorMessage(tkINVALID_FILE);
		return false;
	}

	if (_stricmp(node->pszValue, "MapWinGIS") != 0)
	{
		// it can be MW4 project file, then MapWinGis should be the first child node
		CPLXMLNode* nodeChild = node->psChild;
		while (nodeChild)
		{
			if (_stricmp(nodeChild->pszValue, "MapWinGIS") == 0)
			{
				// we got it
				node = nodeChild;
				break;
			}
			nodeChild = nodeChild->psNext;
		}

		// check once again, if it wasn't find - abandon it
		if (_stricmp(node->pszValue, "MapWinGIS") != 0)
		{
			ErrorMessage(tkINVALID_FILE);
			return false;
		}
	}

	CPLXMLNode* nodeState = CPLGetXMLNode(node, "MapState");
	CPLXMLNode* nodeLayers = CPLGetXMLNode(node, "Layers");
	CPLXMLNode* nodeTiles = CPLGetXMLNode(node, "Tiles");
	CPLXMLNode* nodeMeasuring = CPLGetXMLNode(node, "Measuring");
	CPLXMLNode* nodeEditor = CPLGetXMLNode(node, "ShapeEditor");

	if (!nodeState || !nodeLayers)
	{
		ErrorMessage(tkINVALID_FILE);
		return false;
	}

	bool utf8 = false;
	CString s;
	s = CPLGetXMLValue(node, "FilenamesEncoding", NULL);
	utf8 = s.CompareNoCase(UTF8_ENCODING_MARKER) == 0;

	// control options
	s = CPLGetXMLValue(nodeState, "BackColor", NULL);
	m_backColor = (s != "") ? (OLE_COLOR)atoi(s.GetString()) : RGB(255, 255, 255);

	s = CPLGetXMLValue(nodeState, "ExtentPad", NULL);
	m_extentPad = (s != "") ? atoi(s) : 0.02;

	s = CPLGetXMLValue(nodeState, "ExtentHistory", NULL);
	_extentHistoryCount = (s != "") ? atoi(s) : 20;

	s = CPLGetXMLValue(nodeState, "DoubleBuffer", NULL);
	m_doubleBuffer = (s != "") ? (BOOL)atoi(s) : TRUE;

	s = CPLGetXMLValue(nodeState, "SendMouseMove", NULL);
	m_sendMouseMove = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "SendMouseDown", NULL);
	m_sendMouseDown = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "SendMouseUp", NULL);
	m_sendMouseUp = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "SendSelectBoxDrag", NULL);
	m_sendSelectBoxDrag = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "SendSelectBoxFinal", NULL);
	m_sendSelectBoxFinal = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "SendOnDrawBackBuffer", NULL);
	m_sendOnDrawBackBuffer = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "ZoomPercent", NULL);
	m_zoomPercent = (s != "") ? (float)Utility::atof_custom(s) : 0.3;

	m_key = CPLGetXMLValue(nodeState, "Key", NULL);

	s = CPLGetXMLValue(nodeState, "CursorMode", NULL);
	m_cursorMode = (s != "") ? (short)atoi(s) : cmZoomIn;

	s = CPLGetXMLValue(nodeState, "MapCursor", NULL);
	m_mapCursor = (s != "") ? (short)atoi(s) : crsrMapDefault;

	s = CPLGetXMLValue(nodeState, "ResizeBehavior", NULL);
	_mapResizeBehavior = (s != "") ? (tkResizeBehavior)atoi(s) : rbClassic;

	s = CPLGetXMLValue(nodeState, "TrapRightMouseDown", NULL);
	_doTrapRMouseDown = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "UseSeamlessPan", NULL);
	_useSeamlessPan = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "MouseWheelSpeed", NULL);
	_mouseWheelSpeed = (s != "") ? Utility::atof_custom(s) : 0.5;

	s = CPLGetXMLValue(nodeState, "ShapeDrawingMethod", NULL);
	_shapeDrawingMethod = (s != "") ? (tkShapeDrawingMethod)atoi(s) : dmNewSymbology;

	s = CPLGetXMLValue(nodeState, "UnitsOfMeasure", NULL);
	_unitsOfMeasure = (s != "") ? (tkUnitsOfMeasure)atoi(s) : umMeters;

	s = CPLGetXMLValue(nodeState, "DisableWaitCursor", NULL);
	_disableWaitCursor = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "RotationAngle", NULL);
	_rotateAngle = (s != "") ? (float)Utility::atof_custom(s) : 0;

	s = CPLGetXMLValue(nodeState, "CanUseImageGrouping", NULL);
	_canUseImageGrouping = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "ShowRedrawTime", NULL);
	_showRedrawTime = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "ShowVersionNumber", NULL);
	_showVersionNumber = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue(nodeState, "IdentifierMode", NULL);
	tkIdentifierMode mode = s != "" ? (tkIdentifierMode)atoi(s) : imAllLayers;
	_identifier->put_IdentifierMode(mode);

	s = CPLGetXMLValue(nodeState, "HotTracking", NULL);
	VARIANT_BOOL hotTracking = s != "" ? (VARIANT_BOOL)atoi(s) : VARIANT_TRUE;
	_identifier->put_HotTracking(hotTracking);

	s = CPLGetXMLValue(nodeState, "IdentifierColor", NULL);
	OLE_COLOR outlineColor = s != "" ? (OLE_COLOR)atoi(s.GetString()) : m_globalSettings.identifierColor;
	_identifier->put_OutlineColor(outlineColor);

	CComPtr<IGeoProjection> gp = NULL;
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&gp);
	s = CPLGetXMLValue(nodeState, "Projection", NULL);
	if (s.GetLength() > 0)
	{
		VARIANT_BOOL vb;
		CComBSTR bstr(s);
		gp->ImportFromAutoDetect(bstr, &vb);
	}
	SetGeoProjection(gp);

	if (LoadLayers)
	{
		// processing layers
		this->RemoveAllLayers();

		wchar_t* cwd = new wchar_t[4096];
		_wgetcwd(cwd, 4096);

		ProjectName = Utility::GetFolderFromPath(ProjectName);
		_wchdir(ProjectName);

		CPLXMLNode* nodeLayer = nodeLayers->psChild;
		while (nodeLayer)
		{
			if (_stricmp(nodeLayer->pszValue, "Layer") == 0)
			{
				int handle = DeserializeLayerCore(nodeLayer, ProjectName, utf8, callback);
			}
			nodeLayer = nodeLayer->psNext;
		}
		_wchdir(cwd);
		delete[] cwd;
	}

	// restoring tiles settings
	((CTiles*)_tiles)->DeserializeCore(nodeTiles);

	MeasuringHelper::Deserialize(GetMeasuringBase(), nodeMeasuring);

	MeasuringHelper::Deserialize(GetEditorBase(), nodeEditor);

	// extents
	Extent extents;
	s = CPLGetXMLValue(nodeState, "ExtentsLeft", NULL);
	if (s != "") extents.left = Utility::atof_custom(s);

	s = CPLGetXMLValue(nodeState, "ExtentsRight", NULL);
	if (s != "") extents.right = Utility::atof_custom(s);

	s = CPLGetXMLValue(nodeState, "ExtentsBottom", NULL);
	if (s != "") extents.bottom = Utility::atof_custom(s);

	s = CPLGetXMLValue(nodeState, "ExtentsTop", NULL);
	if (s != "") extents.top = Utility::atof_custom(s);

	s = CPLGetXMLValue(nodeState, "ExtentsPad", NULL);
	if (s != "") m_extentPad = Utility::atof_custom(s);

	this->SetExtentsCore(extents);

	return true;
}

// ************************************************************
//		SerializeMapState()
// ************************************************************
BSTR CMapView::SerializeMapState(VARIANT_BOOL relativePaths, LPCTSTR basePath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CPLXMLNode* node = SerializeMapStateCore(relativePaths, basePath);
	BSTR bstr;
	Utility::SerializeAndDestroyXmlTree(node, &bstr);
	return bstr;
}

// ************************************************************
//		SerializeMapStateCore()
// ************************************************************
CPLXMLNode* CMapView::SerializeMapStateCore(VARIANT_BOOL RelativePaths, CStringW ProjectName)
{
	CPLXMLNode* psTree = CPLCreateXMLNode(NULL, CXT_Element, "MapWinGIS");
	if (psTree)
	{
		Utility::WriteXmlHeaderAttributes(psTree, "MapState");

		// control options
		CPLXMLNode* psState = CPLCreateXMLNode(NULL, CXT_Element, "MapState");
		if (psState)
		{
			if (m_backColor != RGB(255, 255, 255))
				Utility::CPLCreateXMLAttributeAndValue(psState, "BackColor", CPLString().Printf("%d", m_backColor));

			if (m_extentPad != 0.02)
				Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentPad", CPLString().Printf("%f", m_extentPad));

			if (_extentHistoryCount != 20)
				Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentHistory", CPLString().Printf("%d", _extentHistoryCount));

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
				Utility::CPLCreateXMLAttributeAndValue(psState, "CursorMode", CPLString().Printf("%d", (int)m_cursorMode));

			if (m_mapCursor != 0)
				Utility::CPLCreateXMLAttributeAndValue(psState, "MapCursor", CPLString().Printf("%d", (int)m_mapCursor));

			if (_mapResizeBehavior != rbClassic)
				Utility::CPLCreateXMLAttributeAndValue(psState, "ResizeBehavior", CPLString().Printf("%d", (int)_mapResizeBehavior));

			if (_doTrapRMouseDown != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "TrapRightMouseDown", CPLString().Printf("%d", (int)_doTrapRMouseDown));

			if (_useSeamlessPan != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "UseSeamlessPan", CPLString().Printf("%d", (int)_useSeamlessPan));

			if (_mouseWheelSpeed != 0.5)
				Utility::CPLCreateXMLAttributeAndValue(psState, "MouseWheelSpeed", CPLString().Printf("%f", _mouseWheelSpeed));

			if (_shapeDrawingMethod != dmNewSymbology)
				Utility::CPLCreateXMLAttributeAndValue(psState, "ShapeDrawingMethod", CPLString().Printf("%d", (int)_shapeDrawingMethod));

			if (_unitsOfMeasure != umMeters)
				Utility::CPLCreateXMLAttributeAndValue(psState, "UnitsOfMeasure", CPLString().Printf("%d", (int)_unitsOfMeasure));

			if (_disableWaitCursor != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "DisableWaitCursor", CPLString().Printf("%d", (int)_disableWaitCursor));

			if (_rotateAngle != 0.0f)
				Utility::CPLCreateXMLAttributeAndValue(psState, "RotationAngle", CPLString().Printf("%f", _rotateAngle));

			if (_canUseImageGrouping != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "CanUseImageGrouping", CPLString().Printf("%d", (int)_canUseImageGrouping));

			if (_showRedrawTime != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "ShowRedrawTime", CPLString().Printf("%d", (int)_showRedrawTime));

			if (_showVersionNumber != FALSE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "ShowVersionNumber", CPLString().Printf("%d", (int)_showVersionNumber));

			tkIdentifierMode identifierMode;
			_identifier->get_IdentifierMode(&identifierMode);
			if (identifierMode != imAllLayers)
				Utility::CPLCreateXMLAttributeAndValue(psState, "IdentifierMode", CPLString().Printf("%d", (int)identifierMode));

			VARIANT_BOOL hotTracking;
			_identifier->get_HotTracking(&hotTracking);
			if (hotTracking != VARIANT_TRUE)
				Utility::CPLCreateXMLAttributeAndValue(psState, "HotTracking", CPLString().Printf("%d", (int)hotTracking));

			OLE_COLOR outlineColor;
			_identifier->get_OutlineColor(&outlineColor);
			if (outlineColor != m_globalSettings.identifierColor)
				Utility::CPLCreateXMLAttributeAndValue(psState, "IdentifierColor", CPLString().Printf("%d", outlineColor));

			IGeoProjection* gp = GetMapProjection();
			VARIANT_BOOL isEmpty;
			gp->get_IsEmpty(&isEmpty);
			if (!isEmpty)
			{
				CComBSTR proj = NULL;
				gp->ExportToProj4(&proj);
				USES_CONVERSION;
				Utility::CPLCreateXMLAttributeAndValue(psState, "Projection", OLE2A(proj));
			}

			Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentsLeft", CPLString().Printf("%f", _extents.left));
			Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentsRight", CPLString().Printf("%f", _extents.right));
			Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentsBottom", CPLString().Printf("%f", _extents.bottom));
			Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentsTop", CPLString().Printf("%f", _extents.top));
			Utility::CPLCreateXMLAttributeAndValue(psState, "ExtentsPad", CPLString().Printf("%f", m_extentPad));

			CPLAddXMLChild(psTree, psState);

			// layer options
			CPLXMLNode* psLayers = CPLCreateXMLNode(NULL, CXT_Element, "Layers");
			if (psLayers)
			{
				for (unsigned int i = 0; i < _activeLayers.size(); i++)
				{
					LONG handle = _activeLayers[i];

					if (!this->GetLayerSkipOnSaving(handle))
					{
						// getting relative name
						CComBSTR layerName;
						layerName.Attach(this->GetLayerFilename(handle));

						USES_CONVERSION;
						CStringW layerNameW = OLE2W(layerName);
						if (RelativePaths)
						{
							layerNameW = Utility::GetRelativePath(ProjectName, layerNameW);
						}
						CPLXMLNode* node = this->SerializeLayerCore(handle, layerNameW);
						if (node)
						{
							CPLAddXMLChild(psLayers, node);
						}
					}
				}
				CPLAddXMLChild(psTree, psLayers);
			}

			// adding tiles
			CPLXMLNode* nodeTiles = ((CTiles*)_tiles)->SerializeCore("Tiles");
			if (nodeTiles)
			{
				CPLAddXMLChild(psTree, nodeTiles);
			}

			// measuring
			CPLXMLNode* nodeMeasuring = MeasuringHelper::Serialize(GetMeasuringBase(), "Measuring");
			if (nodeMeasuring)
			{
				CPLAddXMLChild(psTree, nodeMeasuring);
			}

			// editor
			CPLXMLNode* nodeEditor = MeasuringHelper::Serialize(GetEditorBase(), "ShapeEditor");
			if (nodeEditor)
			{
				CPLAddXMLChild(psTree, nodeEditor);
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
void CMapView::SetMapState(LPCTSTR lpszNewValue)
{
	_mapstateMutex.Lock();
	CString s = lpszNewValue;
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	this->DeserializeMapStateCore(node, L"", VARIANT_TRUE, NULL);
	CPLDestroyXMLNode(node);
	_mapstateMutex.Unlock();
}
#pragma endregion