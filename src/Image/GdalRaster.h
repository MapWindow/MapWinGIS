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
#include <cassert>
#include "colour.h"
#include "ImageStructs.h"

class GdalRaster
{
public:
    GdalRaster()
		: _transColor(RGB(0,0,0))
	{	
		_rasterDataset=NULL;
		_predefinedColorScheme = NULL;
		_customColorScheme = NULL;
		_poBandR = NULL;
		_poBandG = NULL;
		_poBandB = NULL;
		_colorTable = NULL;
		_hasColorTable = false;
		_hasTransparency = false;
		_callback = NULL;
		_dX = - 1.0; 
		_dY = -1.0;
		_xllCenter = -1.0;
		_yllCenter = -1.0;
		_width = 0;
		_height = 0;
		_origXllCenter = 0.0;
		_origYllCenter = 0.0;
		_origDx = 1.0;
		_origDy = 1.0;
		_origWidth = 0;
		_origHeight = 0;
		clearGDALCache = false;
		_warped = false;
		_allowHillshade = true;
		_allowAsGrid = grForGridsOnly;
		_activeBandIndex = 1;
		_predefinedColors = FallLeaves;
		_redBandIndex = 1;
		_greenBandIndex = 2;
		_blueBandIndex = 3;
		_useRgbBandMapping = false;
		_forceSingleBandRendering = false;
		ComHelper::CreateInstance(idGridColorScheme, (IDispatch**)&_predefinedColorScheme);
	};

	~GdalRaster()
	{
		Close();
	};

private:
	struct BreakVal
	{	
		float lowVal;
		float highVal;
		OLE_COLOR hiColor;
		OLE_COLOR lowColor;
				
		ColoringType colortype;
		GradientModel gradmodel;
		
		BreakVal(IGridColorBreak* bi)
		{
			assert(bi != nullptr);
			double val;
			bi->get_LowValue(&val);
			lowVal = static_cast<float>(val);
			bi->get_HighValue(&val);
			highVal = static_cast<float>(val);
			bi->get_HighColor(&hiColor);
			bi->get_LowColor(&lowColor);
			bi->get_ColoringType(&colortype);
			bi->get_GradientModel(&gradmodel);
		}
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

	bool _useRgbBandMapping;
	int _redBandIndex;
	int _greenBandIndex;
	int _blueBandIndex;

	//HandleImage _handleImage;
	GDALColorTable * _colorTable;
	GDALPaletteInterp _palleteInterpretation;
	GDALColorInterp _cInterp;		// temporary storage during reading of built-in color table
	GDALDataType _genericType;

	// Histogram variables
	// TODO: wrap in class
	bool	_histogramComputed;	// Has the histogram been computed
	bool	_allowHistogram;		// It the computation fails don't allow again
	int		_numBuckets;
    
	// TODO: where is this memory being freed?
	double  *_padfHistogramMin;		// minimum histogram value for each band
    double  *_padfHistogramMax;		// maximum histogram value for each band
	int     **_papanLUTs;			// histogram lookout table for each band
	
	int _nBands;					// No of bands in the image

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

	bool _hasColorTable;		// The image has a color table	

	//world coordinate related variables
	double _dY;				// change in Y (for size of cell or pixel)
	double _dX;				// change in X (for size of cell or pixel)
	double _yllCenter;		// Y coordinate of lower left corner of image (world coordinate)
	double _xllCenter;		// X coordinate of lower left corner of image (world coordinate)	
	long _width;
	long _height;

	bool _useHistogram;		// Use histogram equalization
	bool _hasTransparency;

	// original size and position
	double _origXllCenter;
	double _origYllCenter;
	double _origDx;
	double _origDy;
	long _origWidth;
	long _origHeight;

	GDALPaletteInterp _paletteInterp; // palette interpretation
	bool _allowHillshade;	// if false changes ColorType from Hillshade to gradient

	bool _warped;	// a warped dataset was created on opening
	int _imageQuality;		// Set the image quality 0-100
	tkGridRendering _allowAsGrid;		// Allow the image to be read as grid (if appropriate)

	int _activeBandIndex;
	bool _forceSingleBandRendering;

	bool clearGDALCache;
	colort _transColor;		// transparent color
	CRect _visibleRect;		// indices of pixels of image that are visible at least partially
	ICallback * _callback;

private:
	bool OpenCore(CStringA& filename, GDALAccess accessType = GA_ReadOnly);
	bool ReadBandData(colour ** ImageData, int xOffset, int yOffset, int width, int height, int xBuff, int yBuff);
	IGridColorScheme* GetColorSchemeForRendering();		// returns either of the 2 available
	void ComputeBandMinMax();
	void GDALColorEntry2Colour(int band, double colorValue, double shift, double range, double noDataValue, const GDALColorEntry * poCE, bool useHistogram, colour* result);
	bool ComputeEqualizationLUTs(CStringW filename, double **ppadfScaleMin, double **ppadfScaleMax, int ***ppapanLUTs);
	const BreakVal* FindBreak(const std::vector<BreakVal> & bvals, double val) const;

