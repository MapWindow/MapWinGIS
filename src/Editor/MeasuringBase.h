#pragma once
#include "ActiveShape.h"

class MeasuringBase: public ActiveShape
{
public:
	MeasuringBase(void) {
		_stopped = false;
		_measuringType = tkMeasuringType::MeasureDistance;
		_closedPoly = false;
		_firstPolyPointIndex = -1;
		_mixedLinePolyMode = false;
		Persistent = VARIANT_FALSE;
	};
	~MeasuringBase(void){ 
};

private:
	bool _stopped;
	bool _closedPoly;
	tkMeasuringType _measuringType;
	bool _mixedLinePolyMode;
	int	_firstPolyPointIndex;
public:
	VARIANT_BOOL Persistent;			   // will be drawn even when cursor mode isn't cmMeasure

	int GetFirstPolyPointIndex() { return _firstPolyPointIndex == -1 ? 0 : _firstPolyPointIndex; }
	bool GetMixedMode() { return _mixedLinePolyMode; }
	bool DrawAccumalatedLength() { return true; }
	bool CloseOnPreviousVertex() { return _measuringType == MeasureDistance;}
	tkMeasuringType GetMeasuringType() { return _measuringType; };
	bool HasClosedPolygon() { return false; }
	bool HasPolygon(bool dynamicBuffer);
	bool HasPolygon();
	bool HasLine(bool dynamicBuffer);
	bool IsDynamic() {return !_stopped;}
	bool IsStopped() {return _stopped; }
	void Stop() { _stopped = true; }
	bool NeedsDrawing() {return Persistent && _points.size() > 0;}
	void UpdatePolyCloseState(bool close, int pointIndex = -1);
	bool SnapToPreviousVertex(int& vertexIndex, double screenX, double screenY);
	void HandleProjPointAdd(double projX, double projY);
	void SetMeasuringType(tkMeasuringType type);
	bool UndoPoint();
	void ClearIfStopped();
	void Clear();
	bool GetPartStartAndEnd(int partIndex, MixedShapePart whichPoints, int& startIndex, int& endIndex);
};
