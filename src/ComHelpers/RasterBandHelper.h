
#pragma once
#include "GdalRasterBand.h"

class RasterBandHelper
{
public:
	static CGdalRasterBand* Cast(IGdalRasterBand* band);
	
};

