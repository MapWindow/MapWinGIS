#pragma once
#include "ogr_feature.h"

class OgrLabelsHelper
{
public:
	struct LabelFields
	{
		int X;
		int Y;
		int Rotation;
		int Text;
        int OffsetX;
        int OffsetY;
		LabelFields() : X(-1), Y(-1), Rotation(-1), Text(-1), OffsetX(-1), OffsetY(-1) {}
		bool HasFields(){
			return X != -1 && Y != -1 && Rotation != -1 && Text != -1;
		}
	};

	static bool AddFieldsForLabels(OGRLayer* ogrLayer);
	static bool AddLabel2Feature(ILabels* labels, int shapeIndex, OGRFeature* feature, LabelFields& labelFields);
	static bool GetLabelFields(OGRLayer* ogrLayer, LabelFields& fields);
};

