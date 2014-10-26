#include "stdafx.h"
#include "MeasuringBase.h"

// *******************************************************
//		GetPartStartAndEnd()
// *******************************************************
bool MeasuringBase::GetPartStartAndEnd(int partIndex, MixedShapePart whichPoints, int& startIndex, int& endIndex)
{
	startIndex = 0;
	endIndex = _points.size();		// actually the next after end one

	switch (whichPoints)
	{
		case PolygonPart:
		{
			if (_firstPolyPointIndex == -1)
				return false;
			if (_firstPolyPointIndex >(int)_points.size() - 1)
			{
				return false;
			}
			startIndex = _firstPolyPointIndex;
			endIndex = (int)_points.size();
			return true;
		}
	}
	return true;
}

// *******************************************************
//		Clear()
// *******************************************************
void MeasuringBase::Clear() {
	ActiveShape::Clear();
	_firstPolyPointIndex = -1;
}

// *******************************************************
//		ClearIfStopped()
// *******************************************************
void MeasuringBase::ClearIfStopped()
{
	_closedPoly = false;
	if (_stopped)
	{
		_points.clear();
		_stopped = false;
	}
}

// *******************************************************
//		UndoPoint()
// *******************************************************
bool MeasuringBase::UndoPoint()
{
	bool result = ActiveShape::UndoPoint();
	_stopped = false;
	return result;
}

// *******************************************************
//		SetMeasuringType()
// *******************************************************
void MeasuringBase::SetMeasuringType(tkMeasuringType type)
{
	_measuringType = type;
	_firstPolyPointIndex = type == MeasureArea ? 0 : -1;
	if (_measuringType == MeasureArea)
		_closedPoly = true;
}

// *******************************************************
//		HandleProjPointAdd()
// *******************************************************
void MeasuringBase::HandleProjPointAdd(double projX, double projY)
{
	double pixelX, pixelY;
	_mapCallback->_ProjectionToPixel(projX, projY, &pixelX, &pixelY);
	AddPoint(projX, projY, pixelX, pixelY);
}

// *******************************************************
//		UpdatePolyCloseState()
// *******************************************************
void MeasuringBase::UpdatePolyCloseState(bool close, int pointIndex)
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
}

// *******************************************************
//		SnapToPreviousVertex()
// *******************************************************
bool MeasuringBase::SnapToPreviousVertex(int& vertexIndex, double screenX, double screenY)
{
	double minDist = DBL_MAX;
	double xTemp, yTemp;
	vertexIndex = -1;

	int size = _points.size() - 2;
	for (int i = 0; i < size; i++)
	{
		ProjToPixel(_points[i]->Proj.x, _points[i]->Proj.y, xTemp, yTemp);
		double dist = sqrt(pow(screenX - xTemp, 2.0) + pow(screenY - yTemp, 2.0));
		if (dist < minDist && dist < SNAP_TOLERANCE)
		{
			minDist = dist;
			vertexIndex = i;
		}
	}
	return vertexIndex > -1;
}

// *******************************************************
//		HasLine()
// *******************************************************
bool MeasuringBase::HasLine(bool dynamicBuffer)
{
	//if (_measuringType != MeasureDistance || _points.size()== 0) return false;
	if ( _points.size() == 0) return false;
	if (dynamicBuffer && _stopped) return false;
	return true;
}

// ************************************************
//     HasPolygon
// ************************************************
bool MeasuringBase::HasPolygon(bool dynamicBuffer)
{
	if (!dynamicBuffer) {
		return (_measuringType == MeasureDistance && _closedPoly) || (_measuringType == MeasureArea && _stopped && _points.size() > 2);
	}
	else {
		return _measuringType == MeasureArea && _points.size() > 0 && !_stopped;
	}
}

// ************************************************
//     HasPolygon
// ************************************************
bool MeasuringBase::HasPolygon()
{
	return _firstPolyPointIndex != -1 && _points.size() >= 2;
}