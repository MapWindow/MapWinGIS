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
 #include "stdafx.h"
#include "TilesDrawer.h"
#include "TileHelper.h"
#include "CustomProjection.h"

using namespace Gdiplus;

// ***************************************************************
//		DrawTiles()
// ***************************************************************
void TilesDrawer::DrawTiles( TileManager* manager, double pixelsPerMapUnit, IGeoProjection* mapProjection, bool printing, int projectionChangeCount )
{
	if (!manager) return;

	BaseProvider* provider = manager->get_Provider();
	if (!provider) return;

	bool drawGrid = manager->get_GridLinesVisible();
	
	ImageAttributes attr;
	attr.SetWrapMode(WrapModeTileFlipXY);

	bool isSame = IsSameProjection(mapProjection, provider);
	
	// copy to temporary vector, for not lock the original one for the whole length of drawing	
	std::vector<TileCore*> tiles;
	manager->CopyBuffer(tiles);

	// per tile drawing
	for (size_t i = 0; i < tiles.size();i++)
	{
		TileCore* tile = tiles[i];

		if (!tile->isDrawn() || printing)
		{
			UpdateTileBounds(tile, isSame ? true : false, projectionChangeCount);

			RectF screenBounds;
			if (!CalculateScreenBounds(tile, pixelsPerMapUnit, screenBounds)) {
				continue;
			}

			DrawOverlays(tile, screenBounds, attr);

			if (drawGrid) {
				DrawGrid(tile, screenBounds);
			}

			tile->isDrawn(true);	
		}
	}
}

// ***************************************************************
//		DrawOverlays()
// ***************************************************************
void TilesDrawer::DrawOverlays(TileCore* tile, RectF screenBounds, ImageAttributes& attr)
{
	for (size_t i = 0; i < tile->Overlays.size(); i++)
	{
		Bitmap* bmp = tile->get_Bitmap(i)->m_bitmap;
		if (bmp)
		{
			Status status;

			double ROUNDING_TOLERANCE = 0.1;

			if (abs(screenBounds.Width - bmp->GetWidth()) < ROUNDING_TOLERANCE &&
				abs(screenBounds.Height - bmp->GetHeight()) < ROUNDING_TOLERANCE)
			{
				// TODO: better to check that all tiles have the same size and apply this rendering only then
				status = _graphics->DrawImage(bmp, Utility::Rint(screenBounds.X), Utility::Rint(screenBounds.Y));
			}
			else
			{
				status = _graphics->DrawImage(bmp, screenBounds, 0.0f, 0.0f, (REAL)bmp->GetWidth(), (REAL)bmp->GetHeight(), UnitPixel, &attr);
			}

			if (status != Gdiplus::Status::Ok)
			{
				Debug::WriteLine("Failed to draw tile.");
			}
		}
	}
}

// ***************************************************************
//		IsSameProjection()
// ***************************************************************
bool TilesDrawer::IsSameProjection(IGeoProjection* mapProjection, BaseProvider* provider)
{
	// check perhaps map projection is the same as the one for tiles
	// then we don't have to use conversion to WGS84 decimal degrees
	VARIANT_BOOL isSame = VARIANT_FALSE;
	CustomProjection* customProj = NULL;

	if (mapProjection)
	{
		customProj = dynamic_cast<CustomProjection*>(provider->get_Projection());
		if (customProj)
		{
			// TODO: can be cached
			mapProjection->get_IsSame(customProj->get_GeoProjection(), &isSame);
		}
	}

	return isSame ? true : false;
}

// ***************************************************************
//		DrawTiles()
// ***************************************************************
// doing transformation on the first drawing of tile
bool TilesDrawer::UpdateTileBounds(TileCore* tile, bool isSameProjection, int projectionChangeCount)
{
	if (tile->get_ProjectionChangeCount() < projectionChangeCount)
	{
		RectLatLng rect;

		bool transform = TileHelper::Transform(tile, _transfomation, isSameProjection ? true : false, rect);

		if (!transform) {
			return false;
		}

		tile->UpdateProjectedBounds(rect, projectionChangeCount);
	}

	return true;
}

// ***************************************************************
//		TransformToScreen()
// ***************************************************************
bool TilesDrawer::CalculateScreenBounds(TileCore* tile, double pixelsPerMapUnit, RectF& screenBounds)
{
	// convert to screen coordinates
	double x, y;

	RectLatLng* bounds = tile->get_ProjectedBounds();
	ProjectionToPixel(bounds->xLng, bounds->yLat, x, y);

	double width = bounds->WidthLng * pixelsPerMapUnit;
	double height = bounds->HeightLat * pixelsPerMapUnit;

	if (width < 0 || height < 0)
	{
		// there was a problem with transformation (coordinates outside bounds, don't draw it)
		Debug::WriteLine("Negative height on drawing tile.");
		return false;
	}

	screenBounds.X = static_cast<REAL>(x);
	screenBounds.Y = static_cast<REAL>(y);
	screenBounds.Width = static_cast<REAL>(width);
	screenBounds.Height = static_cast<REAL>(height);

	return true;
}

// ***************************************************************
//		DrawGrid()
// ***************************************************************
void TilesDrawer::DrawGrid(TileCore* tile, RectF& screenRect)
{
	// draw grid (debugging)
	Pen pen(Color::Orange, 1.0f);
	pen.SetDashStyle(DashStyleDash);
	_graphics->DrawRectangle(&pen, screenRect);

	// draw indices of tiles (for debugging)
	DrawGridText(tile, screenRect);
}

// ***************************************************************
//		DrawGridText()
// ***************************************************************
void TilesDrawer::DrawGridText(TileCore* tile, RectF& screenRect)
{
	CString str;
	str.Format("x=%d; y=%d", tile->tileX(), tile->tileY());
	WCHAR* wStr = Utility::StringToWideChar(str);
	Font* font = Utility::GetGdiPlusFont("Arial", 14);

	SolidBrush brush(Color::Orange);

	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);

	_graphics->DrawString(wStr, wcslen(wStr), font, screenRect, &format, &brush);

	delete font;
	delete wStr;
}