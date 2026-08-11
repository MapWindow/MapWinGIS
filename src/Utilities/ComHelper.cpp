/**************************************************************************************
* File name: ComHelper.cpp
*
* Project: MapWindow Open Source (MapWinGis ActiveX control)
* Description: 
*
**************************************************************************************
* The contents of this file are subject to the Mozilla Public License Version 1.1
* (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/
* See the License for the specific language governing rights and limitations
* under the License.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
**************************************************************************************
* Contributor(s):
* (Open source contributors should list themselves and their modifications here). */
// june 2017 PaulM - Added GdalUtils

#include "stdafx.h"
#include "ComHelper.h"

// ********************************************************
//	  ComHelper::SetRef
// ********************************************************
// Sets new instance of COM object to the given pointer. Takes care to release the old reference
bool ComHelper::SetRef(IDispatch* newVal, IDispatch** oldVal, bool allowNull)
{
	if (*oldVal == nullptr)
	{
		if (newVal)
		{
			(*oldVal) = newVal;
			newVal->AddRef();
		}
	}
	else if ((*oldVal) != newVal)
	{
		if (newVal == nullptr && !allowNull)
		{
			// NULL reference isn't allowed
			return false;
		}
		else
		{
			if (*oldVal)
			{
				(*oldVal)->Release();
				(*oldVal) = nullptr;
			}
			(*oldVal) = newVal;

			if (newVal)
			{
				newVal->AddRef();
			}
		}
	}
	else
	{
		// do nothing, the reference is in place
	}
	return true;
}

