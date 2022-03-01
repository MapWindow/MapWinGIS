/**************************************************************************************
 * File name: Charts.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control)
 * Description: Implementation of CCharts
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
 // Sergei Leschinski (lsu) 19 june 2010 - created the file.
 // Paul Meems sept. 2019 - MWGIS-183: Merge .NET and VB drawing functions

#include "stdafx.h"
#include "Charts.h"
#include "Shapefile.h"
#include "macros.h"
#include "GeometryHelper.h"
#include "Templates.h"
#include "CollisionList.h"
#include "Chart.h"

// **********************************************************
//		get/put_Key()
// **********************************************************
STDMETHODIMP CCharts::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CCharts::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		::SysFreeString(_key);
	USES_CONVERSION;
	_key = OLE2BSTR(newVal);
	return S_OK;
}

// **********************************************************
//		get/put_VisibilityExpression()
// **********************************************************
STDMETHODIMP CCharts::get_VisibilityExpression(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = OLE2BSTR(_expression);
	return S_OK;
}
STDMETHODIMP CCharts::put_VisibilityExpression(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		::SysFreeString(_expression);
	USES_CONVERSION;
	_expression = OLE2BSTR(newVal);
	return S_OK;
}

// **********************************************************
//		get/put_Caption()
// **********************************************************
STDMETHODIMP CCharts::get_Caption(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = OLE2BSTR(_caption);
	return S_OK;
}
STDMETHODIMP CCharts::put_Caption(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		::SysFreeString(_caption);
	USES_CONVERSION;
	_caption = OLE2BSTR(newVal);
	return S_OK;
}

// **********************************************************
//		get/put_Visible()
// **********************************************************
STDMETHODIMP CCharts::get_Visible(VARIANT_BOOL *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.visible;
	return S_OK;
}
STDMETHODIMP CCharts::put_Visible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.visible = newVal;
	return S_OK;
}

// **********************************************************
//		get/put_AvoidCollisions()
// **********************************************************
STDMETHODIMP CCharts::get_AvoidCollisions(VARIANT_BOOL *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.avoidCollisions;
	return S_OK;
}
STDMETHODIMP CCharts::put_AvoidCollisions(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.avoidCollisions = newVal;
	return S_OK;
}

// **********************************************************
//		get/put_ChartType()
// **********************************************************
STDMETHODIMP CCharts::get_ChartType(tkChartType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.chartType;
	return S_OK;
}
STDMETHODIMP CCharts::put_ChartType(tkChartType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.chartType = newVal;
	return S_OK;
}

// **********************************************************
//		get/put_BarChartWidth()
// **********************************************************
STDMETHODIMP CCharts::get_BarWidth(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.barWidth;
	return S_OK;
}
STDMETHODIMP CCharts::put_BarWidth(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.barWidth = newVal;
	return S_OK;
}

// **********************************************************
//		get/put_BarChartHeight()
// **********************************************************
STDMETHODIMP CCharts::get_BarHeight(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.barHeight;
	return S_OK;
}
STDMETHODIMP CCharts::put_BarHeight(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.barHeight = newVal;
	return S_OK;
}

// **********************************************************
//		get/put_PieChartRadius()
// **********************************************************
STDMETHODIMP CCharts::get_PieRadius(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.radius;
	return S_OK;
}
STDMETHODIMP CCharts::put_PieRadius(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.radius = newVal;
	return S_OK;
}

// **********************************************************
//		get/put_PieChartRotation()
// **********************************************************
STDMETHODIMP CCharts::get_PieRotation(double* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.rotation;
	return S_OK;
}
STDMETHODIMP CCharts::put_PieRotation(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.rotation = newVal;
	return S_OK;
}

// **********************************************************
//		get_NumBars
// **********************************************************
STDMETHODIMP CCharts::get_NumFields(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _bars.size();
	return S_OK;
}

// **********************************************************
//		AddBar()
// **********************************************************
STDMETHODIMP CCharts::AddField2(long FieldIndex, OLE_COLOR Color)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		VARIANT_BOOL vbretval;
	this->InsertField2(_bars.size(), FieldIndex, Color, &vbretval);
	return S_OK;
}

STDMETHODIMP CCharts::AddField(IChartField* Field, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		if (!Field)
		{
			ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
			*retVal = VARIANT_FALSE;
		}
		else
		{
			_bars.push_back(Field);
			Field->AddRef();
		}
	*retVal = VARIANT_TRUE;
	return S_OK;
}

// **********************************************************
//		InsertField2()
// **********************************************************
STDMETHODIMP CCharts::InsertField2(long Index, long FieldIndex, OLE_COLOR Color, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		if (Index < 0 || Index >(long)_bars.size())
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			*retVal = VARIANT_FALSE;
		}
		else
		{
			IChartField* chartField = NULL;
			CoCreateInstance(CLSID_ChartField, NULL, CLSCTX_INPROC_SERVER, IID_IChartField, (void**)&chartField);
			if (chartField)
			{
				chartField->put_Index(FieldIndex);
				chartField->put_Color(Color);
			}

			if (Index == _bars.size())	_bars.push_back(chartField);
			else						_bars.insert(_bars.begin() + Index, chartField);
			*retVal = VARIANT_TRUE;
		}
	return S_OK;
}

// **********************************************************
//		InsertField()
// **********************************************************
STDMETHODIMP CCharts::InsertField(long Index, IChartField* Field, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		if (Index < 0 || Index >(long)_bars.size())
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			*retVal = VARIANT_FALSE;
		}
		else
		{
			if (!Field)
			{
				ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
				*retVal = VARIANT_FALSE;
			}
			else
			{
				_bars.insert(_bars.begin() + Index, Field);
				Field->AddRef();
				*retVal = VARIANT_TRUE;
			}
		}
	return S_OK;
}

// **********************************************************
//		RemoveBar()
// **********************************************************
STDMETHODIMP CCharts::RemoveField(long Index, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		if (Index < 0 || Index >(long)_bars.size())
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			*retVal = VARIANT_FALSE;
		}
		else
		{
			_bars[Index]->Release();
			_bars.erase(_bars.begin() + Index);
			*retVal = VARIANT_TRUE;
		}
	return S_OK;
}

// **********************************************************
//		MoveBar()
// **********************************************************
STDMETHODIMP CCharts::MoveField(long OldIndex, long NewIndex, VARIANT_BOOL* vbretval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);
	return S_OK;
}

// **********************************************************
//		SetPositions
// **********************************************************
STDMETHODIMP CCharts::Generate(tkLabelPositioning Position, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		if (!_shapefile)
		{
			ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
			*retVal = VARIANT_FALSE;
		}
		else
		{
			((CShapefile*)_shapefile)->SetChartsPositions(Position);
			_chartsExist = true;
			*retVal = VARIANT_TRUE;
		}
	return S_OK;
}

// **********************************************************
//		ErrorMessage()
// **********************************************************
inline void CCharts::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("Charts", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// *******************************************************************
//		get/put_ParentShapefile()
// *******************************************************************
// For inner use only
void CCharts::put_ParentShapefile(IShapefile* newVal)
{
	_shapefile = newVal;
}
IShapefile* CCharts::get_ParentShapefile()
{
	return _shapefile;
}

// **********************************************************
//		Thickness
// **********************************************************
STDMETHODIMP CCharts::get_Thickness(double* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.thickness;
	return S_OK;
}
STDMETHODIMP CCharts::put_Thickness(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.thickness = newVal;
	return S_OK;
}

// **********************************************************
//		Tilt
// **********************************************************
STDMETHODIMP CCharts::get_Tilt(double* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.tilt;
	return S_OK;
}
STDMETHODIMP CCharts::put_Tilt(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.tilt = newVal;
	return S_OK;
}

// **********************************************************
//		PieRadius2
// **********************************************************
STDMETHODIMP CCharts::get_PieRadius2(LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.radius2;
	return S_OK;
}
STDMETHODIMP CCharts::put_PieRadius2(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.radius2 = newVal;
	return S_OK;
}

// **********************************************************
//		SizeField
// **********************************************************
STDMETHODIMP CCharts::get_SizeField(LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.sizeField;
	return S_OK;
}
STDMETHODIMP CCharts::put_SizeField(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.sizeField = newVal;
	return S_OK;
}

// **********************************************************
//		NormalizationField
// **********************************************************
STDMETHODIMP CCharts::get_NormalizationField(LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.normalizationField;
	return S_OK;
}
STDMETHODIMP CCharts::put_NormalizationField(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.normalizationField = newVal;
	return S_OK;
}

// **********************************************************
//		UseVariableRadius
// **********************************************************
STDMETHODIMP CCharts::get_UseVariableRadius(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.useVariableRadius;
	return S_OK;
}
STDMETHODIMP CCharts::put_UseVariableRadius(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.useVariableRadius = newVal ? true : false;
	return S_OK;
}

// **********************************************************
//		Transparency
// **********************************************************
STDMETHODIMP CCharts::get_Transparency(SHORT* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = (short)_options.transparency;
	return S_OK;
}
STDMETHODIMP CCharts::put_Transparency(SHORT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		if (newVal >= 0 && newVal <= 255)
			_options.transparency = newVal;
		else
			ErrorMessage(tkINVALID_PARAMETER_VALUE);
	return S_OK;
}

// **********************************************************
//		LineColor
// **********************************************************
STDMETHODIMP CCharts::get_LineColor(OLE_COLOR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.lineColor;
	return S_OK;
}
STDMETHODIMP CCharts::put_LineColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.lineColor = newVal;
	return S_OK;
}

// **********************************************************
//		3DMode
// **********************************************************
STDMETHODIMP CCharts::get_Use3DMode(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.use3Dmode;
	return S_OK;
}
STDMETHODIMP CCharts::put_Use3DMode(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.use3Dmode = newVal ? true : false;
	return S_OK;
}

// **********************************************************
//		VerticalPosition
// **********************************************************
STDMETHODIMP CCharts::get_VerticalPosition(tkVerticalPosition* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _options.verticalPosition;
	return S_OK;
}
STDMETHODIMP CCharts::put_VerticalPosition(tkVerticalPosition newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_options.verticalPosition = newVal;
	return S_OK;
}

// **********************************************************
//		get_Chart
// **********************************************************
STDMETHODIMP CCharts::get_Chart(long ShapeIndex, IChart** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		if (!_shapefile)
		{
			ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
			*retVal = NULL;
			return S_OK;
		}
		else
		{
			std::vector<ShapeRecord*>* positions = ((CShapefile*)_shapefile)->get_ShapeVector();
			if (ShapeIndex < 0 || ShapeIndex >(long)positions->size())
			{
				ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
				*retVal = NULL;
				return S_OK;
			}
			else
			{
				IChart* chart = NULL;
				CoCreateInstance(CLSID_Chart, NULL, CLSCTX_INPROC_SERVER, IID_IChart, (void**)&chart);
				if (chart)
				{
					ShapeRecord* data = (*positions)[ShapeIndex];
					((CChart*)chart)->put_ChartData(reinterpret_cast<char*>(data->chart));
				}
				*retVal = chart;
			}
		}
	return S_OK;
}

// **********************************************************
//		get_Field
// **********************************************************
STDMETHODIMP CCharts::get_Field(long FieldIndex, IChartField** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		if (FieldIndex < 0 || FieldIndex >(long)_bars.size() - 1)
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
		else
		{
			*retVal = _bars[FieldIndex];
			(*retVal)->AddRef();
		}
	return S_OK;
}

// *************************************************************
//		get_LastErrorCode
// *************************************************************
STDMETHODIMP CCharts::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;

	return S_OK;
}

// *************************************************************
//		get_ErrorMsg
// *************************************************************
STDMETHODIMP CCharts::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

// *************************************************************
//		get/put_GlobalCallback
// *************************************************************
STDMETHODIMP CCharts::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*pVal = _globalCallback;
	if (_globalCallback)
	{
		_globalCallback->AddRef();
	}
	return S_OK;
}
STDMETHODIMP CCharts::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// *************************************************************
//		get/put_NumCharts
// *************************************************************
STDMETHODIMP CCharts::get_Count(long *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		if (!_shapefile)
		{
			ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
			*retVal = 0;
		}
		else
		{
			if (_chartsExist)
			{
				long numShapes;
				_shapefile->get_NumShapes(&numShapes);
				*retVal = numShapes;
			}
			else
			{
				*retVal = 0;
			}
		}
	return S_OK;
}

// *************************************************************
//		ClearFields()
// *************************************************************
STDMETHODIMP CCharts::ClearFields()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		for (unsigned int i = 0; i < _bars.size(); i++)
		{
			_bars[i]->Release();
		}
	_bars.clear();
	return S_OK;
}

// *************************************************************
//		Clear()
// *************************************************************
STDMETHODIMP CCharts::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		IShapefile* sf = this->get_ParentShapefile();
	if (sf)
	{
		std::vector<ShapeRecord*>* data = ((CShapefile*)sf)->get_ShapeVector();
		for (unsigned int i = 0; i < data->size(); i++)
		{
			if ((*data)[i]->chart != NULL)
			{
				delete (*data)[i]->chart;
				(*data)[i]->chart = NULL;
			}
		}
	}
	_chartsExist = false;
	return S_OK;
}



// **************************************************************
//		DrawChart()
// **************************************************************
STDMETHODIMP CCharts::DrawChart(int hdc, float x, float y, VARIANT_BOOL hideLabels, OLE_COLOR backColor, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		if (!hdc)
		{
			ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
			return S_OK;
		}

	CDC* dc = CDC::FromHandle((HDC)hdc);
	*retVal = DrawChartCore(dc, x, y, hideLabels, backColor);
	return S_OK;
}

// **************************************************************
//		DrawChartVB()
// **************************************************************
//STDMETHODIMP CCharts::DrawChartVB(int hdc, float x, float y, VARIANT_BOOL hideLabels, OLE_COLOR backColor, VARIANT_BOOL* retVal)
//{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState())
//	CDC* dc = CDC::FromHandle((HDC)hdc);
//	*retVal = DrawChartCore(dc, x, y, hideLabels, backColor);
//	return S_OK;
//}

// **************************************************************
//		DrawChartCore()
// **************************************************************
// Performs drawing by external calls
VARIANT_BOOL CCharts::DrawChartCore(CDC* dc, float x, float y, VARIANT_BOOL hideLabels, OLE_COLOR backColor)
{
	if (!dc)
	{
		ErrorMessage(tkFAILED_TO_OBTAIN_DC);
		return VARIANT_FALSE;
	}

	long numBars;
	this->get_NumFields(&numBars);
	bool noFields = false;
	if (numBars == 0)
	{
		numBars = 1;
		noFields = true;
	}

	// initializing graphics
	Gdiplus::Graphics g(dc->GetSafeHdc());

	long alpha = _options.transparency << 24;
	Gdiplus::Pen pen(Utility::OleColor2GdiPlus(_options.lineColor, (BYTE)_options.transparency));

	Gdiplus::Color clr = Utility::OleColor2GdiPlus(backColor, 255);
	Gdiplus::SolidBrush brushBackground(clr);
	Gdiplus::Pen penBackground(clr);

	CFont* oldFont = NULL;
	CFont fnt;
	CBrush brushFrame(_options.valuesFrameColor);

	// values font initialization
	bool vertical = (_options.valuesStyle == vsVertical);
	CString sFormat = "%g";	// format for numbers

	if (_options.valuesVisible && !hideLabels)
	{
		LOGFONT lf;
		CString s(_options.valuesFontName);

		fnt.CreatePointFont(_options.valuesFontSize * 10, s);
		fnt.GetLogFont(&lf);

		if (vertical && _options.chartType != chtPieChart)
		{
			lf.lfEscapement = 900;
		}

		dc->SetTextColor(_options.valuesFontColor);

		lf.lfItalic = (BYTE)_options.valuesFontItalic;

		if (_options.valuesFontBold)
			lf.lfWeight = FW_BOLD;
		else
			lf.lfWeight = 0;

		fnt.DeleteObject();
		fnt.CreateFontIndirectA(&lf);
		oldFont = dc->SelectObject(&fnt);

		dc->SetBkMode(TRANSPARENT);
	}

	// textOut with vertical labels works incorrectly otherwise
	dc->SetGraphicsMode(GM_COMPATIBLE);

	// drawing pie charts
	if (_options.chartType == chtPieChart)
	{
		Gdiplus::REAL pieThickness = (Gdiplus::REAL)(_options.thickness *_options.tilt / 90.0);

		Gdiplus::REAL pieHeight;
		if (_options.use3Dmode)
			pieHeight = (Gdiplus::REAL)(_options.radius * (1.0 - _options.tilt / 90.0) * 2.0);
		else
			pieHeight = (Gdiplus::REAL)(_options.radius * 2.0);

		Gdiplus::REAL pieWidth = (Gdiplus::REAL)(_options.radius * 2.0);

		double sum = 0.0;
		std::vector<double> values;

		for (int j = 0; j < numBars; j++)
		{
			values.push_back(100.0 / (double)numBars);
			sum += values[j];
		}

		Gdiplus::REAL xStart = (Gdiplus::REAL)x; //0
		Gdiplus::REAL yStart = (Gdiplus::REAL)y; //0

		Gdiplus::REAL startAngle = 0.0, sweepAngle = 0.0;
		Gdiplus::GraphicsPath path;

		for (int j = 0; j < numBars; j++)
		{
			// retrieving color
			OLE_COLOR color;
			if (noFields)
			{
				color = RGB(150, 150, 150);
			}
			else
			{
				CComPtr<IChartField> fld = NULL;
				this->get_Field(j, &fld);
				fld->get_Color(&color);
			}

			// initializing brushes
			Gdiplus::Color clr(alpha | BGR_TO_RGB(color));
			Gdiplus::Color clrDimmed = Utility::ChangeBrightness(color, -100, alpha);
			Gdiplus::SolidBrush brush(clr);
			Gdiplus::SolidBrush brushDimmed(clrDimmed);

			sweepAngle = (Gdiplus::REAL)(values[j] / sum * 360.0);
			g.FillPie(&brushBackground, xStart, yStart, pieWidth, pieHeight, startAngle, sweepAngle);
			g.FillPie(&brush, xStart, yStart, pieWidth, pieHeight, startAngle, sweepAngle);
			path.AddPie(xStart, yStart, pieWidth, pieHeight, startAngle, sweepAngle);

			// 3D mode
			if (startAngle < 180.0 &&_options.use3Dmode)
			{
				Gdiplus::GraphicsPath pathBottom;
				if (startAngle + sweepAngle > 180.0)
					pathBottom.AddArc(xStart, yStart + pieThickness, pieWidth, pieHeight, startAngle, 180 - startAngle);
				else
					pathBottom.AddArc(xStart, yStart + pieThickness, pieWidth, pieHeight, startAngle, sweepAngle);

				Gdiplus::PathData pathData;
				pathBottom.GetPathData(&pathData);
				Gdiplus::PointF* pntStart, *pntEnd;
				if (pathData.Count > 0)
				{
					pntStart = &(pathData.Points[0]);
					pntEnd = &(pathData.Points[pathData.Count - 1]);
				}
				pathBottom.AddLine(pntEnd->X, pntEnd->Y - pieThickness, pntEnd->X, pntEnd->Y);

				if (startAngle + sweepAngle > 180.0)
				{
					pathBottom.AddArc(xStart, yStart, pieWidth, pieHeight, 180.0, -(180 - startAngle));
				}
				else
					pathBottom.AddArc(xStart, yStart, pieWidth, pieHeight, startAngle + sweepAngle, -sweepAngle);

				pathBottom.AddLine(pntStart->X, pntStart->Y - (Gdiplus::REAL)pieThickness, pntStart->X, pntStart->Y);

				g.FillPath(&brushBackground, &pathBottom);
				g.FillPath(&brushDimmed, &pathBottom);
				g.DrawPath(&penBackground, &pathBottom);
				g.DrawPath(&pen, &pathBottom);
			}
			startAngle += sweepAngle;
		}
		g.DrawPath(&penBackground, &path);
		g.DrawPath(&pen, &path);

		// drawing values
		if (_options.valuesVisible && !hideLabels)
		{
			CCollisionList collisionList;
			startAngle = 0.0;
			xStart += pieWidth / 2.0f;
			yStart += pieHeight / 2.0f;

			for (int j = 0; j < numBars; j++)
			{
				sweepAngle = (Gdiplus::REAL)(values[j] / sum * 360.0);

				// label drawing
				ValueRectangle value;
				CRect* rect = &value.rect;

				value.string = Utility::FormatNumber(values[j], CString("%g"));
				dc->DrawText(value.string, rect, DT_CALCRECT);	// add alignment

				Gdiplus::REAL labelAngle = startAngle + sweepAngle / 2.0f + 90.0f;
				if (labelAngle > 360.0f)
				{
					labelAngle = labelAngle - 360.0f;
				}

				int x = (int)(xStart + sin(labelAngle / 180.0 * pi_) * _options.radius);
				int y = (int)(yStart - cos(labelAngle / 180.0 * pi_) * _options.radius);

				if (labelAngle >= 0.0 && labelAngle <= 180.0)
				{
					x += rect->Width() / 2;
				}
				else
				{
					x -= rect->Width() / 2;
				}

				if (labelAngle >= 90.0 && labelAngle <= 270.0)
				{
					y += rect->Height() / 2;
				}
				else
				{
					y -= rect->Height() / 2;
				}

				startAngle += sweepAngle;

				rect->MoveToX(x - rect->Width() / 2);
				rect->MoveToY(y - rect->Height() / 2);

				if (collisionList.HaveCollision(*rect))
				{
					continue;
				}
				else
				{
					// saving the rect and text; we shall draw it when make sure that all the values don't have collisions
					CRect* rect = &value.rect;
					collisionList.AddRectangle(rect, 0, 0);

					// drawing frame							
					if (_options.valuesFrameVisible)
					{
						CBrush* oldBrush = dc->SelectObject(&brushFrame);

						CRect r(rect->left - 3, rect->top, rect->right + 2, rect->bottom);
						dc->Rectangle(r);
						dc->SelectObject(oldBrush);
					}

					dc->DrawText(value.string, rect, DT_CENTER | DT_VCENTER);
				}
			}
		}
	}

	// -------------------------------------------------------------
	//	Bar charts drawing
	// -------------------------------------------------------------
	else if (_options.chartType == chtBarChart)
	{
		Gdiplus::PointF points[5];

		std::vector<double> values;

		for (int j = 1; j <= numBars; j++)
		{
			values.push_back(j);
		}

		double minValue = 1;
		double maxValue = numBars;

		int barHeight = _options.barHeight; // > 30 ? 30 : _options.barHeight;
		double maxHeight = (double)barHeight;

		int xStart = (int)x;
		int yStart = (int)y + (_options.use3Dmode ? (int)(_options.thickness  * _options.tilt / 90.0 + 2) : 0);

		double angle = 45.0;

		for (int j = 0; j < numBars; j++)
		{
			// retrieving color
			OLE_COLOR color;
			if (noFields)
			{
				color = RGB(150, 150, 0);
			}
			else
			{
				IChartField* fld = NULL;
				this->get_Field(j, &fld);
				fld->get_Color(&color);
				fld->Release();
				fld = NULL;
			}

			// initializing brushes
			Gdiplus::Color clr(alpha | BGR_TO_RGB(color));
			Gdiplus::Color clrDimmed = Utility::ChangeBrightness(color, -100, alpha);
			Gdiplus::SolidBrush brush(clr);
			Gdiplus::SolidBrush brushDimmed(clrDimmed);

			int height = int((double)barHeight / maxValue * values[j]);
			int offsetY = barHeight - height + yStart;

			if (height != 0)
			{
				g.FillRectangle(&brushBackground, xStart, offsetY, _options.barWidth, height);
				g.FillRectangle(&brush, xStart, offsetY, _options.barWidth, height);

				g.DrawRectangle(&penBackground, xStart, offsetY, _options.barWidth, height);
				g.DrawRectangle(&pen, xStart, offsetY, _options.barWidth, height);

				// 3D mode
				if (_options.use3Dmode)
				{
					points[0].X = (Gdiplus::REAL)xStart;
					points[1].X = (Gdiplus::REAL)(xStart + sin(angle / 180 * pi_) * _options.thickness);
					points[2].X = (Gdiplus::REAL)points[1].X + _options.barWidth;
					points[3].X = (Gdiplus::REAL)xStart + _options.barWidth;
					points[4].X = (Gdiplus::REAL)xStart;

					points[0].Y = (Gdiplus::REAL)offsetY;
					points[1].Y = (Gdiplus::REAL)(offsetY - cos(angle / 180 * pi_) * _options.thickness);
					points[2].Y = (Gdiplus::REAL)points[1].Y;
					points[3].Y = (Gdiplus::REAL)offsetY;
					points[4].Y = (Gdiplus::REAL)offsetY;

					g.DrawPolygon(&penBackground, points, 5);
					g.DrawPolygon(&pen, points, 5);
					g.FillPolygon(&brushBackground, points, 5);
					g.FillPolygon(&brushDimmed, points, 5);

					points[0].X = (Gdiplus::REAL)xStart + _options.barWidth;
					points[1].X = (Gdiplus::REAL)(points[0].X + sin(angle / 180 * pi_) * _options.thickness);
					points[2].X = (Gdiplus::REAL)points[1].X;
					points[3].X = (Gdiplus::REAL)points[0].X;
					points[4].X = (Gdiplus::REAL)points[0].X;

					points[0].Y = (Gdiplus::REAL)offsetY;
					points[1].Y = (Gdiplus::REAL)(points[0].Y - cos(angle / 180 * pi_) * _options.thickness);
					points[2].Y = (Gdiplus::REAL)points[1].Y + height;
					points[3].Y = (Gdiplus::REAL)points[0].Y + height;
					points[4].Y = (Gdiplus::REAL)points[0].Y;

					g.DrawPolygon(&penBackground, points, 5);
					g.DrawPolygon(&pen, points, 5);
					g.FillPolygon(&brushBackground, points, 5);
					g.FillPolygon(&brushDimmed, points, 5);
				}
			}
			xStart += _options.barWidth;
		}

		// drawing the labels
		std::vector<ValueRectangle> labels;
		if (_options.valuesVisible && !hideLabels)
		{
			int xAdd = (int)(sin(45.0 / 180 * pi_) * _options.thickness);

			xStart = int(x); //- numBars * _options.barWidth/2.0);
			yStart = int(y + maxHeight);
			int x, y;

			// calculating position of drawing
			for (int j = 0; j < numBars; j++)
			{
				int height = int((double)barHeight / maxValue * values[j]);
				if (height != 0)
				{
					CString s = Utility::FormatNumber(values[j], sFormat);

					CRect* rect = new CRect();
					dc->DrawText(s, rect, DT_CALCRECT);

					if (!vertical)
					{
						rect->MoveToY(yStart - rect->Height() - height - 3);

						if (j == 0)
						{
							rect->MoveToX(xStart + _options.barWidth / 2 - rect->Width());
						}
						else if (j == numBars - 1)
						{
							rect->MoveToX(xStart + _options.barWidth / 2);
						}
						else
						{
							rect->MoveToX(xStart + _options.barWidth / 2 - rect->Width() / 2);
						}
					}
					else
					{
						int wd = rect->Width();
						int ht = rect->Height();

						rect->bottom = 0;
						rect->left = 0;
						rect->right = ht;
						rect->top = -wd;

						x = xStart + _options.barWidth / 2 - rect->Width() / 2 + xAdd / 2;
						y = yStart - rect->Height() - height - 6;

						rect->MoveToXY(x, y);
					}

					// we shall store the label, to keep the collision list clean
					ValueRectangle value;
					value.string = s;

					// drawing frame							
					if (!vertical)
					{
						CRect r(rect->left - 2, rect->top, rect->right + 2, rect->bottom);
						value.rect = r;
					}
					else
					{
						CRect r(rect->left, rect->top - 2, rect->right, rect->bottom + 2);
						value.rect = r;
					}
					labels.push_back(value);

				}
				xStart += _options.barWidth;
			}	// numBars


			// the drawing; actually we could draw at once without storing rectangles to the structure
			// but this way we have the code more compatible with main drawing routine; so it's easier to maintain
			for (unsigned int j = 0; j < labels.size(); j++)
			{
				CRect* rect = &labels[j].rect;
				if (_options.valuesFrameVisible)
				{
					CBrush* oldBrush = dc->SelectObject(&brushFrame);
					dc->Rectangle(rect);
					dc->SelectObject(oldBrush);
				}

				if (!vertical)
				{
					dc->DrawText(labels[j].string, rect, DT_CENTER | DT_VCENTER);
				}
				else
				{
					dc->TextOutA(rect->left, rect->bottom - 3, labels[j].string);
				}
			}
		} // valuesVisible
	}

	if (_options.valuesVisible)
	{
		dc->SelectObject(oldFont);
		fnt.DeleteObject();
	}
	brushFrame.DeleteObject();
	return VARIANT_TRUE;
}

// **************************************************************
//			get_IconWidth()
// **************************************************************
STDMETHODIMP CCharts::get_IconWidth(long *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		if (_options.chartType == chtBarChart)
		{
			int barCount = _bars.size() == 0 ? 1 : _bars.size();
			*retVal = _options.barWidth * barCount + 2;
			if (_options.use3Dmode)
				*retVal += int(sqrt(2.0f) / 2.0 * _options.thickness);		// 45 degrees
		}
		else
		{
			*retVal = _options.radius * 2 + 2;
		}
	return S_OK;
}

// **************************************************************
//			get_IconWidth()
// **************************************************************
STDMETHODIMP CCharts::get_IconHeight(long *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		if (_options.chartType == chtBarChart)
		{
			*retVal = _options.barHeight + 2;
			if (_options.use3Dmode)
				*retVal += (int)(sqrt(2.0f) / 2.0 * _options.thickness);
		}
		else
		{
			if (_options.use3Dmode)
				*retVal = (long)(_options.radius * (1.0 - _options.tilt / 90.0) * 2.0 + _options.thickness  * _options.tilt / 90.0 + 2);
			else
				*retVal = (long)(_options.radius * 2.0 + 2);
		}
	return S_OK;
}

// *****************************************************************
//		FontName()
// *****************************************************************
STDMETHODIMP CCharts::get_ValuesFontName(BSTR* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*retval = OLE2BSTR(_options.valuesFontName);
	return S_OK;
};
STDMETHODIMP CCharts::put_ValuesFontName(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	::SysFreeString(_options.valuesFontName);
	_options.valuesFontName = OLE2BSTR(newVal);
	return S_OK;
};

// *****************************************************************
//		Select()
// *****************************************************************
// Selection of labels which fall in the given bounding box
STDMETHODIMP CCharts::Select(IExtents* BoundingBox, long Tolerance, SelectMode SelectMode, VARIANT* Indices, VARIANT_BOOL* retval)
{
	*retval = VARIANT_FALSE;
	if (!BoundingBox) return S_OK;

	if (!_chartsExist || !_shapefile)
	{
		return S_OK;
	}

	double xMin, yMin, zMin, xMax, yMax, zMax;
	BoundingBox->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
	CRect box(int(xMin - Tolerance / 2), int(yMin - Tolerance / 2), int(xMax + Tolerance / 2), int(yMax + Tolerance / 2));

	vector<long> results;

	IUtils* utils = NULL;
	CoCreateInstance(CLSID_Utils, NULL, CLSCTX_INPROC_SERVER, IID_IUtils, (void**)&utils);

	long numShapes;
	_shapefile->get_NumShapes(&numShapes);

	std::vector<ShapeRecord*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();

	for (long i = 0; i < numShapes; i++)
	{
		if ((*data)[i]->chart->isDrawn && (*data)[i]->chart->frame != NULL)
		{
			CRect* frame = (*data)[i]->chart->frame;
			tkExtentsRelation relation = GeometryHelper::RelateExtents(box, *frame);
			if ((SelectMode == INTERSECTION) && (relation != tkExtentsRelation::erNone))
			{
				results.push_back(i);
			}
			else if ((SelectMode == INCLUSION) && ((relation == tkExtentsRelation::erInclude) || (relation == tkExtentsRelation::erEqual)))
			{
				results.push_back(i);
			}
		}

	}

	utils->Release();

	(*retval) = Templates::Vector2SafeArray(&results, VT_I4, Indices);
	return S_OK;
};

#pragma region "Serialiation"

// ********************************************************
//     Serialize()
// ********************************************************
STDMETHODIMP CCharts::Serialize(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		CPLXMLNode* psTree = SerializeCore("ChartsClass");
	Utility::SerializeAndDestroyXmlTree(psTree, retVal);
	return S_OK;
}

// ********************************************************
//     Serialize()
// ********************************************************
CPLXMLNode* CCharts::SerializeCore(CString ElementName)
{
	USES_CONVERSION;

	CPLXMLNode* psTree = CPLCreateXMLNode(NULL, CXT_Element, "ChartsClass");
	CString str;

	// fields
	if (_bars.size() > 0)
	{
		CPLXMLNode* psFields = CPLCreateXMLNode(psTree, CXT_Element, "ChartFields");
		if (psFields)
		{
			for (unsigned int i = 0; i < _bars.size(); i++)
			{
				CPLXMLNode* psNode = CPLCreateXMLNode(psFields, CXT_Element, "ChartFieldClass");

				// name				
				CComBSTR name;
				_bars[i]->get_Name(&name);
				str = OLE2CA(name);
				Utility::CPLCreateXMLAttributeAndValue(psNode, "Name", str);

				// color
				OLE_COLOR color;
				_bars[i]->get_Color(&color);
				Utility::CPLCreateXMLAttributeAndValue(psNode, "Color", CPLString().Printf("%d", color));

				// index
				long index;
				_bars[i]->get_Index(&index);
				Utility::CPLCreateXMLAttributeAndValue(psNode, "Index", CPLString().Printf("%d", index));
			}
		}
	}

	// serializing data
	if (_chartsExist)
	{
		if (_savingMode == modeStandard)
		{
			CPLXMLNode* nodeCharts = this->SerializeChartData("Charts");
			if (nodeCharts)
			{
				CPLAddXMLChild(psTree, nodeCharts);
			}
		}
		else if (_savingMode == modeXML || _savingMode == modeXMLOverwrite)
		{
			if (_shapefile)
			{
				tkShapefileSourceType sourceType;
				_shapefile->get_SourceType(&sourceType);
				if (sourceType == sstDiskBased)
				{
					// constructing the name of .lbl file
					CComBSTR name;
					_shapefile->get_Filename(&name);
					CString path = Utility::GetPathWOExtension(OLE2CA(name));
					path += ".chart";

					if (Utility::FileExists(path) && _savingMode == modeXMLOverwrite)
					{
						if (remove(path) != 0)
						{
							ErrorMessage(tkCANT_DELETE_FILE);
						}
					}

					// serialize only if there is no file, it's a responsibility 
					// of user to keep this file updated
					if (!Utility::FileExists(path))
					{
						VARIANT_BOOL retVal;
						CComBSTR bstrPath(path);
						this->SaveToXML(bstrPath, &retVal);

						// user will need to save modeXMLOverwrite once more to overwrite the file
						if (_savingMode == modeXMLOverwrite)
							_savingMode = modeXML;
					}
				}
			}
		}
		else if (_savingMode == modeDBF)
		{
			// TODO: implement
			ErrorMessage(tkMETHOD_NOT_IMPLEMENTED);
		}
	}

	ChartOptions opt;	// to compare with default settings

	if (_options.avoidCollisions != opt.avoidCollisions)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AvoidCollisions", CPLString().Printf("%d", (int)_options.avoidCollisions));

	if (_options.barHeight != opt.barHeight)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "BarHeight", CPLString().Printf("%d", _options.barHeight));

	if (_options.barWidth != opt.barWidth)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "BarWidth", CPLString().Printf("%d", _options.barWidth));

	if (_options.chartType != opt.chartType)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ChartType", CPLString().Printf("%d", (int)_options.chartType));

	if (_options.lineColor != opt.lineColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LineColor", CPLString().Printf("%d", _options.lineColor));

	if (_options.normalizationField != opt.normalizationField)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "NormalizationField", CPLString().Printf("%d", (int)_options.normalizationField));

	if (_options.normalizationField != opt.normalizationField)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Radius", CPLString().Printf("%d", (int)_options.radius));

	if (_options.radius2 != opt.radius2)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Radius2", CPLString().Printf("%d", (int)_options.radius2));

	if (_options.rotation != opt.rotation)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Rotation", CPLString().Printf("%f", _options.rotation));

	if (_options.sizeField != opt.sizeField)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "SizeField", CPLString().Printf("%d", _options.sizeField));

	if (_options.thickness != opt.thickness)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Thickness", CPLString().Printf("%f", _options.thickness));

	if (_options.tilt != opt.tilt)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Tilt", CPLString().Printf("%f", _options.tilt));

	if (_options.transparency != opt.transparency)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Transparency", CPLString().Printf("%d", _options.transparency));

	if (_options.use3Dmode != opt.use3Dmode)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Use3Dmode", CPLString().Printf("%d", (int)_options.use3Dmode));

	if (_options.useVariableRadius != opt.useVariableRadius)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseVariableRadius", CPLString().Printf("%d", (int)_options.useVariableRadius));

	if (_options.valuesFontBold != opt.valuesFontBold)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ValuesFontBold", CPLString().Printf("%d", (int)_options.valuesFontBold));

	if (_options.valuesFontColor != opt.valuesFontColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ValuesFontColor", CPLString().Printf("%d", _options.valuesFontColor));

	if (_options.valuesFontItalic != opt.valuesFontItalic)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ValuesFontItalic", CPLString().Printf("%d", (int)_options.valuesFontItalic));

	str = OLE2A(_options.valuesFontName);
	if (str != OLE2A(opt.valuesFontName))
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ValuesFontName", str);

	if (_options.valuesFontSize != opt.valuesFontSize)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ValuesFontSize", CPLString().Printf("%d", _options.valuesFontSize));

	if (_options.valuesFrameColor != opt.valuesFrameColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ValuesFrameColor", CPLString().Printf("%d", _options.valuesFrameColor));

	if (_options.valuesFrameVisible != opt.valuesFrameVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ValuesFrameVisible", CPLString().Printf("%d", (int)_options.valuesFrameVisible));

	if (_options.valuesStyle != opt.valuesStyle)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ValuesStyle", CPLString().Printf("%d", (int)_options.valuesStyle));

	if (_options.valuesVisible != opt.valuesVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ValuesVisible", CPLString().Printf("%d", (int)_options.valuesVisible));

	if (_options.verticalPosition != opt.verticalPosition)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "VerticalPosition", CPLString().Printf("%d", (int)_options.verticalPosition));

	if (_options.visible != opt.visible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Visible", CPLString().Printf("%d", (int)_options.visible));

	if (_savingMode != modeXML)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "SavingMode", CPLString().Printf("%d", (int)_savingMode));

	return psTree;
}

// *********************************************************
//		DeserializeCore()
// *********************************************************
bool CCharts::DeserializeCore(CPLXMLNode* node)
{
	if (!node)
		return false;

	// restoring fields
	this->ClearFields();

	// we don't touch charts in this mode
	if (_savingMode != modeNone)
	{
		this->Clear();
	}

	CString s;

	CPLXMLNode* nodeFields = CPLGetXMLNode(node, "ChartFields");
	if (nodeFields)
	{
		CPLXMLNode* node = nodeFields->psChild;
		while (node)
		{
			if (strcmp(node->pszValue, "ChartFieldClass") == 0)
			{
				IChartField* field = NULL;
				CoCreateInstance(CLSID_ChartField, NULL, CLSCTX_INPROC_SERVER, IID_IChartField, (void**)&field);

				// name
				s = CPLGetXMLValue(node, "Name", NULL);
				CComBSTR vbstr(s);
				field->put_Name(vbstr);

				s = CPLGetXMLValue(node, "Color", NULL);
				OLE_COLOR color = atoi(s);
				field->put_Color(color);

				s = CPLGetXMLValue(node, "Index", NULL);
				long index = atoi(s);
				field->put_Index(index);

				VARIANT_BOOL vbretval;
				this->AddField(field, &vbretval);

				field->Release();
			}
			node = node->psNext;
		}
	}

	// restoring labels
	if (_savingMode == modeStandard)
	{
		node = CPLGetXMLNode(node, "Charts");
		if (node)
		{
			this->DeserializeChartData(node);
		}
	}
	else if (_savingMode == modeXML || _savingMode == modeXMLOverwrite)
	{
		if (_shapefile)
		{
			tkShapefileSourceType sourceType;
			_shapefile->get_SourceType(&sourceType);

			if (sourceType == sstDiskBased)
			{
				// constructing the name of .lbl file
				CComBSTR name;
				_shapefile->get_Filename(&name);
				USES_CONVERSION;
				CString path = Utility::GetPathWOExtension(OLE2CA(name));
				path += ".chart";

				// restoring labels
				if (Utility::FileExists(path))
				{
					VARIANT_BOOL retVal;
					CComBSTR bstrPath(path);
					this->LoadFromXML(bstrPath, &retVal);
				}
			}
		}
	}

	ChartOptions opt;	// for default set of options

	// start labels specific options
	s = CPLGetXMLValue(node, "AvoidCollisions", NULL);
	_options.avoidCollisions = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : opt.avoidCollisions;

	s = CPLGetXMLValue(node, "BarHeight", NULL);
	_options.barHeight = (s != "") ? atoi(s.GetString()) : opt.barHeight;

	s = CPLGetXMLValue(node, "BarWidth", NULL);
	_options.barWidth = (s != "") ? atoi(s.GetString()) : opt.barWidth;

	s = CPLGetXMLValue(node, "ChartType", NULL);
	_options.chartType = (s != "") ? (tkChartType)atoi(s.GetString()) : opt.chartType;

	s = CPLGetXMLValue(node, "LineColor", NULL);
	_options.lineColor = (s != "") ? (OLE_COLOR)atoi(s.GetString()) : opt.lineColor;

	s = CPLGetXMLValue(node, "NormalizationField", NULL);
	_options.normalizationField = (s != "") ? atoi(s.GetString()) : opt.normalizationField;

	s = CPLGetXMLValue(node, "Radius", NULL);
	_options.radius = (s != "") ? atoi(s.GetString()) : opt.radius;

	s = CPLGetXMLValue(node, "Radius2", NULL);
	_options.radius2 = (s != "") ? atoi(s.GetString()) : opt.radius2;

	s = CPLGetXMLValue(node, "Rotation", NULL);
	_options.rotation = (s != "") ? atoi(s.GetString()) : opt.rotation;

	s = CPLGetXMLValue(node, "SizeField", NULL);
	_options.sizeField = (s != "") ? atoi(s.GetString()) : opt.sizeField;

	s = CPLGetXMLValue(node, "Thickness", NULL);
	_options.thickness = (s != "") ? Utility::atof_custom(s) : opt.thickness;

	s = CPLGetXMLValue(node, "Tilt", NULL);
	_options.tilt = (s != "") ? Utility::atof_custom(s) : opt.tilt;

	s = CPLGetXMLValue(node, "Transparency", NULL);
	_options.transparency = (s != "") ? atoi(s) : opt.transparency;

	s = CPLGetXMLValue(node, "Use3Dmode", NULL);
	_options.use3Dmode = (s != "") ? (atoi(s) == 0 ? false : true) : opt.use3Dmode;

	s = CPLGetXMLValue(node, "UseVariableRadius", NULL);
	_options.useVariableRadius = (s != "") ? (atoi(s.GetString()) == 0 ? false : true) : opt.useVariableRadius;

	s = CPLGetXMLValue(node, "ValuesFontBold", NULL);
	_options.valuesFontBold = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : opt.valuesFontBold;

	s = CPLGetXMLValue(node, "ValuesFontColor", NULL);
	_options.valuesFontColor = (s != "") ? (OLE_COLOR)atoi(s.GetString()) : opt.valuesFontColor;

	s = CPLGetXMLValue(node, "ValuesFontItalic", NULL);
	_options.valuesFontItalic = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : opt.valuesFontItalic;

	s = CPLGetXMLValue(node, "ValuesFontName", NULL);
	if (s != "")
	{
		SysFreeString(_options.valuesFontName);
		_options.valuesFontName = A2BSTR(s);
	}

	s = CPLGetXMLValue(node, "ValuesFontSize", NULL);
	_options.valuesFontSize = (s != "") ? atoi(s.GetString()) : opt.valuesFontSize;

	s = CPLGetXMLValue(node, "ValuesFrameColor", NULL);
	_options.valuesFrameColor = (s != "") ? (OLE_COLOR)atoi(s.GetString()) : opt.valuesFrameColor;

	s = CPLGetXMLValue(node, "ValuesFrameVisible", NULL);
	_options.valuesFrameVisible = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : opt.valuesFrameVisible;

	s = CPLGetXMLValue(node, "ValuesStyle", NULL);
	_options.valuesStyle = (s != "") ? (tkChartValuesStyle)atoi(s.GetString()) : opt.valuesStyle;

	s = CPLGetXMLValue(node, "ValuesVisible", NULL);
	_options.valuesVisible = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : opt.valuesVisible;

	s = CPLGetXMLValue(node, "VerticalPosition", NULL);
	_options.verticalPosition = (s != "") ? (tkVerticalPosition)atoi(s.GetString()) : opt.verticalPosition;

	s = CPLGetXMLValue(node, "Visible", NULL);
	_options.visible = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : opt.visible;

	s = CPLGetXMLValue(node, "SavingMode", NULL);
	_savingMode = (s != "") ? (tkSavingMode)atoi(s.GetString()) : modeXML;

	return true;
}

// ********************************************************
//     Deserialize()
// ********************************************************
STDMETHODIMP CCharts::Deserialize(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	CString s = OLE2CA(newVal);
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		CPLXMLNode* nodeCharts = CPLGetXMLNode(node, "=ChartsClass");
		if (nodeCharts)
		{
			this->DeserializeCore(nodeCharts);
		}
		CPLDestroyXMLNode(node);
	}
	return S_OK;
}

// *******************************************************
//		SaveToXML()
// *******************************************************
STDMETHODIMP CCharts::SaveToXML(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = VARIANT_FALSE;

	USES_CONVERSION;
	CStringW s = OLE2W(Filename);
	if (s.GetLength() < 7)
	{
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	if (s.Right(6).MakeLower() != L".chart")
	{
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	CPLXMLNode *psTree = CPLCreateXMLNode(NULL, CXT_Element, "MapWindow");
	if (psTree)
	{
		Utility::WriteXmlHeaderAttributes(psTree, "Charts");

		CPLXMLNode* node = SerializeChartData("Charts");
		if (node)
		{
			long numShapes;
			_shapefile->get_NumShapes(&numShapes);
			Utility::CPLCreateXMLAttributeAndValue(node, "Count", CPLString().Printf("%d", numShapes));
			CPLAddXMLChild(psTree, node);
			*retVal = GdalHelper::SerializeXMLTreeToFile(psTree, s);
			CPLDestroyXMLNode(psTree);
		}
	}
	return S_OK;
}

// ********************************************************
//		SerializeLabelData()
// ********************************************************
CPLXMLNode* CCharts::SerializeChartData(CString ElementName)
{
	CPLXMLNode* psCharts = CPLCreateXMLNode(NULL, CXT_Element, ElementName);
	if (psCharts)
	{
		if (!_shapefile)
			return NULL;

		std::vector<ShapeRecord*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();
		if (data)
		{
			CPLXMLNode* nodeOld = NULL;
			CPLXMLNode* nodeNew = NULL;

			for (unsigned int i = 0; i < data->size(); i++)
			{
				if (!nodeOld)
				{
					nodeOld = CPLCreateXMLNode(psCharts, CXT_Element, "Chart");
				}
				else
				{
					nodeNew = CPLCreateXMLNode(NULL, CXT_Element, "Chart");
					CPLAddXMLSibling(nodeOld, nodeNew);
					nodeOld = nodeNew;
				}

				CChartInfo* info = (*data)[i]->chart;
				Utility::CPLCreateXMLAttributeAndValue(nodeOld, "X", CPLString().Printf("%f", info->x));
				Utility::CPLCreateXMLAttributeAndValue(nodeOld, "Y", CPLString().Printf("%f", info->y));
			}
		}
	}
	return psCharts;
}

// *************************************************************
//		get_SavingMode()
// *************************************************************
STDMETHODIMP CCharts::get_SavingMode(tkSavingMode* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = _savingMode;
	return S_OK;
}

// *************************************************************
//		put_SavingMode()
// *************************************************************
STDMETHODIMP CCharts::put_SavingMode(tkSavingMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_savingMode = newVal;
	return S_OK;
}

// *******************************************************
//		LoadFromXML()
// *******************************************************
STDMETHODIMP CCharts::LoadFromXML(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retVal = VARIANT_FALSE;

	USES_CONVERSION;
	CStringW name = OLE2W(Filename);
	if (!Utility::FileExistsW(name))
	{
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	CPLXMLNode* node = GdalHelper::ParseXMLFile(name);
	if (node)
	{
		node = CPLGetXMLNode(node, "=MapWindow");
		if (node)
		{
			CString s = CPLGetXMLValue(node, "FileVersion", "0");
			int version = atoi(s);

			node = CPLGetXMLNode(node, "Charts");
			if (node)
			{
				DeserializeChartData(node);
			}
		}
	}
	return S_OK;
}

// ********************************************************
//     DeserializeLabelData()
// ********************************************************
bool CCharts::DeserializeChartData(CPLXMLNode* node)
{
    if (!node || !_shapefile)
        return false;
	
	std::vector<ShapeRecord*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();
    if (!data)
        return false;

	this->Clear();
	((CShapefile*)_shapefile)->SetChartsPositions(lpNone);

	CString s;
	double x, y;
	int i = 0;

	node = CPLGetXMLNode(node, "Chart");

	int count = data->size();
	while (node && i < count)
	{
		s = CPLGetXMLValue(node, "X", "0.0");
		x = Utility::atof_custom(s);

		s = CPLGetXMLValue(node, "Y", "0.0");
		y = Utility::atof_custom(s);

		CChartInfo* info = (*data)[i]->chart;
		info->x = x;
		info->y = y;
		i++;

		node = node->psNext;
	}
	_chartsExist = true;
	return true;
}

#pragma endregion