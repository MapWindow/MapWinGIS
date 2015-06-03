#include "stdafx.h"
#include "RasterBandHelper.h"

// ***************************************************************
//		Cast()
// ***************************************************************
CGdalRasterBand* RasterBandHelper::Cast(IGdalRasterBand* band)
{
	return (CGdalRasterBand*)band;
}

// ***************************************************************
//		ColorTableToColorScheme()
// ***************************************************************
// Retrieve the color table, assuming that there is a color entry
// for each integer value between min and max.
bool RasterBandHelper::ColorTableToColorScheme(GDALRasterBand* _poBand, IGridColorScheme** newscheme)
{
	if (!_poBand)
	{
		return false;
	}

	if (_poBand->GetRasterDataType() == GDT_Float32)
	{
		return false;
	}

	GDALColorTable * poCT = NULL;
	GDALColorInterp cInterp;
	const GDALColorEntry * poCE;
	double tmp = 0;
	bool colorTableExists = false;
	bool colorEntryExists = false;
	int bGotMin = false;
	int bGotMax = false;
	int iMin = static_cast<int>(GDALGetRasterMinimum(_poBand, &bGotMin));
	int iMax = static_cast<int>(GDALGetRasterMaximum(_poBand, &bGotMax));

	double range = GDALGetRasterMaximum(_poBand, &bGotMax) - GDALGetRasterMinimum(_poBand, &bGotMin);
	double shift = 0 - GDALGetRasterMinimum(_poBand, &bGotMin);

	poCT = _poBand->GetColorTable();

	if (poCT != NULL)
		colorTableExists = true;

	if (!colorTableExists)
		return false;

	CoCreateInstance(CLSID_GridColorScheme, NULL, CLSCTX_INPROC_SERVER, IID_IGridColorScheme, (void**)newscheme);
	(*newscheme)->put_AmbientIntensity(0.7);
	(*newscheme)->put_LightSourceIntensity(0.7);
	(*newscheme)->SetLightSource(0, 1);
	(*newscheme)->put_LightSourceIntensity(0.7);
	(*newscheme)->put_NoDataColor(0);

	cInterp = _poBand->GetColorInterpretation();

	OLE_COLOR lastColor = RGB(0, 0, 0);
	int boundA = iMin;
	int boundB = iMin;

	for (int i = iMin; i < iMax; i++)
	{
		poCE = GDALGetColorEntry(poCT, i);
		colorEntryExists = poCE != NULL;

		OLE_COLOR newColor = RGB(0, 0, 0);

		if (colorEntryExists)
		{
			if (cInterp == GCI_GrayIndex || cInterp == GCI_AlphaBand)
			{
				newColor = RGB((unsigned char)poCE->c4, (unsigned char)poCE->c4, (unsigned char)poCE->c4);
			}
			else
			{
				newColor = RGB((unsigned char)poCE->c1, (unsigned char)poCE->c2, (unsigned char)poCE->c3);
			}
		}
		else
		{
			newColor = RGB((unsigned char)((tmp + shift) * 255 / range), (unsigned char)((tmp + shift) * 255 / range), (unsigned char)((tmp + shift) * 255 / range));
		}

		if (newColor != lastColor && i != iMin)
		{
			// Put a break for the last chunk of color bands
			IGridColorBreak * newbreak;
			CoCreateInstance(CLSID_GridColorBreak, NULL, CLSCTX_INPROC_SERVER, IID_IGridColorBreak, (void**)&newbreak);
			newbreak->put_LowValue(boundA);
			newbreak->put_HighValue(boundB);
			newbreak->put_LowColor(lastColor);
			newbreak->put_HighColor(lastColor);
			newbreak->put_ColoringType(ColoringType::Random);

			(*newscheme)->InsertBreak(newbreak);
			newbreak->Release();

			// Set the new bounds and the new "last" color
			lastColor = newColor;
			boundA = i;
			boundB = i;
		}
		else if (newColor == lastColor)
		{
			boundB = i;
		}
	}

	// Write the last one
	IGridColorBreak * newbreak;
	CoCreateInstance(CLSID_GridColorBreak, NULL, CLSCTX_INPROC_SERVER, IID_IGridColorBreak, (void**)&newbreak);
	newbreak->put_LowValue(boundA);
	newbreak->put_HighValue(boundB);
	newbreak->put_LowColor(lastColor);
	newbreak->put_HighColor(lastColor);
	(*newscheme)->InsertBreak(newbreak);
	newbreak->Release();

	return true;
}

// ***************************************************************
//		AllocateBuffer()
// ***************************************************************
bool RasterBandHelper::AllocateBuffer(GDALDataType dataType, int width, int height, void** buffer)
{
	if (width <= 1 || height != 1)
	{
		CallbackHelper::ErrorMsg("GdalRasterBand::AllocateBuffer: Width and height must be greater than zero");
		return false;
	}
	
	dataType = SimplifyDataType(dataType);
	switch (dataType)
	{
		case GDT_Float64:
			*buffer = new double[width * height];
			break;
		case GDT_Float32:
			*buffer = new float[width * height];
			break;
		case GDT_Int32:
			*buffer = new int[width * height];
			break;
		default: 
			return false;
	}

	return true;
}

// ***************************************************************
//		GetSimpleDataType()
// ***************************************************************
GDALDataType RasterBandHelper::GetSimpleDataType(GDALRasterBand* band)
{
	if (!band) return GDT_Unknown;
	
	GDALDataType dataType = band->GetRasterDataType();

	return SimplifyDataType(dataType);
}

// ***************************************************************
//		GetSimpleDataType()
// ***************************************************************
GDALDataType RasterBandHelper::SimplifyDataType(GDALDataType dataType)
{
	switch (dataType)
	{
		case GDT_Unknown:
			return GDT_Unknown;
		case GDT_CFloat64:
		case GDT_Float64:
			return GDT_Float64;
		case GDT_CFloat32:
		case GDT_Float32:
			return GDT_Float32;
		default:
			return GDT_Int32;
	}
}

// ***************************************************************
//		GetOverviewCount()
// ***************************************************************
int RasterBandHelper::GetOverviewCount(GDALRasterBand* band)
{
	if (!band) return 0;

	m_globalSettings.SetGdalUtf8(true);
	int count = band->GetOverviewCount();
	m_globalSettings.SetGdalUtf8(false);

	return count;
}

