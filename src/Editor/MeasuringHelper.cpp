#include "stdafx.h"
#include "MeasuringHelper.h"
#include "MeasuringBase.h"


// ************************************************************
//		OnCursorChanged
// ************************************************************
bool MeasuringHelper::OnCursorChanged(IMeasuring* measuring, tkCursorMode newCursor)
{
	if (!measuring) return false;

	VARIANT_BOOL isEmpty;
	measuring->get_IsEmpty(&isEmpty);
	if (!isEmpty)
	{
		VARIANT_BOOL persistent;
		measuring->get_Persistent(&persistent);
		if (newCursor != cmMeasure && !persistent)
		{
			measuring->Clear();
			return true;
		}
	}
	return false;
}

// ************************************************************
//		Serialize
// ************************************************************
CPLXMLNode* MeasuringHelper::Serialize(ActiveShape* activeShape, CString elementName)
{
	if (!activeShape) return NULL;

	USES_CONVERSION;

	CPLXMLNode* psTree = CPLCreateXMLNode(NULL, CXT_Element, elementName);
	
	MeasuringBase defaults;
	
	if (defaults.AngleFormat != activeShape->AngleFormat)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AngleFormat", CPLString().Printf("%d", (int)activeShape->AngleFormat));

	if (defaults.AnglePrecision != activeShape->AnglePrecision)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AnglePrecision", CPLString().Printf("%d", activeShape->AnglePrecision));

	if (defaults.AreaDisplayMode != activeShape->AreaDisplayMode)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AreaUnits", CPLString().Printf("%d", (int)activeShape->AreaDisplayMode));

	if (defaults.AreaPrecision != activeShape->AreaPrecision)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AreaPrecision", CPLString().Printf("%d", activeShape->AreaPrecision));

	if (defaults.BearingType != activeShape->BearingType)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "BearingType", CPLString().Printf("%d", (int)activeShape->BearingType));

	if (defaults.FillColor != activeShape->FillColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillColor", CPLString().Printf("%d", activeShape->FillColor));

	if (defaults.FillTransparency != activeShape->FillTransparency)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillTransparency", CPLString().Printf("%d", activeShape->FillTransparency));

	if (defaults.LengthPrecision != activeShape->LengthPrecision)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LengthPrecision", CPLString().Printf("%d", activeShape->LengthPrecision));

	if (defaults.LengthUnits != activeShape->LengthUnits)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LengthUnits", CPLString().Printf("%d", (int)activeShape->LengthUnits));

	if (defaults.LineColor != activeShape->LineColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LineColor", CPLString().Printf("%d", activeShape->LineColor));

	if (defaults.LineStyle != activeShape->LineStyle)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LineStyle", CPLString().Printf("%d", (int)activeShape->LineStyle));

	if (defaults.LineWidth != activeShape->LineWidth)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LineWidth", CPLString().Printf("%f", activeShape->LineWidth));

	if (defaults.PointLabelsVisible != activeShape->PointLabelsVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "PointLabelsVisible", CPLString().Printf("%d", (int)activeShape->PointLabelsVisible));

	if (defaults.PointsVisible != activeShape->PointsVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "PointsVisible", CPLString().Printf("%d", (int)activeShape->PointsVisible));

	if (defaults.ShowArea != activeShape->ShowArea)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ShowArea", CPLString().Printf("%d", (int)activeShape->ShowArea));

	if (defaults.ShowBearing != activeShape->ShowBearing)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ShowBearing", CPLString().Printf("%d", (int)activeShape->ShowBearing));

	if (defaults.ShowLength != activeShape->ShowLength)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ShowLength", CPLString().Printf("%d", (int)activeShape->ShowLength));

	if (defaults.ShowTotalLength != activeShape->ShowTotalLength)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ShowTotalLength", CPLString().Printf("%d", (int)activeShape->ShowTotalLength));

	return psTree;
}

// ************************************************************
//		Deserialize
// ************************************************************
bool MeasuringHelper::Deserialize(ActiveShape* shape, CPLXMLNode* node)
{
	if (!node || !shape) {
		return false;
	}

	MeasuringBase defaults;

	CString s;

	s = CPLGetXMLValue(node, "AngleFormat", NULL);
	shape->AngleFormat = (s == "") ? defaults.AngleFormat : (tkAngleFormat)atoi(s.GetString());

	s = CPLGetXMLValue(node, "AnglePrecision", NULL);
	shape->AnglePrecision = (s == "") ? defaults.AnglePrecision : atoi(s.GetString());
	
	s = CPLGetXMLValue(node, "AreaDisplayMode", NULL);
	shape->AreaDisplayMode = (s == "") ? defaults.AreaDisplayMode : (tkAreaDisplayMode)atoi(s.GetString());
	
	s = CPLGetXMLValue(node, "AreaPrecision", NULL);
	shape->AreaPrecision = (s == "") ? defaults.AreaPrecision : atoi(s.GetString());

	s = CPLGetXMLValue(node, "BearingType", NULL);
	shape->BearingType = (s == "") ? defaults.BearingType : (tkBearingType)atoi(s.GetString());

	s = CPLGetXMLValue(node, "FillColor", NULL);
	shape->FillColor = (s == "") ? defaults.FillColor : (OLE_COLOR)atoi(s.GetString());

	s = CPLGetXMLValue(node, "FillTransparency", NULL);
	shape->FillTransparency = (s == "") ? defaults.FillTransparency : (byte)atoi(s.GetString());

	s = CPLGetXMLValue(node, "LengthPrecision", NULL);
	shape->LengthPrecision = (s == "") ? defaults.LengthPrecision : atoi(s.GetString());

	s = CPLGetXMLValue(node, "LengthUnits", NULL);
	shape->LengthUnits = (s == "") ? defaults.LengthUnits : (tkLengthDisplayMode)atoi(s.GetString());

	s = CPLGetXMLValue(node, "LineColor", NULL);
	shape->LineColor = (s == "") ? defaults.LineColor : (OLE_COLOR)atoi(s.GetString());

	s = CPLGetXMLValue(node, "LineStyle", NULL);
	shape->LineStyle = (s == "") ? defaults.LineStyle : (tkDashStyle)atoi(s.GetString());

	s = CPLGetXMLValue(node, "LineWidth", NULL);
	shape->LineWidth = (s == "") ? defaults.LineWidth : static_cast<float>(Utility::atof_custom(s));

	s = CPLGetXMLValue(node, "PointsVisible", NULL);
	shape->PointsVisible = (s == "") ? defaults.PointsVisible : atoi(s.GetString()) == 0 ? false : true;

	s = CPLGetXMLValue(node, "PointLabelsVisible", NULL);
	shape->PointLabelsVisible = (s == "") ? defaults.PointLabelsVisible : atoi(s.GetString()) == 0 ? false : true;

	s = CPLGetXMLValue(node, "ShowArea", NULL);
	shape->ShowArea = (s == "") ? defaults.ShowArea : atoi(s.GetString()) == 0 ? false : true;

	s = CPLGetXMLValue(node, "ShowBearing", NULL);
	shape->ShowBearing = (s == "") ? defaults.ShowBearing : atoi(s.GetString()) == 0 ? false : true;

	s = CPLGetXMLValue(node, "ShowLength", NULL);
	shape->ShowLength = (s == "") ? defaults.ShowLength : atoi(s.GetString()) == 0 ? false : true;

	s = CPLGetXMLValue(node, "ShowTotalLength", NULL);
	shape->ShowTotalLength = (s == "") ? defaults.ShowTotalLength : atoi(s.GetString()) == 0 ? false : true;

	return true;
}
