#include "StdAfx.h"
#include "HotTrackingInfo.h"
#include "ShapefileHelper.h"
#include "Shapefile.h"
#include "ShapeStyleHelper.h"

// ************************************************************
//		Update
// ************************************************************
void HotTrackingInfo::Update(IShapefile* source, IShape* shp, int layerHandle, int shapeIndex)
{
	VARIANT_BOOL vb;
	if (!Shapefile) {
		ComHelper::CreateInstance(idShapefile, (IDispatch**)&Shapefile);
	}
	else {
		Shapefile->Close(&vb);
	}

	ShpfileType type = ShapefileHelper::GetShapeType2D(source);

	((CShapefile*)Shapefile)->CreateNewCore(m_globalSettings.emptyBstr, type, false, &vb);

	long index = 0;
	Shapefile->EditInsertShape(shp, &index, &vb);

	Shapefile->RefreshExtents(&vb);
	LayerHandle = layerHandle;
	ShapeIndex = shapeIndex;
}

