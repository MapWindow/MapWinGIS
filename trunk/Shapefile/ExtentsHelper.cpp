#include "stdafx.h"
#include "ExtentsHelper.h"

// ****************************************************************
//		Populate()
// ****************************************************************
IExtents* ExtentsHelper::Populate(Extent& source)
{
	IExtents* box = NULL;
	GetUtils()->CreateInstance(idExtents, (IDispatch**)&box);
	box->SetBounds(source.left, source.bottom, 0.0, source.right, source.top, 0.0);
	return box;
}

// ****************************************************************
//		GetWorldBounds()
// ****************************************************************
IExtents* ExtentsHelper::GetWorldBounds()
{
	IExtents* bounds = NULL;
	GetUtils()->CreateInstance(idExtents, (IDispatch**)&bounds);
	bounds->SetBounds(-179.5, -85.0, 0.0, 179.5, 85.0, 0.0);
	return bounds;
}