	template <typename T>
	bool GdalBufferToMemoryBuffer(colour ** ImageData, T* data, int xBuff, int yBuff, int band, double shift, double range, double noDataValue /*, const GDALColorEntry * poCE*/, bool useHistogram);
	template <typename DataType>
	bool ReadBandDataAsGrid(colour** ImageData, int xOff, int yOff, int width, int height, int xBuff, int yBuff, bool setRGBToGrey); 
	template <typename DataType>
	bool ReadBandDataAsGridCore(colour** ImageData, int xOff, int yOff, int width, int height, int xBuff, int yBuff, bool setRGBToGrey); 
	
	bool ReadColorTableToMemoryBuffer(colour ** imageData, int* srcDataInt, int bandIndex, int xBuff, int yBuff, double noDataValue, double shift, double range);

public:
	// properties
	GDALDataset* GetDataset(){ return _rasterDataset; }
	GDALRasterBand* GetBand(int bandIndex);
	IGridColorScheme* GetCustomColorScheme() { return _customColorScheme; }
	GDALPaletteInterp GetPaletteInterpretation() { return _palleteInterpretation; }
	PredefinedColorScheme GetDefaultColors() { return _predefinedColors; }

	CRect GetVisibleRect() { return _visibleRect; }
	colort GetTransparentColor() { return _transColor; }

	int GetActiveBandIndex() { return _activeBandIndex; }
	bool SetActiveBandIndex(int bandIndex);

	double GetDX() { return _dX; }
	double GetDY() { return _dY; }
	
	double GetOrigDx() { return _origDx; }
	void SetOrigDx(double value) { _origDx = value; }
	
	double GetOrigDy() { return _origDy; }
	void SetOrigDy(double value) { _origDy = value; }

	double GetXllCenter() { return _xllCenter; }
	double GetYllCenter() { return _yllCenter; }

	double GetOrigXllCenter() { return _origXllCenter; }
	void SetOrigXllCenter(double value) { _origXllCenter = value; }

	double GetOrigYllCenter() { return _origYllCenter; }
	void SetOrigYllCenter(double value) { _origYllCenter = value; }
	
	long GetWidth(){ return _width; }
	long GetHeight(){ return _height; }
	long GetOrigWidth() { return _origWidth; }
	long GetOrigHeight() { return _origHeight; }

	int GetNoBands(){ return _nBands; };
	bool GetHasColorTable() { return _hasColorTable; }
	bool GetUseHistogram() { return _useHistogram; }
	void SetUseHistogram(bool value) { _useHistogram = value; }
	bool HasTransparency() { return _hasTransparency; }
	bool IsWarped() { return _warped; }

	int GetImageQuality() { return _imageQuality; }
	void SetImageQuality(int value) { _imageQuality = value; }

	tkGridRendering GetAllowAsGrid() { return _allowAsGrid; }
	void SetAllowAsGrid(tkGridRendering value) { _allowAsGrid = value; }

	bool GetAllowHillshade() { return _allowHillshade; }
	void SetAllowHillshade(bool value) { _allowHillshade = value; }

	bool GetClearGdalCache() { return clearGDALCache; }
	void SetClearGdalCache(bool value) { clearGDALCache = value; }
	
	void SetCallback(ICallback* value) { _callback = value;}

	int GetRgbBandIndex(BandChannel color);
	void SetRgbBandIndex(BandChannel color, int bandIndex);

	bool GetUseRgbBandMapping() { return _useRgbBandMapping; }
	void SetUseRgbBandMapping(bool value) {_useRgbBandMapping = value; }

	bool GetForceSingleBandRendering() { return _forceSingleBandRendering; }
	void SetForceSingleBandRendering(bool value) { _forceSingleBandRendering = value; }

	// methods
	bool SetNoDataValue(double Value);
	void ApplyCustomColorScheme(IGridColorScheme * scheme) ;
	void ApplyPredefinedColorScheme(PredefinedColorScheme colorScheme);

	bool Open(CStringW filename, GDALAccess accessType = GA_ReadOnly); 
	bool LoadBuffer(colour ** ImageData, Extent& screenExtents, CStringW filename, tkInterpolationMode downsamplingMode, bool setRGBToGrey, double mapUnitsPerScreenPixel);
	bool LoadBufferFull(colour** ImageData, CStringW filename, double maxBufferSize = 50);
	
	bool CanUseExternalColorScheme();
	bool WillBeRenderedAsGrid();
	void RefreshExtents();
	int Dereference();
	void Close();
	bool IsRgb();

	void UpdateVisibleExtents(double minX, double minY, double maxX, double maxY);
	void TryDecreaseBufferSize(tkInterpolationMode downsamplingMode, double mapUnitsPerScreenPixel, int& xBuff, int& yBuff);
	void SetEmptyBuffer(colour ** ImageData);
	bool ReopenDatasetIfNeeded(CStringW filename);
	void ApplyBufferQuality(int& xBuff, int& yBuff);
	void CompuateHistogram(CStringW filename);
	bool ReadDataGeneric(colour ** ImageData, int& xBuff, int& yBuff, bool setToGrey);
	bool ReadGeoTransform();
	void OpenDefaultBands();
	void InitNoDataValue();
	bool InitDataType();
	void InitSettings();
	GDALRasterBand* GetDefaultRgbBand(int bandIndex);
	GDALRasterBand* GetMappedRgbBand(int bandIndex);
	GDALDataType GetSimplifiedDataType(GDALRasterBand* band);
	bool NeedsGridRendering();
};