// ********************************************************
//	  CreateInstance
// ********************************************************
HRESULT ComHelper::CreateInstance(tkInterface interfaceId, IDispatch** retVal)
{
	HRESULT result = S_FALSE;
	void* val = nullptr;
	switch (interfaceId)
	{
		case tkInterface::idPoint:
			ComHelper::CreatePoint(reinterpret_cast<IPoint**>(&val));
			break;
		case tkInterface::idShape:
			ComHelper::CreateShape(reinterpret_cast<IShape**>(&val));
			break;
		case tkInterface::idExtents:
			ComHelper::CreateExtents(reinterpret_cast<IExtents**>(&val));
			break;
		case tkInterface::idField:
			result = CoCreateInstance(CLSID_Field, nullptr, CLSCTX_INPROC_SERVER, IID_IField, &val);
			break;
		case tkInterface::idLabel:
			result = CoCreateInstance(CLSID_Label, nullptr, CLSCTX_INPROC_SERVER, IID_ILabel, &val);
			break;
		case tkInterface::idChart:
			result = CoCreateInstance(CLSID_Chart, nullptr, CLSCTX_INPROC_SERVER, IID_IChart, &val);
			break;
		case tkInterface::idChartField:
			result = CoCreateInstance(CLSID_ChartField, nullptr, CLSCTX_INPROC_SERVER, IID_IChartField, &val);
			break;
		case tkInterface::idCharts:
			result = CoCreateInstance(CLSID_Charts, nullptr, CLSCTX_INPROC_SERVER, IID_ICharts, &val);
			break;
		case tkInterface::idColorScheme:
			result = CoCreateInstance(CLSID_ColorScheme, nullptr, CLSCTX_INPROC_SERVER, IID_IColorScheme, &val);
			break;
		case tkInterface::idDrawingRectangle:
			result = CoCreateInstance(CLSID_DrawingRectangle, nullptr, CLSCTX_INPROC_SERVER, IID_IDrawingRectangle, &val);
			break;
		case tkInterface::idESRIGridManager:
			result = CoCreateInstance(CLSID_ESRIGridManager, nullptr, CLSCTX_INPROC_SERVER, IID_IESRIGridManager, &val);
			break;
		case tkInterface::idExpression:
			result = CoCreateInstance(CLSID_Expression, nullptr, CLSCTX_INPROC_SERVER, IID_IExpression, &val);
			break;
		case tkInterface::idFieldStatOperations:
			result = CoCreateInstance(CLSID_FieldStatOperations, nullptr, CLSCTX_INPROC_SERVER, IID_IFieldStatOperations, &val);
			break;
		case tkInterface::idFileManager:
			result = CoCreateInstance(CLSID_FileManager, nullptr, CLSCTX_INPROC_SERVER, IID_IFileManager, &val);
			break;
		case tkInterface::idFunction:
			result = CoCreateInstance(CLSID_Function, nullptr, CLSCTX_INPROC_SERVER, IID_IFunction, &val);
			break;
		case tkInterface::idGdalDataset:
			result = CoCreateInstance(CLSID_GdalDataset, nullptr, CLSCTX_INPROC_SERVER, IID_IGdalDataset, &val);
			break;
		case tkInterface::idGdalDriver:
			result = CoCreateInstance(CLSID_GdalDriver, nullptr, CLSCTX_INPROC_SERVER, IID_IGdalDriver, &val);
			break;
		case tkInterface::idGdalDriverManager:
			result = CoCreateInstance(CLSID_GdalDriverManager, nullptr, CLSCTX_INPROC_SERVER, IID_IGdalDriverManager, &val);
			break;
		case tkInterface::idGdalRasterBand:
			result = CoCreateInstance(CLSID_GdalRasterBand, nullptr, CLSCTX_INPROC_SERVER, IID_IGdalRasterBand, &val);
			break;
		case tkInterface::idGeoProjection:
			result = CoCreateInstance(CLSID_GeoProjection, nullptr, CLSCTX_INPROC_SERVER, IID_IGeoProjection, &val);
			break;
		case tkInterface::idGlobalSettings:
			result = CoCreateInstance(CLSID_GlobalSettings, nullptr, CLSCTX_INPROC_SERVER, IID_IGlobalSettings, &val);
			break;
		case tkInterface::idGrid:
			result = CoCreateInstance(CLSID_Grid, nullptr, CLSCTX_INPROC_SERVER, IID_IGrid, &val);
			break;
		case tkInterface::idGridColorBreak:
			result = CoCreateInstance(CLSID_GridColorBreak, nullptr, CLSCTX_INPROC_SERVER, IID_IGridColorBreak, &val);
			break;
		case tkInterface::idGridColorScheme:
			result = CoCreateInstance(CLSID_GridColorScheme, nullptr, CLSCTX_INPROC_SERVER, IID_IGridColorScheme, &val);
			break;
		case tkInterface::idGridHeader:
			result = CoCreateInstance(CLSID_GridHeader, nullptr, CLSCTX_INPROC_SERVER, IID_IGridHeader, &val);
			break;
		case tkInterface::idHistogram:
			result = CoCreateInstance(CLSID_Histogram, nullptr, CLSCTX_INPROC_SERVER, IID_IHistogram, &val);
			break;
		case tkInterface::idIdentifier:
			result = CoCreateInstance(CLSID_Identifier, nullptr, CLSCTX_INPROC_SERVER, IID_IIdentifier, &val);
			break;
		case tkInterface::idImage:
			result = CoCreateInstance(CLSID_Image, nullptr, CLSCTX_INPROC_SERVER, IID_IImage, &val);
			break;
		case tkInterface::idLabelCategory:
			result = CoCreateInstance(CLSID_LabelCategory, nullptr, CLSCTX_INPROC_SERVER, IID_ILabelCategory, &val);
			break;
		case tkInterface::idLabels:
			result = CoCreateInstance(CLSID_Labels, nullptr, CLSCTX_INPROC_SERVER, IID_ILabels, &val);
			break;
		case tkInterface::idLinePattern:
			result = CoCreateInstance(CLSID_LinePattern, nullptr, CLSCTX_INPROC_SERVER, IID_ILinePattern, &val);
			break;
		case tkInterface::idLineSegment:
			result = CoCreateInstance(CLSID_LineSegment, nullptr, CLSCTX_INPROC_SERVER, IID_ILineSegment, &val);
			break;
		case tkInterface::idMeasuring:
			result = CoCreateInstance(CLSID_Measuring, nullptr, CLSCTX_INPROC_SERVER, IID_IMeasuring, &val);
			break;
		case tkInterface::idOgrDatasource:
			result = CoCreateInstance(CLSID_OgrDatasource, nullptr, CLSCTX_INPROC_SERVER, IID_IOgrDatasource, &val);
			break;
		case tkInterface::idOgrLayer:
			result = CoCreateInstance(CLSID_OgrLayer, nullptr, CLSCTX_INPROC_SERVER, IID_IOgrLayer, &val);
			break;	
		case tkInterface::idSelectionList:
			result = CoCreateInstance(CLSID_SelectionList, nullptr, CLSCTX_INPROC_SERVER, IID_ISelectionList, &val);
			break;
		case tkInterface::idShapeDrawingOptions:
			result = CoCreateInstance(CLSID_ShapeDrawingOptions, nullptr, CLSCTX_INPROC_SERVER, IID_IShapeDrawingOptions, &val);
			break;
		case tkInterface::idShapeEditor:
			result = CoCreateInstance(CLSID_ShapeEditor, nullptr, CLSCTX_INPROC_SERVER, IID_IShapeEditor, &val);
			break;
		case tkInterface::idShapefile:
			result = CoCreateInstance(CLSID_Shapefile, nullptr, CLSCTX_INPROC_SERVER, IID_IShapefile, &val);
			break;
		case tkInterface::idShapefileCategories:
			result = CoCreateInstance(CLSID_ShapefileCategories, nullptr, CLSCTX_INPROC_SERVER, IID_IShapefileCategories, &val);
			break;
		case tkInterface::idShapefileCategory:
			result = CoCreateInstance(CLSID_ShapefileCategory, nullptr, CLSCTX_INPROC_SERVER, IID_IShapefileCategory, &val);
			break;
#ifdef OLD_API
		case tkInterface::idShapefileColorBreak:
			result = CoCreateInstance(CLSID_ShapefileColorBreak, nullptr, CLSCTX_INPROC_SERVER, IID_IShapefileColorBreak, &val);
			break;
		case tkInterface::idShapefileColorScheme:
			result = CoCreateInstance(CLSID_ShapefileColorScheme, nullptr, CLSCTX_INPROC_SERVER, IID_IShapefileColorScheme, &val);
			break;
#endif
		case tkInterface::idShapeNetwork:
			result = CoCreateInstance(CLSID_ShapeNetwork, nullptr, CLSCTX_INPROC_SERVER, IID_IShapeNetwork, &val);
			break;
		case tkInterface::idShapeValidationInfo:
			result = CoCreateInstance(CLSID_ShapeValidationInfo, nullptr, CLSCTX_INPROC_SERVER, IID_IShapeValidationInfo, &val);
			break;
		case tkInterface::idTable:
			result = CoCreateInstance(CLSID_Table, nullptr, CLSCTX_INPROC_SERVER, IID_ITable, &val);
			break;
		case tkInterface::idTileProviders:
			result = CoCreateInstance(CLSID_TileProviders, nullptr, CLSCTX_INPROC_SERVER, IID_ITileProviders, &val);
			break;
		case tkInterface::idTiles:
			result = CoCreateInstance(CLSID_Tiles, nullptr, CLSCTX_INPROC_SERVER, IID_ITiles, &val);
			break;
		case tkInterface::idTin:
			result = CoCreateInstance(CLSID_Tin, nullptr, CLSCTX_INPROC_SERVER, IID_ITin, &val);
			break;
		case tkInterface::idUndoList:
			result = CoCreateInstance(CLSID_UndoList, nullptr, CLSCTX_INPROC_SERVER, IID_IUndoList, &val);
			break;
		case tkInterface::idUtils:
			result = CoCreateInstance(CLSID_Utils, nullptr, CLSCTX_INPROC_SERVER, IID_IUtils, &val);
			break;
		case tkInterface::idVector:
			result = CoCreateInstance(CLSID_Vector, nullptr, CLSCTX_INPROC_SERVER, IID_IVector, &val);
			break;
		case tkInterface::idWmsLayer:
			result = CoCreateInstance(CLSID_WmsLayer, nullptr, CLSCTX_INPROC_SERVER, IID_IWmsLayer, &val);
			break;
		case tkInterface::idGdalUtils:
			result = CoCreateInstance(CLSID_GdalUtils, nullptr, CLSCTX_INPROC_SERVER, IID_IGdalUtils, &val);
			break;
		case tkInterface::idPlacedLabels:
			result = CoCreateInstance(CLSID_PlacedLabels, nullptr, CLSCTX_INPROC_SERVER, IID_IPlacedLabels, &val);
			break;
	}
	*retVal = val ? static_cast<IDispatch*>(val) : nullptr;
	return result;
}

