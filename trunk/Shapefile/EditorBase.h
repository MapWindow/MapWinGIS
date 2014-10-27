#pragma once
#include "ActiveShape.h"

class EditorBase: public ActiveShape
{
private:
	ShpfileType _shpType;
	bool _creationMode;

public:
	EditorBase(void) {
		_shpType = SHP_NULLSHAPE;
		_creationMode = false;
		DisplayAngles = true;
	};
	virtual ~EditorBase(void) {};

public:
	ShpfileType GetShapeType() { return Utility::ShapeTypeConvert2D(_shpType); }
	bool DrawAccumalatedLength() { return false; }
	bool CloseOnPreviousVertex() {	return false; }
	void UpdatePolyCloseState(bool close, int pointIndex = -1) {}
	void SetShapeType(ShpfileType mode) { _shpType = mode; }
	void SetCreationMode(bool mode) { _creationMode = mode; }
	bool GetCreationMode() { return _creationMode; }
	bool ShapeCreated() { return _shpType != SHP_NULLSHAPE; };
	bool HasSelectedVertex() { return _selectedVertex != -1; }
	bool HasSelectedPart() { return _selectedPart != -1; }
	void ClearIfStopped() {	}
	bool IsDynamic() {return _creationMode;	}
	bool SnapToPreviousVertex(int& vertexIndex, double screenX, double screenY);
	bool HasPolygon(bool dynamicBuffer);
	bool HasPolygon();
	bool HasLine(bool dynamicBuffer);
	void Clear();
	bool SetHighlightedPart(int part);
	bool SetSelectedPart(int index);
	bool SetHighlightedVertex(int index);
	bool SetSelectedVertex(int index);
	bool ClearHighlightedPart();
	bool ClearHighlightedVertex();
	int SelectPart(double xProj, double yProj);
	int GetClosestVertex(double projX, double projY, double tolerance);
	bool HasClosedPolygon();
	bool RemovePart();
	bool RemoveSelectedVertex() {return RemoveVertex(_selectedVertex);}
	bool RemoveVertex(int vertexIndex);
	void Move(double offsetXProj, double offsetYProj);
	void MovePart(double offsetXProj, double offsetYProj);
	void MoveVertex(double xProj, double yProj);
	bool TryInsertVertex(double xProj, double yProj);
	bool UpdatePoint(int pointIndex, double projX, double ProjY);
	tkDeleteTarget GetDeleteTarget();
	bool CanDeleteVertex(int vertexIndex);
	bool PartIsWithin(int outerRing, int innerRing);
	bool GetPart(int partIndex, int& startIndex, int& endIndex);
	IShape* GetPartAsShape(int partIndex);
	void SelectRelatedParts(int outerRing);
	bool CanDeletePart(int partIndex);
};
