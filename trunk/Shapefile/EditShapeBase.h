#pragma once
#include "ActiveShape.h"

class EditShapeBase: public ActiveShape
{
private:
	ShpfileType _shpType;
	bool _creationMode;
	
public:
	EditShapeBase(void) {
		_shpType = SHP_NULLSHAPE;
		_creationMode = false;
		_mixedLinePolyMode = false;
		DisplayAngles = true;
	};
	~EditShapeBase(void) {};

public:
	// implementation of abstract members
	bool DrawAccumalatedLength() { return false; }

	bool CloseOnPreviousVertex() {	return false; }
	void UpdatePolyCloseState(bool close, int pointIndex = -1) {
		// TODO: implement
	}
	bool HasStaticPolygon()
	{
		//return Utility::ShapeTypeConvert2D(_shpType) == SHP_POLYGON && !_creationMode;
		if (_points.size() < 2) return false;
		if (_creationMode) return false;
		if (_mixedLinePolyMode)
		{
			return _firstPolyPointIndex != -1;
		}
		else
		{
			if (Utility::ShapeTypeConvert2D(_shpType) == SHP_POLYGON) return true;
		}
		return false;
	}
	bool HasDynamicPolygon()
	{
		if (_points.size() < 2) return false;
		if (!_creationMode) return false;
		if (_mixedLinePolyMode)
		{
			if (_firstPolyPointIndex != -1) return true;
		}
		else
		{
			if (Utility::ShapeTypeConvert2D(_shpType) == SHP_POLYGON) return true;
		}
		return false;
	}
	bool HasStaticLine()
	{
		return _points.size() > 0;
	}
	bool HasDynamicLine()
	{
		return _points.size() > 0 && _creationMode;
	}
	void ClearIfStopped() {
		// TODO: implement
	}
	bool IsDynamic() {return _creationMode;	}

	bool SnapToPreviousVertex(int& vertexIndex, double screenX, double screenY)
	{
		ShpfileType type = Utility::ShapeTypeConvert2D(_shpType);
		if (type == SHP_POLYLINE && _points.size() == 0 ||
			type == SHP_POLYGON && _points.size() < 3)
		{
			return false;
		}
		vertexIndex = type != SHP_POLYGON ? -1: 0;
		return true;
	}
	virtual bool UndoPoint()
	{
		if (_firstPolyPointIndex >= (int)_points.size() - 1) 
		{
			_firstPolyPointIndex = -1;
			return true;
		}
		return ActiveShape::UndoPoint();
	}

	void Clear()
	{
		SetShapeType(SHP_NULLSHAPE);
		ActiveShape::Clear();
	}
public:
	void SetShapeType(ShpfileType mode) {_shpType = mode;}
	ShpfileType GetShapeType() { return Utility::ShapeTypeConvert2D(_shpType);}

	void SetCreationMode(bool mode) 
	{
		if (mode != _creationMode) {
			_creationMode = mode;
		}
	}
	bool GetCreationMode() { return _creationMode;}

	bool ShapeCreated() 
	{ 
		return _shpType != SHP_NULLSHAPE;
	};

	void SetShape(IShape* shape);

	int SelectVertex(double projX, double projY, double tolerance)
	{
		double min = FLT_MAX;
		int pointIndex = -1;
		for(size_t i = 0; i < _points.size(); i++)
		{
			double dist = sqrt( pow(_points[i]->Proj.x - projX, 2.0) + pow(_points[i]->Proj.y - projY, 2.0) );
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

	bool HasClosedPolygon() 
	{
		if (GetShapeType() == SHP_POLYGON && _points.size() > 3) {
			return (_points[0]->x == _points[_points.size() - 1]->x &&
					_points[0]->y == _points[_points.size() - 1]->y);
		}
		return false;
	}
};