// ********************************************************
//     GetInterfaceName()
// ********************************************************
CString ComHelper::GetInterfaceName(tkInterface id)
{
	switch (id)
	{
		case idChart: 			return "Chart";
		case idChartField:		return "ChartField";
		case idCharts:			return "Charts";
		case idColorScheme:		return "ColorScheme";
		case idESRIGridManager:	return "ESRIGridManager";
		case idExtents:			return "Extents";
		case idField:			return "Field";
		case idFieldStatOperations:			return "FieldStatOperations";
		case idFileManager:		return "FileManager";
		case idGeoProjection:	return "GeoProjection";
		case idGlobalSettings:	return "GlobalSettings";
		case idGrid:			return "Grid:";
		case idGridColorBreak:	return "GridColorBreak";
		case idGridColorScheme:	return "GridColorScheme";
		case idGridHeader:		return "GridHeader";
		case idIdentifier:      return "Identifier";
		case idImage:			return "Image";
		case idLabelCategory:	return "LabelCategory";
		case idLabel:			return "Label";
		case idLabels:			return "Labels";
		case idLinePattern:		return "LinePattern";
		case idLineSegment:		return "LineSegment";
		case idMeasuring:		return "Measuring";
		case idPoint:			return "Point";
		case idShape:			return "Shape";
		case idShapeDrawingOptions:		return "ShapeDrawingOptions";
		case idShapefile:		return "Shapefile";
		case idShapefileCategories:		return "ShapefileCategories";
		case idShapefileCategory:		return "ShapefileCategory";
		case idShapefileColorBreak:		return "ShapefileColorBreak";
		case idShapefileColorScheme:	return "ShapefileColorScheme";
		case idShapeNetwork:			return "ShapeNetwork";
		case idShapeValidationInfo:		return "ShapeValidationInfo";
		case idTable:			return "Table";
		case idTileProviders:	return "TileProviders";
		case idTiles:			return "Tiles";
		case idTin:				return "Tin";
		case idUtils:			return "Utils";
		case idVector:			return "Vector";
		case idOgrDatasource:   return "OgrDatasource";
		case idOgrLayer:		return "OgrLayer";
		case idShapeEditor:		return "ShapeEditor";
		case idUndoList:		return "UndoList";
		case idWmsLayer:		return "WmsLayer";
		case idGdalUtils:		return "GdalUtils";
		default:				return "Unknown";
	}
}

