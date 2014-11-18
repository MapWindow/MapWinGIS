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
	if (GetShapeType2D() == SHP_POLYGON && _points.size() > 3) {
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
		if (GetShapeType2D() == SHP_POLYGON)
		{
			if (_points[index]->Part == PartBegin)
				index = SeekPartEnd(index);
		}
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
		if (GetShapeType2D() == SHP_POLYGON)
		{
			if (_points[index]->Part == PartBegin)
				index = SeekPartEnd(index);
		}

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
	if (_selectedPart != index) 
	{
		_selectedPart = index;
		SelectRelatedParts(_selectedPart);		// for inner rings of poly
		return true;
	}
	//_selectedPart = -1;
	return true;
}

// ************************************************
//     SelectRelatedParts
// ************************************************
void EditorBase::SelectRelatedParts(int partIndex) 
{
	_selectedParts.clear();

	int numParts = GetNumParts();
	for (int i = 0; i < numParts; i++) 
	{
		if (i == partIndex || PartIsWithin(partIndex, i))
			_selectedParts.insert(i);
	}
}

// ************************************************
//     PartIsWithin
// ************************************************
bool EditorBase::PartIsWithin(int outerRing, int innerRing)
{
	if (GetShapeType2D() == SHP_POLYGON)
	{
		CComPtr<IShape> shp = GetPartAsShape(outerRing);
		if (shp != NULL)
		{
			int startIndex, endIndex;
			if (GetPart(innerRing, startIndex, endIndex))
			{
				VARIANT_BOOL vb;
				for (int i = startIndex; i <= endIndex; i++)
				{
					CComPtr<IPoint> pnt = NULL;
					GetUtils()->CreateInstance(idPoint, (IDispatch**)&pnt);
					pnt->put_X(_points[i]->Proj.x);
					pnt->put_Y(_points[i]->Proj.y);
					shp->PointInThisPoly(pnt, &vb);
					if (!vb) return false;
				}
				return true;
			}
		}
	}
	return false;
}

// ************************************************
//     GetPartAsShape
// ************************************************
IShape* EditorBase::GetPartAsShape(int partIndex)
{
	int startIndex, endIndex;
	if (GetPart(partIndex, startIndex, endIndex))
	{
		IShape* shp = NULL;
		GetUtils()->CreateInstance(idShape, (IDispatch**)&shp);
		VARIANT_BOOL vb;
		shp->Create(GetShapeType2D(), &vb);

		long pointIndex = 0;
		for (int i = startIndex; i <= endIndex; i++)
		{
			shp->AddPoint(_points[i]->Proj.x, _points[i]->Proj.y, &pointIndex);
			pointIndex++;
		}
		return shp;
	}
	return NULL;
}

// ************************************************
//     GetPart
// ************************************************
bool EditorBase::GetPart(int partIndex, int& startIndex, int& endIndex)
{
	startIndex = GetPartStart(partIndex);
	endIndex = SeekPartEnd(startIndex);
	if (startIndex >= (int)_points.size() && endIndex >= (int)_points.size()) return false;
	return startIndex != -1 && endIndex != -1;
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

	if (GetShapeType2D() == SHP_POLYGON && part != PartNone) {
		MeasurePoint* source = _points[vertexIndex];
		MeasurePoint* target = _points[closeIndex];
		source->CopyTo(*target);
	}

	SetModified();
	return true;
}

// *******************************************************
//		RemoveSelectedPart()
// *******************************************************
bool EditorBase::RemovePart()
{
	if (_selectedPart == -1 && _selectedVertex == -1) return false;
	
	// optionally remove the part with selected vertex	
	int partIndex = _selectedPart == -1 ? GetPartForPoint(_selectedVertex) : _selectedPart;
	
	SelectRelatedParts(partIndex);   // mark inner rings as well
	
	bool success = false;
	for (int n = GetNumParts() - 1; n >= 0; n--)
	{
		if (!PartIsSelected(n)) continue;

		int startIndex, endIndex;
		if (GetPart(n, startIndex, endIndex))
		{
			for (int i = endIndex; i >= startIndex; i--) {
				delete _points[i];
				_points.erase(_points.begin() + i);
			}
			success = true;
		}
	}

	if (success) {
		_selectedPart = -1;
		_selectedVertex = -1;
		SetModified();
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
	SetModified();
}

// *******************************************************
//		MovePart()
// *******************************************************
void EditorBase::MovePart(double offsetXProj, double offsetYProj)
{
	if (_selectedPart == -1) return;

	for (int n = 0; n < GetNumParts(); n++) 
	{
		if (!PartIsSelected(n)) continue;

		int startIndex, endIndex;
		GetPart(n, startIndex, endIndex);

		if (startIndex != -1 && endIndex != -1)
		{
			for (int i = startIndex; i <= endIndex; i++)
			{
				_points[i]->Proj.x += offsetXProj;
				_points[i]->Proj.y += offsetYProj;
				UpdateLatLng(i);
			}
			SetModified();
		}
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
	SetModified();
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

		SetSelectedVertex(pntIndex + 1);
		SetModified();
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
		if (CanDeleteVertex(_selectedVertex))
			return dtVertex;
	}
	if (HasSelectedPart() || HasSelectedVertex())
	{
		int partIndex = HasSelectedVertex() ? this->GetPartForPoint(_selectedVertex) : _selectedPart;
		if (CanDeletePart(partIndex))
			return dtPart;
	}
	return _points.size() > 0 ? dtShape : dtNone;
}

// *******************************************************
//		CanDeleteVertex()
// *******************************************************
bool EditorBase::CanDeleteVertex(int vertexIndex)
{
	bool polygon = GetShapeType2D() == SHP_POLYGON;
	bool polyline = GetShapeType2D() == SHP_POLYLINE;

	int startIndex = SeekPartStart(vertexIndex);
	int numPoints = SeekPartEnd(startIndex) - startIndex + 1;

	if (polygon && numPoints <= 4) return false;
	if (polyline && numPoints <= 2) return false;
	return true;
}

// *******************************************************
//		CanDeletePart()
// *******************************************************
bool EditorBase::CanDeletePart(int partIndex)
{
	SelectRelatedParts(partIndex);
	for (int i = 0; i < GetNumParts(); i++)
	{
		if (!PartIsSelected(i))
			return true;		// at least one part will be left, so it's ok to delete the selection
	}
	return false;   // all parts are selected, should be elevated to delete whole shape
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

