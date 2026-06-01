/**************************************************************************************
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/ 
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */
 #pragma once
#include "basedrawer.h"
#include "TileManager.h"

static int _tileDrawCount;

class TilesDrawer : public CBaseDrawer
{
public:
	TilesDrawer(Gdiplus::Graphics* g, Extent* extents, double pixelPerProjectionX, double pixelPerProjectionY, 
				double pixelsPerMapUnit, IGeoProjection* transform)
		: _graphics(g), _transfomation(transform)
	{
		_dc = NULL;
#if LOG_TILE_DRAWING
		_pTileLog = nullptr;
#endif
		_tileDrawCount = 0;
		_extents = extents;
		_pixelPerProjectionX = pixelPerProjectionX;
		_pixelPerProjectionY = pixelPerProjectionY;
		_pixelPerMapUnit = pixelsPerMapUnit;
	};

	virtual ~TilesDrawer(void){};

private:
	Gdiplus::Graphics* _graphics;
	IGeoProjection* _transfomation;
	double _pixelPerMapUnit;
#if LOG_TILE_DRAWING
	std::ofstream* _pTileLog;
#endif
public:
	// properties
	IGeoProjection* get_Transform() { return _transfomation; }		// WGS84 to map transformation
	int GetTileDrawCount() { return _tileDrawCount++; }

private:
	bool IsSameProjection(IGeoProjection* mapProjection, BaseProvider* provider);
	bool UpdateTileBounds(TileCore* tile, bool isSameProjection, int projectionChangeCount);
	void DrawGrid(TileCore* tile, Gdiplus::RectF& screenRect);
#if SQUARE_TILES
	void DrawOverlays(TileCore* tile, int minTileY, int maxTileY, Gdiplus::RectF screenBounds, Gdiplus::ImageAttributes& attr);
#else
	void DrawOverlays(TileCore* tile, Gdiplus::RectF screenBounds, Gdiplus::ImageAttributes& attr);
#endif
	bool CalculateScreenBounds(TileCore* tile, Gdiplus::RectF& screenBounds);
	void DrawGridText(TileCore* tile, Gdiplus::RectF& screenRect);
	void InitImageAttributes(TileManager* manager, Gdiplus::ImageAttributes& attr);
	void DrawWmsBounds(BaseProvider* provider);
	void DumpTile(TileCore* tile, Gdiplus::Bitmap* bmp);

public:
	// methods
	void DrawTiles(TileManager* manager, IGeoProjection* mapProjection, bool printing, int projectionChangeCount);
	
};
