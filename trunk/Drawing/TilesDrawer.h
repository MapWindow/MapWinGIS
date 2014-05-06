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
 // lsu 17 apr 2012 - created the file

#pragma once
#include "basedrawer.h"
#include "Tiles.h"

class CTilesDrawer : public CBaseDrawer
{
public:
	// old version for GDI drawing
	CTilesDrawer(CDC* dc, Extent* extents, double pixelPerProjectionX, double pixelPerProjectionY)
	{
		_dc = dc;
		_extents = extents;
		_pixelPerProjectionX = pixelPerProjectionX;
		_pixelPerProjectionY = pixelPerProjectionY;
		m_transfomation = NULL;
		m_graphics = NULL;
	};
	// newer one for GDI+ drawing
	CTilesDrawer(Gdiplus::Graphics* g, Extent* extents, double pixelPerProjectionX, double pixelPerProjectionY)
	{
		_dc = NULL;
		m_graphics = g;
		_extents = extents;
		_pixelPerProjectionX = pixelPerProjectionX;
		_pixelPerProjectionY = pixelPerProjectionY;
		m_transfomation = NULL;
	};
	~CTilesDrawer(void){};

	void DrawTiles(ITiles* tiles, double pixelsPerDegree, IGeoProjection* mapProjection, BaseProjection* tileProjection, bool printing, int projectionChangeCount);

	IGeoProjection* m_transfomation;
	Gdiplus::Graphics* m_graphics;
};
