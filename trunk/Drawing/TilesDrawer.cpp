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

#include "stdafx.h"
#include "TilesDrawer.h"
#include "Tiles.h"
#include "ImageAttributes.h"
#include "AmersfoortProjection.h"

using namespace Gdiplus;

// ***************************************************************
//		DrawTiles()
// ***************************************************************
void CTilesDrawer::DrawTiles( ITiles* itiles, double pixelsPerMapUnit, IGeoProjection* mapProjection, 
							 BaseProjection* tileProjection, bool printing, int projectionChangeCount )
{
	vector<TileCore*> tiles = ((CTiles*)itiles)->m_tiles;

	VARIANT_BOOL drawGrid;
	itiles->get_GridLinesVisible(&drawGrid);
	
	// to support both GDI and GDI+ drawing
	Graphics* g = m_graphics ? m_graphics : Graphics::FromHDC(_dc->m_hDC);		
	
	CImageAttributesEx attr(1.0, false, false, 0, 0);
	attr.SetWrapMode(Gdiplus::WrapModeTileFlipXY);

	// check perhaps map projection is the same as the one for tiles
	// then we don't have to use conversion to WGS84 decimal degrees
	VARIANT_BOOL isSame;
	CustomProjection* customProj = NULL;
	if (tileProjection && mapProjection)
	{
		customProj = dynamic_cast<CustomProjection*>(tileProjection);
		if (customProj)
		{
			// TODO: can be cached
			mapProjection->get_IsSame(customProj->projCustom, &isSame);
			Debug::WriteLine("Is same projection: %d", isSame);
		}
	}
	
	// copy to temporary vector, for not lock the original one for the whole length of drawing	
	((CTiles*)itiles)->_tilesBufferLock.Lock();
	std::vector<TileCore*> tempTiles(tiles);
	((CTiles*)itiles)->_tilesBufferLock.Unlock();

	// per tile drawing
	for (size_t i = 0; i < tiles.size();i++)
	{
		TileCore* tile = tiles[i];
		if (!tile->m_drawn || printing)
		{
			// doing transformation on the first drawing of tile
			if (tile->m_projectionOk < projectionChangeCount)
			{
				if (isSame)
				{
					// projection for tiles matches map projection (most often it's Google Mercator; EPSG:3857)
					if (!tile->UpdateProjection(customProj, projectionChangeCount))
						continue;
				}
				else
				{
					if (m_transfomation)
					{
						// projection differs from WGS84 decimal degrees
						if (!tile->UpdateProjection(m_transfomation, projectionChangeCount))
							continue;
					}
					else
					{
						// we are working with WGS84 decimal degrees
						tile->Proj.xLng = tile->Geog.xLng;
						tile->Proj.yLat = tile->Geog.yLat;
						tile->Proj.WidthLng = tile->Geog.WidthLng;
						tile->Proj.HeightLat = tile->Geog.HeightLat;
					}
				}
			}
			
			// convert to screen coordinates
			double x, y;
			this->ProjectionToPixel(tile->Proj.xLng, tile->Proj.yLat, x, y);
			double width = tile->Proj.WidthLng * pixelsPerMapUnit;
			double height = tile->Proj.HeightLat * pixelsPerMapUnit;
			
			//Debug::WriteLine("X: %f; y: %f", x, y);
			//Debug::WriteLine("Width: %f; height: %f", width, height);

			Gdiplus::RectF rect((Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)width, (Gdiplus::REAL)height);
			
			// actual drawing
			for (size_t i = 0; i < tile->Overlays.size(); i++)
			{
				Gdiplus::Bitmap* bmp = tile->getBitmap(i)->m_bitmap;
				if (bmp)
				{
					double TOLERANCE = 0.01;
					if ( abs(width - bmp->GetWidth()) < TOLERANCE && 
						 abs(height - bmp->GetHeight()) < TOLERANCE)
					{
						// TODO: perhaps better to check that all tiles have the same size
						// and apply this rendering only then
						g->DrawImage(bmp,  Utility::Rint(x), Utility::Rint(y));
					}
					else
					{
						g->DrawImage(bmp, rect, (Gdiplus::REAL)(-0.0), (Gdiplus::REAL)(-0.0), 
							(Gdiplus::REAL)bmp->GetWidth(), (Gdiplus::REAL)bmp->GetHeight(), Gdiplus::UnitPixel, &attr);
					}
				}
			}
			
			// draw grid (debugging)
			if (drawGrid)
			{
				Gdiplus::Pen pen(Gdiplus::Color::Orange, 1.0f);
				pen.SetDashStyle(Gdiplus::DashStyleDash);
				g->DrawRectangle(&pen, rect);
			}

			// draw indices of tiles (for debugging)
			bool drawText = drawGrid != 0;
			if (drawText)
			{
				CString str;
				str.Format("x=%d; y=%d", tile->m_tileX, tile->m_tileY);
				WCHAR* wStr = Utility::StringToWideChar(str);
				Gdiplus::Font* font = Utility::GetGdiPlusFont("Arial", 14);
				
				Gdiplus::SolidBrush brush(Gdiplus::Color::Orange);
				Gdiplus::RectF r((Gdiplus::REAL)rect.X, (Gdiplus::REAL)rect.Y, (Gdiplus::REAL)rect.Width, (Gdiplus::REAL)rect.Height);
				
				Gdiplus::StringFormat format;
				format.SetAlignment(StringAlignmentCenter);
				format.SetLineAlignment(StringAlignmentCenter);

				g->DrawString(wStr, wcslen(wStr), font, r, &format,  &brush);
				
				delete font;
				delete wStr;
			}
			tile->m_drawn = true;	
		}
	}
	if (!m_graphics)
	{
		g->ReleaseHDC(_dc->m_hDC);
		delete g;
	}
}


