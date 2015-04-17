#include "stdafx.h"
#include "RasterBandHelper.h"

// ***************************************************************
//		Cast()
// ***************************************************************
CGdalRasterBand* RasterBandHelper::Cast(IGdalRasterBand* band)
{
	return (CGdalRasterBand*)band;
}
