
#pragma once
#include <set>
#include "GdalRasterBand.h"
#include "Templates.h"

class RasterBandHelper
{
public:
	static CGdalRasterBand* Cast(IGdalRasterBand* band);
	static bool ColorTableToColorScheme(GDALRasterBand* band, IGridColorScheme** newScheme);
	static GDALDataType GetSimpleDataType(GDALRasterBand* band);
	static GDALDataType SimplifyDataType(GDALDataType dataType);
	static bool AllocateBuffer(GDALDataType dataType, int width, int height, void** buffer);

	template <typename T>
	static bool GetUniqueValues(GDALRasterBand* band, GDALDataType dataType, set<T>& values, int maxCount, VARIANT_BOOL* clipped)
	{
		*clipped = VARIANT_FALSE;

		if (!band) return false;

		int width = band->GetXSize();
		int height = band->GetYSize();

		T* buffer = NULL;
		if (!AllocateBuffer(dataType, width, 1, (void**)&buffer)) {
			return false;
		}

		bool result = false;

		long lastPercent = 0;

		for (int row = 0; row < height; row++)
		{
			CallbackHelper::Progress(NULL, row, height, "Building list of unique values", lastPercent);

			CPLErr err = band->RasterIO(GF_Read, 0, row, width, 1, buffer, width, 1, dataType, 0, 0);
			if (err != CPLErr::CE_None)
			{
				CallbackHelper::ErrorMsg("GdalRasterBand::GetUniqueValues: failed to read row.");
				goto cleaning;
			}

			for (int j = 0; j < width; j++)
			{
				T value = *(buffer + j);

				if (values.find(value) == values.end())
				{
					values.insert(value);
					if (values.size() >(unsigned int)maxCount)
					{
						// there are too many values
						*clipped = VARIANT_TRUE;
						result = true;
						goto cleaning;
					}
				}
			}
		}

		result = true;

cleaning:
		CallbackHelper::ProgressCompleted(NULL);
		delete[] buffer;
		return true;
	}
};

