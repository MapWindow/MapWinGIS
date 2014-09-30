#pragma once
#include "ActiveShape.h"

class MeasuringBase: public ActiveShape
{
public:
	MeasuringBase(void) {
		_stopped = false;
		_measuringType = tkMeasuringType::MeasureDistance;
		_closedPoly = false;
		Persistent = VARIANT_FALSE;
	};
	~MeasuringBase(void){ };

private:
	bool _stopped;
	bool _closedPoly;
	tkMeasuringType _measuringType;
public:
	
	VARIANT_BOOL Persistent;			   // will be drawn even when cursor mode isn't cmMeasure
public:
	// implementation of abstract members
	bool DrawAccumalatedLength() { return true; }

	bool CloseOnPreviousVertex()
	{
		return _measuringType == MeasureDistance;
	}
	void UpdatePolyCloseState(bool close, int pointIndex = -1)
	{
		if (_measuringType == MeasureDistance)
		{
			if (close && pointIndex != -1){
				_closedPoly = true;
				_firstPolyPointIndex = pointIndex;
				_stopped = true;
			}
			else if (!close) {
				_closedPoly = false;
				_firstPolyPointIndex = -1;
				_stopped = false;
			}
		}
	};
	bool HasStaticPolygon()
	{
		if ((_measuringType == MeasureDistance && _closedPoly) || 
			(_measuringType == MeasureArea && _stopped && _points.size() > 2))
		{
			return true;
		};
		return false;
	}
	bool HasStaticLine()
	{
		return _measuringType == MeasureDistance && _points.size() > 0;
	}
	bool HasDynamicPolygon()
	{
		return _measuringType == MeasureArea && _points.size() > 0 && !_stopped;
	}
	bool HasDynamicLine()
	{
		return _measuringType == MeasureDistance && _points.size() > 0 && !_stopped;
	}
	void ClearIfStopped()
	{
		_closedPoly = false;
		if (_stopped)
		{
			_points.clear();
			_stopped = false;
		}
	}
	bool IsDynamic()
	{
		return !_stopped;
	}

	virtual bool UndoPoint()
	{
		bool result = ActiveShape::UndoPoint();
		_stopped = false;
		return result;
	}

	bool SnapToPreviousVertex(int& vertexIndex, double screenX, double screenY)
	{
		double minDist = DBL_MAX;
		double xTemp, yTemp;
		vertexIndex = -1;

		int size = _points.size() - 2;
		for(int i = 0; i < size; i++)
		{
			ProjToPixel(_points[i]->Proj.x, _points[i]->Proj.y, xTemp, yTemp);
			double dist = sqrt( pow(screenX - xTemp, 2.0) + pow(screenY - yTemp, 2.0));
			if (dist < minDist && dist < SNAP_TOLERANCE)
			{
				minDist = dist;
				vertexIndex = i;
			}
		}
		return vertexIndex > -1;
	}

public:
	bool IsStopped() {return _stopped; }
	void Stop() 
	{ 
		_stopped = true; 
	}

	bool NeedsDrawing() {
		return Persistent && _points.size() > 0;
	}
	void SetMeasuringType(tkMeasuringType type)
	{
		_measuringType = type;
		_firstPolyPointIndex = type == MeasureArea ? 0 : -1;
		if (_measuringType == MeasureArea)
			_closedPoly = true;
	}
	tkMeasuringType GetMeasuringType() { return _measuringType; };

};
