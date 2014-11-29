//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//
//Contributor(s): (Open source contributors should list themselves and their modifications here).
//10-25-2005 Rob Cairns. Added extents property. Changed structure of LoadImageBuffer
//////////////////////////////////////////////////////////////////////

#pragma once
#include "colour.h"
#include "ImageStructs.h"

class tkRaster
{
public:
    tkRaster()
		: transColor(RGB(0,0,0))
	{	//Rob Cairns
		_rasterDataset=NULL;
		_predefinedColorScheme = NULL;
		_customColorScheme = NULL;
		_poBandR = NULL;
		_poBandG = NULL;
		_poBandB = NULL;
		_poCT = NULL;
		hasColorTable = false;
		hasTransparency = false;
		cBack = NULL;
		dX = - 1.0; 
		dY = -1.0;
		XllCenter = -1.0;
		YllCenter = -1.0;
		width = 0;
		height = 0;
		orig_XllCenter = 0.0;
		orig_YllCenter = 0.0;
		orig_dX = 1.0;
		orig_dY = 1.0;
		orig_Width = 0;
		orig_Height = 0;
		clearGDALCache = false;
		warped = false;
		allowHillshade = true;
		allowAsGrid = grForGridsOnly;
		activeBandIndex = 1;
		_predefinedColors = FallLeaves;
		ComHelper::CreateInstance(idGridColorScheme, (IDispatch**)&_predefinedColorScheme);
	};
	~tkRaster()
	{
		if (_predefinedColorScheme)
			_predefinedColorScheme->Release();
	};

private:
	struct BreakVal
	{	
		double lowVal;
		double highVal;
	};

private:
	double _dfMin;
	double _dfMax;
	double _adfMinMax[2];	// holds the min and max values for grid images
	
	GDALDataType _dataType;
	GDALDataset * _rasterDataset;
	GDALRasterBand * _poBandR;
	GDALRasterBand * _poBandB;
	GDALRasterBand * _poBandG;

	HandleImage _handleImage;
	GDALColorTable * _poCT;
	GDALPaletteInterp _cPI;
	GDALColorInterp _cInterp;
	GDALDataType _genericType;

	//Histogram variables
	bool	_histogramComputed;	// Has the histogram been computed
	bool	_allowHistogram;		// It the computation fails don't allow again
	int		_nLUTBins;
    double  *_padfScaleMin;
    double  *_padfScaleMax;
    int     **_papanLUTs;
	int _nBands;					// No of bands in the image

	const char* _ciName;			// Color interpretation name
	int _buffSize;
	int _colorMapSize;
	ColorEntry _colorMap[4096];

	Extent _extents;		// full extents in map coordinates
	Extent _visibleExtents;	// visible extents in map coordinates
	
	BSTR _key;
	
	float _pctTrans;

	PredefinedColorScheme _predefinedColors; //Image color scheme if read as grid
	ImageType _imgType;
	
	IGridColorScheme * _customColorScheme;		// the one set by Image.GridColorScheme property
	IGridColorScheme * _predefinedColorScheme;		// the one set by Image.ImageColorScheme property

public:
	//world coordinate related variables
	double dY;				// change in Y (for size of cell or pixel)
	double dX;				// change in X (for size of cell or pixel)
	double YllCenter;		// Y coordinate of lower left corner of image (world coordinate)
	double XllCenter;		// X coordinate of lower left corner of image (world coordinate)	
	long width, height;

	bool warped;	// a warped dataset was created on opening
	int activeBandIndex;
	tkGridRendering allowAsGrid;		// Allow the image to be read as grid (if appropriate)
	bool clearGDALCache;
	int imageQuality;		// Set the image quality 0-100
	bool hasColorTable;		// The image has a color table	
	GDALPaletteInterp paletteInterp; // palette interpretation
	bool allowHillshade;	// if false changes ColorType from Hillshade to gradient
	bool useHistogram;		// Use histogram equalization
	bool hasTransparency;
	colort transColor;		// transparent color

	CRect visibleRect;		// indices of pixels of image that are visible at least partially
	ICallback * cBack;

	// original size and position
	double orig_XllCenter;
	double orig_YllCenter;
	double orig_dX;
	double orig_dY;
	long orig_Width;
	long orig_Height;

private:
	bool LoadRasterCore(CStringA& filename, GDALAccess accessType = GA_ReadOnly);
	HandleImage tkRaster::ChooseRenderingMethod();
	IGridColorScheme* tkRaster::GetColorSchemeForRendering();		// returns either of the 2 available
	void tkRaster::ComputeBandMinMax();
	bool ReadImage(colour ** ImageData, int xOffset, int yOffset, int width, int height, int xBuff, int yBuff);
	void GDALColorEntry2Colour(int band, double colorValue, double shift, double range, double noDataValue, const GDALColorEntry * poCE, bool useHistogram, colour* result);
	template <typename T>
	bool AddToBufferAlt(colour ** ImageData, T* data, int xBuff, int yBuff, int band, double shift, double range, double noDataValue, const GDALColorEntry * poCE, bool useHistogram);
	bool ReadGridAsImage(colour ** ImageData, int xOff, int yOff, int width, int height, int xBuff, int yBuff, bool setRGBToGrey); // double dx, double dy, bool setRGBToGrey, PredefinedColorScheme imageColorScheme,bool clearGDALCache);
	long findBreak(std::deque<BreakVal> & bvals, double val);
	bool ComputeEqualizationLUTs(CStringW filename, double **ppadfScaleMin, double **ppadfScaleMax, int ***ppapanLUTs);

public:
	bool SetActiveBandIndex(int bandIndex);
	GDALRasterBand* get_RasterBand(int BandIndex);
	void ApplyCustomColorScheme(IGridColorScheme * scheme) { ComHelper::SetRef((IDispatch*)scheme, (IDispatch**)&_customColorScheme, true);	}
	void ApplyPredefinedColorScheme(PredefinedColorScheme colorScheme);
	IGridColorScheme* get_CustomColorScheme() { return _customColorScheme; }
	GDALDataset* get_Dataset()	{return _rasterDataset;}
	bool get_HasColorTable(){return hasColorTable;};
	GDALPaletteInterp get_PaletteInterpretation(){return _cPI;};
	int get_NoBands(){return _nBands;};
	double getDX() {return dX;} 
	double getDY() {return dY;}
	double getYllCenter() {return YllCenter;}
	double getXllCenter() {return XllCenter;}
	long getWidth(){return width;}
	long getHeight(){return height;}
	PredefinedColorScheme GetDefaultColors() { return _predefinedColors; }

	bool LoadRaster(CStringW filename, GDALAccess accessType = GA_ReadOnly); 
	bool LoadBuffer(colour** ImageData, double MinX, double MinY, double MaxX, double MaxY, CStringW filename, tkInterpolationMode downsamplingMode, bool setRGBToGrey, double mapUnitsPerScreenPixel);
    bool LoadBufferFull(colour** ImageData, CStringW filename, double maxBufferSize = 50);
	void tkRaster::RefreshExtents();
	bool SetNoDataValue(double Value);
	bool CanUseExternalColorScheme();
	bool tkRaster::WillBeRenderedAsGrid();
	void Close();
	int Dereference();
	bool IsRgb();
};

