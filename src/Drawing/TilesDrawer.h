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

class TilesDrawer : public CBaseDrawer
{
public:
	TilesDrawer(Gdiplus::Graphics* g, Extent* extents, double pixelPerProjectionX, double pixelPerProjectionY, IGeoProjection* transform)
		: _graphics(g) , _transfomation(transform)
	{
		_dc = NULL;
		_extents = extents;
		_pixelPerProjectionX = pixelPerProjectionX;
		_pixelPerProjectionY = pixelPerProjectionY;
	};

	virtual ~TilesDrawer(void){};

private:
	Gdiplus::Graphics* _graphics;
	IGeoProjection* _transfomation;

public:
	// properties
	IGeoProjection* get_Transform() { return _transfomation; }		// WGS84 to map transformation

private:
	bool IsSameProjection(IGeoProjection* mapProjection, BaseProvider* provider);
	bool UpdateTileBounds(TileCore* tile, bool isSameProjection, int projectionChangeCount);
	void DrawGrid(TileCore* tile, Gdiplus::RectF& screenRect);
	void DrawOverlays(TileCore* tile, Gdiplus::RectF screenBounds, Gdiplus::ImageAttributes& attr);
	bool CalculateScreenBounds(TileCore* tile, double pixelsPerMapUnit, Gdiplus::RectF& screenBounds);
	void DrawGridText(TileCore* tile, Gdiplus::RectF& screenRect);

public:
	// methods
	// TODO: move pixelPerMapUnit to constructor
	void DrawTiles(TileManager* manager, double pixelsPerMapUnit, IGeoProjection* mapProjection, bool printing, int projectionChangeCount);
};
