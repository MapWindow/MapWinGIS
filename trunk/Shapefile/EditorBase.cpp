#include "stdafx.h"
#include "EditorBase.h"

// ************************************************
//     GetClosestVertex
// ************************************************
int EditorBase::GetClosestVertex(double projX, double projY, double tolerance)
{
	double min = FLT_MAX;
	int pointIndex = -1;
	for (size_t i = 0; i < _points.size(); i++)
	{
		double dist = sqrt(pow(_points[i]->Proj.x - projX, 2.0) + pow(_points[i]->Proj.y - projY, 2.0));
		if (dist < min)
		{
			min = dist;
			pointIndex = i;
		}
	}
	if (pointIndex == 0 && HasClosedPolygon()) {
		pointIndex = _points.size() - 1;
	}

	return min < tolerance ? pointIndex : -1;
}

// ************************************************
//     SelectPart
// ************************************************
int EditorBase::SelectPart(double xProj, double yProj)
{
	if (GetNumParts() > 1) {
		int segment = FindSegmentWithPoint(xProj, yProj);
		return GetPartForPoint(segment);
	}
	return -1;
}

// ************************************************
//     HasClosedPolygon
// ************************************************
bool EditorBase::HasClosedPolygon()
{
	if (GetShapeType() == SHP_POLYGON && _points.size() > 3) {
		return (_points[0]->x == _points[_points.size() - 1]->x &&
			_points[0]->y == _points[_points.size() - 1]->y);
	}
	return false;
}

// ************************************************
//     SnapToPreviousVertex
// ************************************************
bool EditorBase::SnapToPreviousVertex(int& vertexIndex, double screenX, double screenY)
{
	ShpfileType type = Utility::ShapeTypeConvert2D(_shpType);
	if (type == SHP_POLYLINE && _points.size() == 0 ||
		type == SHP_POLYGON && _points.size() < 3)
	{
		return false;
	}
	vertexIndex = type != SHP_POLYGON ? -1 : 0;
	return true;
}

// ************************************************
//     Clear
// ************************************************
void EditorBase::Clear()
{
	SetShapeType(SHP_NULLSHAPE);
	ActiveShape::Clear();
}

// ************************************************
//     ClearHighlightedVertex
// ************************************************
bool EditorBase::ClearHighlightedVertex()
{
	if (_highlightedVertex != -1) {
		_highlightedVertex = -1;
		return true;
	}
	return false;
}

// ************************************************
//     ClearHighlightedPart
// ************************************************
bool EditorBase::ClearHighlightedPart()
{
	if (_highlightedPart != -1) {
		_highlightedPart = -1;
		return true;
	}
	return false;
}

// ************************************************
//     SetSelectedVertex
// ************************************************
bool EditorBase::SetSelectedVertex(int index)
{
	if (index < 0 || index >= (int)_points.size())
		return false;

	_selectedPart = -1;
	if (_selectedVertex != index)
	{
		if (_points[index]->Part == PartBegin)
			index = SeekPartEnd(index);
		_selectedVertex = index;
		return true;
	}
	return false;
}

// ************************************************
//     SetHighlightedVertex
// ************************************************
bool EditorBase::SetHighlightedVertex(int index)
{
	if (index < 0 || index >= (int)_points.size())
		return false;

	if (index != _highlightedVertex)
	{
		if (_points[index]->Part == PartBegin)
			index = SeekPartEnd(index);

		_highlightedVertex = index;
		_highlightedPart = -1;
		return true;
	}
	return false;
}

// ************************************************
//     SetSelectedPart
// ************************************************
bool EditorBase::SetSelectedPart(int index)
{
	_selectedVertex = -1;
	if (_selectedPart != index) {
		_selectedPart = index;
		return true;
	}
	_selectedPart = -1;
	return false;
}

// ************************************************
//     SetHighlightedPart
// ************************************************
bool EditorBase::SetHighlightedPart(int part)
{
	if (part != _highlightedPart) {
		_highlightedPart = part;
		_highlightedVertex = -1;
		return true;
	}
	return false;
}

// *******************************************************
//		RemoveVertex()
// *******************************************************
bool EditorBase::RemoveVertex(int vertexIndex)
{
	if (vertexIndex < 0 && vertexIndex >= (int)_points.size())
		return false;

	PointPart part = _points[vertexIndex]->Part;

	delete _points[vertexIndex];
	_points.erase(_points.begin() + vertexIndex);

	if (vertexIndex == _selectedVertex)
		_selectedVertex = -1;

	if (part == PartEnd) vertexIndex--;
	if (part != PartNone)
		_points[vertexIndex]->Part = part;

	// make sure that first and last points of poly are still the same		
	int closeIndex = GetCloseIndex(vertexIndex);

	if (GetShapeType() == SHP_POLYGON && part != PartNone) {
		MeasurePoint* source = _points[vertexIndex];
		MeasurePoint* target = _points[closeIndex];
		source->CopyTo(*target);
	}

	_areaRecalcIsNeeded = true;
	return true;
}