// ********************************************************
//     CreatePoint()
// ********************************************************
void ComHelper::CreatePoint(IPoint** point)
{
	if (point == nullptr)
	{
		return;
	}
	
	*point = nullptr;

	if (m_factory.IsPointFactoryAvailable())
	{
		m_factory.pointFactory->CreateInstance(nullptr, IID_IPoint, reinterpret_cast<void**>(point));
	}
	else
	{
		// Fallback to CoCreateInstance if factory is not available
		CoCreateInstance(CLSID_Point, nullptr, CLSCTX_INPROC_SERVER, IID_IPoint, reinterpret_cast<void**>(point));
	}
}

// ********************************************************
//     CreateShape()
// ********************************************************
void ComHelper::CreateShape(IShape** shp)
{
	if (shp == nullptr)
	{
		return;
	}

	*shp = nullptr;

	if (m_factory.IsShapeFactoryAvailable())
	{
		m_factory.shapeFactory->CreateInstance(nullptr, IID_IShape, reinterpret_cast<void**>(shp));
	}
	else
	{
		// Fallback to CoCreateInstance if factory is not available
		CoCreateInstance(CLSID_Shape, nullptr, CLSCTX_INPROC_SERVER, IID_IShape, reinterpret_cast<void**>(shp));
	}
}

// ********************************************************
//     CreateExtents()
// ********************************************************
void ComHelper::CreateExtents(IExtents** box)
{
	if (box == nullptr)
	{
		return;
	}

	*box = nullptr;

	if (m_factory.IsExtentsFactoryAvailable())
	{
		m_factory.extentsFactory->CreateInstance(nullptr, IID_IExtents, reinterpret_cast<void**>(box));
	}
	else
	{
		// Fallback to CoCreateInstance if factory is not available
		CoCreateInstance(CLSID_Extents, nullptr, CLSCTX_INPROC_SERVER, IID_IExtents, reinterpret_cast<void**>(box));
	}
}
