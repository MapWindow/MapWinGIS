//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
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
//#if _MSC_VER > 1000
//#endif // _MSC_VER > 1000
//#pragma pack(push, gif2, 1)

#pragma once
#include "MapWinGis.h"
#include <deque>
#include "gdal_priv.h"

#include "colour.h"
#include "ImageStructs.h"

#include "ErrorCodes.h"
#include "Extent.h"

class tkRaster
{
public:
	//Constructor
    tkRaster()
	{	//Rob Cairns
		rasterDataset=NULL;
		transColor = RGB(0,0,0);
		gridColorScheme = NULL;
		poBandR = NULL;
		poBandG = NULL;
		poBandB = NULL;
		poCT = NULL;
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
		m_clearGDALCache = false;
		warped = false;
	} 

private:	
	struct BreakVal
	{	
		double lowVal;
		double highVal;
	};

	enum HandleImage
	{ 
		asRGB=0,
		asGrid=1,
		asComplex=2
	};

	// ----------------------------------------------------
	//	 Members
	// ----------------------------------------------------
public:	
	//world coordinate related variables
	long width, height;
	double dY;				// change in Y (for size of cell or pixel)
	double dX;				// change in X (for size of cell or pixel)
	double YllCenter;		// Y coordinate of lower left corner of image (world coordinate)
	double XllCenter;		// X coordinate of lower left corner of image (world coordinate)

	Extent _extents;		// full extents in map coordinates
	Extent _visibleExtents;	// visible extents in map coordinates
	CRect _visibleRect;		// indices of pixels of image that are visible at least partially
	
	BSTR key;
	bool hasTransparency;
	float pctTrans;
	colort transColor;		// transparent color
	ICallback * cBack;

	PredefinedColorScheme imageColorScheme; //Image color scheme if read as grid
	bool allowAsGrid;		// Allow the image to be read as grid (if appropriate)
	int imageQuality;		// Set the image quality 0-100
	bool m_clearGDALCache;
	bool hasColorTable;		// The image has a color table	
	GDALPaletteInterp paletteInterp; //Pallette interpretation
	bool	useHistogram;	// Use histogram equalization
	
	ImageType ImgType;
	
	// original size and position
	double orig_XllCenter;
	double orig_YllCenter;
	double orig_dX;
	double orig_dY;
	long orig_Width;
	long orig_Height;
	bool warped;	// a warped dataset was created on opening
	

private:
	double dfMin;
	double dfMax;
	double adfMinMax[2];	// holds the min and max values for grid images
	
	CString fileName;
	GDALDataset * rasterDataset;
	GDALRasterBand * poBandR;
	GDALRasterBand * poBandB;
	GDALRasterBand * poBandG;

	HandleImage handleImage;
	GDALColorTable * poCT;
	GDALPaletteInterp cPI;
	GDALColorInterp cInterp;
	GDALDataType genericType;
	bool gridColorIncoming;
	PredefinedColorScheme imgColor;

	//Histogram variables
	bool	histogramComputed;	// Has the histogram been computed
	bool	allowHistogram;		// It the computation fails don't allow again
	int		nLUTBins;
    double  *padfScaleMin;
    double  *padfScaleMax;
    int     **papanLUTs;
	int nBands;					// No of bands in the image
	GDALDataType dataType;
	
	const char* ciName;			// Color interpretation name
	int buffSize;
	int ColorMapSize;
	ColorEntry ColorMap[4096];
	IGridColorScheme * gridColorScheme;

	// ---------------------------------------------
	//   Methods
	// ---------------------------------------------
public:
	bool LoadRaster(const char * filename, GDALAccess accessType = GA_ReadOnly); 
	bool LoadBuffer(colour** ImageData, double MinX, double MinY, double MaxX, double MaxY, const char * filename, tkInterpolationMode downsamplingMode, bool setRGBToGrey, double mapUnitsPerScreenPixel);
	bool LoadBuffer2(colour** ImageData, Extent& extent, const char * filename, tkInterpolationMode downsamplingMode, bool setRGBToGrey, double mapUnitsPerScreenPixel);
    bool LoadBufferFull(colour** ImageData, const char * filename, double maxBufferSize = 50);
	void tkRaster::RefreshExtents();
	bool SetNoDataValue(double Value);

	void Close();
	int Dereference();
	
	// Exposing properties
	void ApplyGridColorScheme(IGridColorScheme * scheme);
	IGridColorScheme* get_GridColorScheme();
	
	GDALDataset* tkRaster::get_Dataset()	{return rasterDataset;}
	bool get_HasColorTable(){return hasColorTable;};
	GDALPaletteInterp get_PaletteInterpretation(){return cPI;};
	int get_NoBands(){return nBands;};
	
	double getDX() {return dX;} 
	double getDY() {return dY;}
	double getYllCenter() {return YllCenter;}
	double getXllCenter() {return XllCenter;}
	long getWidth(){return width;}
	long getHeight(){return height;}

	GDALRasterBand* get_RasterBand(int BandIndex)
	{
		if (BandIndex == 1) return poBandR;
		else if (BandIndex == 1) return poBandG;
		else if (BandIndex == 1) return poBandB;
		else return NULL;
	}
private:
	bool ReadImage(colour ** ImageData, int xOffset, int yOffset, int width, int height, int xBuff, int yBuff);
	void GDALColorEntry2Colour(int band, double colorValue, double shift, double range, double noDataValue, const GDALColorEntry * poCE, bool useHistogram, colour* result);
	template <typename T> 
	bool AddToBufferAlt(colour ** ImageData, T* data, int xBuff, int yBuff,	int band, double shift, double range, double noDataValue, const GDALColorEntry * poCE, bool useHistogram);
	bool ReadGridAsImage(colour ** ImageData, int xOff, int yOff, int width, int height, int xBuff, int yBuff, bool setRGBToGrey); // double dx, double dy, bool setRGBToGrey, PredefinedColorScheme imageColorScheme,bool clearGDALCache);
	long findBreak( std::deque<BreakVal> & bvals, double val );
	bool ComputeEqualizationLUTs( const char * filename, double **ppadfScaleMin, double **ppadfScaleMax, int ***ppapanLUTs);
};