// *******************************************************
//		RemoveSelectedPart()
// *******************************************************
bool EditorBase::RemovePart()
{
	// optionally remove the part with selected vertex
	if (_selectedPart == -1 && _selectedVertex == -1) return false;
	int partIndex = _selectedPart == -1 ? GetPartForPoint(_selectedVertex) : _selectedPart;

	int startIndex = GetPartStart(partIndex);
	int endIndex = SeekPartEnd(startIndex);
	if (startIndex != -1 && endIndex != -1)
	{
		for (int i = endIndex; i >= startIndex; i--) {
			delete _points[i];
			_points.erase(_points.begin() + i);
		}
		_selectedPart = -1;
		_selectedVertex = -1;
		_areaRecalcIsNeeded = true;
		return true;
	}
	return false;
}

// *******************************************************
//		Move()
// *******************************************************
void EditorBase::Move(double offsetXProj, double offsetYProj)
{
	for (size_t i = 0; i < _points.size(); i++)
	{
		_points[i]->Proj.x += offsetXProj;
		_points[i]->Proj.y += offsetYProj;
		UpdateLatLng(i);
	}
	_areaRecalcIsNeeded = true;
}

// *******************************************************
//		MovePart()
// *******************************************************
void EditorBase::MovePart(double offsetXProj, double offsetYProj)
{
	if (_selectedPart == -1) return;
	int startIndex = GetPartStart(_selectedPart);
	int endIndex = SeekPartEnd(startIndex);

	if (startIndex != -1 && endIndex != -1)
	{
		for (int i = startIndex; i <= endIndex; i++)
		{
			_points[i]->Proj.x += offsetXProj;
			_points[i]->Proj.y += offsetYProj;
			UpdateLatLng(i);
		}
		_areaRecalcIsNeeded = true;
	}
}

// *******************************************************
//		MoveVertex()
// *******************************************************
void EditorBase::MoveVertex(double xProj, double yProj)
{
	int index = _selectedVertex;
	int closeIndex = GetCloseIndex(index);

	if (index >= 0 && index < (int)_points.size())
	{
		_points[index]->Proj.x = xProj;
		_points[index]->Proj.y = yProj;
		UpdateLatLng(index);

		// coordinates of the first and last point of polygon must be the same
		if (closeIndex >= 0 && closeIndex < (int)_points.size()) {
			_points[closeIndex]->Proj.x = _points[index]->Proj.x;
			_points[closeIndex]->Proj.y = _points[index]->Proj.y;
			UpdateLatLng(closeIndex);
		}
	}
	_areaRecalcIsNeeded = true;
}

// *******************************************************
//		TryInsertVertex()
// *******************************************************
bool EditorBase::TryInsertVertex(double xProj, double yProj)
{
	int pntIndex = FindSegmentWithPoint(xProj, yProj);
	if (pntIndex != -1)
	{
		MeasurePoint* pnt = new MeasurePoint();
		pnt->Proj.x = xProj;
		pnt->Proj.y = yProj;
		_points.insert(_points.begin() + pntIndex + 1, pnt);
		UpdateLatLng(pntIndex + 1);
		UpdateLatLng(pntIndex + 2);

		_areaRecalcIsNeeded = true;
		return true;
	}
	return false;
}

// *******************************************************
//		UpdatePoint()
// *******************************************************
bool EditorBase::UpdatePoint(int pointIndex, double projX, double ProjY)
{
	MeasurePoint* pnt = GetPoint(pointIndex);
	if (pnt)
	{
		pnt->Proj.x = projX;
		pnt->Proj.y = ProjY;
		UpdateLatLng(pointIndex);
		return true;
	}
	return false;
}

// *******************************************************
//		GetDeleteTarget()
// *******************************************************
tkDeleteTarget EditorBase::GetDeleteTarget()
{
	if (HasSelectedVertex())
	{
		return CanDeleteVertex(_selectedVertex) ? dtVertex : dtPart;
	}
	if (HasSelectedPart())
	{
		return dtPart;
	}
	return _points.size() > 0 ? dtShape : dtNone;
}

// *******************************************************
//		CanDeleteVertex()
// *******************************************************
bool EditorBase::CanDeleteVertex(int vertexIndex)
{
	bool polygon = GetShapeType() == SHP_POLYGON;
	bool polyline = GetShapeType() == SHP_POLYLINE;

	int startIndex = SeekPartStart(vertexIndex);
	int numPoints = SeekPartEnd(startIndex) - startIndex + 1;

	if (polygon && numPoints <= 4) return false;
	if (polyline && numPoints <= 2) return false;
	return true;
}

// ************************************************
//     HasPolygon
// ************************************************
bool EditorBase::HasPolygon(bool dynamicBuffer)
{
	if (_points.size() < 2) return false;
	if (dynamicBuffer != _creationMode) return false;
	return (Utility::ShapeTypeConvert2D(_shpType) == SHP_POLYGON);
}

// ************************************************
//     HasPolygon
// ************************************************
bool EditorBase::HasPolygon()
{
	if (GetNumParts() > 1) return false;
	if (_points.size() < 2) return false;
	return (Utility::ShapeTypeConvert2D(_shpType) == SHP_POLYGON);
}

// *******************************************************
//		HasLine()
// *******************************************************
bool EditorBase::HasLine(bool dynamicBuffer)
{
	if (_points.size() == 0) return false;
	return dynamicBuffer == _creationMode;
}


