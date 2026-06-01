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
#define DEBUG_TILES_DRAWER 1
#include <iomanip>
#include <sys/stat.h>
#include "TilesDrawer.h"
#include "TileHelper.h"
#include "CustomProjection.h"
#include "ImageHelper.h"
#include "WmsCustomProvider.h"

using namespace Gdiplus;

// ***************************************************************
//		DrawTiles()
// ***************************************************************
void TilesDrawer::DrawTiles( TileManager* manager, IGeoProjection* mapProjection, bool printing, int projectionChangeCount )
{
	if (!manager) return;

#if LOG_TILE_DRAWING
	if (_pTileLog != nullptr)
		_pTileLog->close();

	CString fileName;
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	//oss << std::put_time(&tm, "%Y-%m-%d_%H%M%S");
	oss << std::put_time(&tm, "%Y-%m-%d_%H%M%S");
	auto timeStr = oss.str();
	fileName.Empty();
	auto nr = GetTileDrawCount();
	fileName.Format(R"(C:\Temp\tiles\tilesLog_%hs_%d.txt)", timeStr.c_str(), nr);
	struct stat buffer;
	/*auto exist = stat(fileName.GetString(), &buffer) == 0;
	if (exist)
	{
		// if file exists, append to it
		fileName.AppendFormat("_%d.txt", 1);
	}*/

	_pTileLog = new std::ofstream();
	_pTileLog->open(fileName.GetString(), std::ios::out | std::ios::app);
#endif

	BaseProvider* provider = manager->get_Provider();
	if (!provider) return;

	bool drawGrid = manager->get_GridLinesVisible();
	
	ImageAttributes attr;
	InitImageAttributes(manager, attr);

	bool isSame = IsSameProjection(mapProjection, provider);

	// copy to temporary vector, for not lock the original one for the whole length of drawing	
	std::vector<TileCore*> tiles;
	manager->CopyBuffer(tiles);

#if DEBUG_TILES_DRAWER
	CString sOutput;
	sOutput.AppendFormat("TilesDrawer::DrawTiles() isSame: %s, tiles.Count: %llu\r\n", isSame ? "True" : "False", tiles.size());
	::OutputDebugStringA(sOutput.GetBuffer());
#if LOG_TILE_DRAWING
	_pTileLog->write(sOutput.GetString(), sOutput.GetLength());
#endif
#endif
	// per tile drawing

	auto minTileY = 100000;
	auto maxTileY = -100000;
	for (size_t i = 0; i < tiles.size(); i++)
	{
		const auto tileY = tiles[i]->tileY();
		if (tileY < minTileY)
			minTileY = tileY;
		if (tileY > maxTileY)
			maxTileY = tileY;
	}

	for (size_t i = 0; i < tiles.size();i++)
	{
		TileCore* tile = tiles[i];

		if (!tile->isDrawn() || printing)
		{
			UpdateTileBounds(tile, isSame, projectionChangeCount);

			RectF screenBounds;
			if (!CalculateScreenBounds(tile, screenBounds)) {
				continue;
			}

#if SQUARE_TILES
			DrawOverlays(tile, minTileY, maxTileY, screenBounds, attr);
#else
			DrawOverlays(tile, screenBounds, attr);
#endif

			if (drawGrid) {
				DrawGrid(tile, screenBounds);
			}

			tile->isDrawn(true);	
		}
	}

	if (drawGrid)
	{
		DrawWmsBounds(provider);
	}
#if LOG_TILE_DRAWING
	_pTileLog->flush();
	_pTileLog->close();
	_pTileLog = nullptr;
#endif
}

// ***************************************************************
//		InitImageAttributes()
// ***************************************************************
void TilesDrawer::InitImageAttributes(TileManager* manager, ImageAttributes& attr)
{
	attr.SetWrapMode(WrapModeTileFlipXY);

	if (!manager->IsBackground())
	{
		Gdiplus::ColorMatrix m = ImageHelper::CreateMatrix(manager->contrast,
			manager->brightness,
			manager->saturation,
			manager->hue,
			0.0f, RGB(255, 255, 255), false, manager->get_Alpha() / 255.0f);

		attr.SetColorMatrix(&m);

		if (manager->useTransparentColor)
		{
			Gdiplus::Color color(GetRValue(manager->transparentColor),
								 GetGValue(manager->transparentColor),
								 GetBValue(manager->transparentColor));

			attr.SetColorKey(color, color);
		}
	}
}

// ***************************************************************
//		DrawOverlays()
// ***************************************************************
#if SQUARE_TILES
void TilesDrawer::DrawOverlays(TileCore* tile, int minTileY, int maxTileY, RectF screenBounds, ImageAttributes& attr)
#else
void TilesDrawer::DrawOverlays(TileCore* tile, RectF screenBounds, ImageAttributes& attr)
#endif
{
	CString sOutput;
#if DEBUG_TILES_DRAWER
	sOutput.AppendFormat("TilesDrawer::DrawOverlays() tileX: %d screenBounds: %f x %f tile->Overlays.size: %llu \r\n", tile->tileX(), screenBounds.Width, screenBounds.Height, tile->Overlays.size());
	::OutputDebugStringA(sOutput.GetBuffer());
#endif
	for (size_t i = 0; i < tile->Overlays.size(); i++)
	{
		Bitmap* bmp = tile->get_Bitmap(i)->m_bitmap;
		if (bmp)
		{
			// to debug the issue with occasional seams
			// DumpTile

			Status status;

			double ROUNDING_TOLERANCE = 0.1;

			double dx = abs(screenBounds.Width - bmp->GetWidth());
			double dy = abs(screenBounds.Height - bmp->GetHeight());

#if !SQUARE_TILES
			if (dx < ROUNDING_TOLERANCE && dy < ROUNDING_TOLERANCE)
			{
				// TODO: better to check that all tiles have the same size and apply this rendering only then
				status = _graphics->DrawImage(bmp, Utility::Rint(screenBounds.X), Utility::Rint(screenBounds.Y));

#else
			auto aspectRatio = screenBounds.Width / screenBounds.Height; // aspect ratio of the tile size
			// How much the tile size differs from the screen bounds
			auto dxyDiff = abs(dx - dy);

			/*if(i == 0)
			{
				CLSID pngClsid;
				CString encoder = L"image/png";
				USES_CONVERSION;
				Utility::GetEncoderClsid(A2OLE(encoder), &pngClsid);
				bmp->Save(L"c:\\tmp\\tile_bmp.png", &pngClsid, nullptr);
			} */

			//sOutput.Format("DrawOverlays(%d, %d) screenBounds: %f, %f, %f, %f\r\n", tile->tileX(), tile->tileY(), screenBounds.X, screenBounds.Y, screenBounds.Width, screenBounds.Height);
			//::OutputDebugStringA(sOutput.GetBuffer());

			if (dxyDiff > 2.5) // Tow much difference between tile size and screen bounds size (aspect ratio)
			{
				// If the tile size is too different from the screen bounds, we draw it with only the top-left corner to prevent stretching of the image
				// TODO: better to check that all tiles have the same size and apply this rendering only then
				//status = _graphics->DrawImage(bmp, Utility::Rint(screenBounds.X), Utility::Rint(screenBounds.Y));

				auto diff = screenBounds.Height - screenBounds.Width;
				auto srcy = 0.0f;
				const auto tilesDown = maxTileY - tile->tileY();
				const auto tilesUpp = tile->tileY() - minTileY;

				if (aspectRatio < 1)
				{
					screenBounds.Height = screenBounds.Width;
					//screenBounds.Y += diff;
					//screenBounds.Y += diff / 2.0f; // center the image vertically

					if (tile->tileX() == 234 && tile->tileY() == 28)
						::OutputDebugStringA("\r\n");

					//if (tilesDown > 0)
#if RELEASE_MODE
					screenBounds.Y += (diff / 2.0) * (tilesUpp + 1);
#endif
					//screenBounds.Y += (diff / 1.0) * (tilesDown - 1);

					CLSID pngClsid;
					const CString encoder = L"image/png";
					USES_CONVERSION;
					Utility::GetEncoderClsid(A2OLE(encoder), &pngClsid);
					CStringW fileName;
					fileName.Format(L"C:\\tmp\\tiles\\live\\tile_bmp_%d_%d.png", tile->tileX(), tile->tileY());
					bmp->Save(fileName.GetBuffer(), &pngClsid, nullptr);

#if DEBUG_TILES_DRAWER
					//DumpTile(tile, bmp);
					sOutput.Format("DrawImage(%d, %d) screenBounds.Height: %f, bmp.Height: %u, tilesDown: %d, tilesUpp: %d\r\n", tile->tileX(), tile->tileY(), screenBounds.Height, bmp->GetHeight(), tilesDown, tilesUpp);
#if LOG_TILE_DRAWING
					if (_pTileLog != nullptr)
					{
						_pTileLog->write(sOutput.GetString(), sOutput.GetLength());
						_pTileLog->flush();
					}
#endif
					::OutputDebugStringA(sOutput.GetBuffer());
#endif
				}
				else
				{
					::OutputDebugStringA("aspectRatio >= 1 ");
					//screenBounds.Width = screenBounds.Height;
				}

				//sOutput.Format("DrawImage() screenBounds.Height: %f, bmp.Height: %u, diff: %f\r\n", screenBounds.Height, bmp->GetHeight(), diff);
				//::OutputDebugStringA(sOutput.GetBuffer());
				//screenBounds.Width = bmp->GetWidth();
				//screenBounds.Height = bmp->GetHeight();
				//if (tilesUpp == 0)
					status = _graphics->DrawImage(bmp, screenBounds, 0.0f, srcy, (REAL)bmp->GetWidth(), (REAL)bmp->GetHeight(), UnitPixel, &attr);

#endif



#if DEBUG_TILES_DRAWER
				sOutput.Format("DrawImage(%f, %f, %f, %f, %d, %d) \r\n", screenBounds.X, screenBounds.Y, screenBounds.Width, screenBounds.Height, bmp->GetWidth(), bmp->GetHeight());
				::OutputDebugStringA(sOutput.GetBuffer());
#endif
			}
			else
			{
				DumpTile(tile, bmp);
				status = _graphics->DrawImage(bmp, screenBounds, 0.0f, 0.0f, (REAL)bmp->GetWidth(), (REAL)bmp->GetHeight(), UnitPixel, &attr);
#if DEBUG_TILES_DRAWER
				if (dxyDiff > 2.5)
				{
					sOutput.Format("DrawImage(%f, %f, %f, %f, %d, %d) \r\n", screenBounds.X, screenBounds.Y, screenBounds.Width, screenBounds.Height, bmp->GetWidth(), bmp->GetHeight());
					::OutputDebugStringA(sOutput.GetBuffer());
				}
#endif
			}

			if (status != Gdiplus::Status::Ok)
			{
				Debug::WriteLine("Failed to draw tile.");
			}
		}
		else
			::OutputDebugStringA("TilesDrawer::DrawOverlays() bmp is null \r\n");
	}
}

// ***************************************************************
//		DumpTile()
// ***************************************************************
void TilesDrawer::DumpTile(TileCore* tile, Bitmap* bmp)
{
	//if (tile->tileX() == 0)
	{
		CLSID clsid;
		Utility::GetEncoderClsid(L"image/png", &clsid);
		CStringW sFileName;
		sFileName.Format(L"C:\\Temp\\tiles\\tile_%d_%d.png", tile->tileX(), tile->tileY());
		bmp->Save(sFileName, &clsid, nullptr);
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
			mapProjection->get_IsSame(customProj->get_ServerProjection(), &isSame);
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
//		CalculateScreenBounds()
// ***************************************************************
bool TilesDrawer::CalculateScreenBounds(TileCore* tile, RectF& screenBounds)
{
	// convert to screen coordinates
	double x, y;

	RectLatLng* bounds = tile->get_ProjectedBounds();
	ProjectionToPixel(bounds->xLng, bounds->yLat, x, y);

	double width = bounds->WidthLng * _pixelPerMapUnit;
	double height = bounds->HeightLat * _pixelPerMapUnit;

	if (width < 0 || height < 0)
	{
		// there was a problem with transformation (coordinates outside bounds, don't draw it)
		Debug::WriteLine("Negative height on drawing tile.");
		return false;
	}

#if DEBUG_TILES_DRAWER
	CString text;
	text.Format("CalculateScreenBounds(%d, %d) %f, %f, %f, %f\r\n", tile->tileX(), tile->tileY(), bounds->xLng, bounds->yLat, bounds->WidthLng, bounds->HeightLat);
#if LOG_TILE_DRAWING
	if (_pTileLog != nullptr)
	{
		_pTileLog->write(text.GetString(), text.GetLength());
		_pTileLog->flush();
	}
#endif
	::OutputDebugStringA(text.GetBuffer());
#endif

	screenBounds.X = static_cast<REAL>(x);
	screenBounds.Y = static_cast<REAL>(y);
	screenBounds.Width = static_cast<REAL>(width);
	screenBounds.Height = static_cast<REAL>(height);
	//screenBounds.Height = static_cast<REAL>(width);

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

// ***************************************************************
//		DrawWmsBounds()
// ***************************************************************
void TilesDrawer::DrawWmsBounds(BaseProvider* provider)
{
	double xMin, yMin, xMax, yMax;

	BaseProjection* pr = provider->get_Projection();
	yMin = pr->get_MinLat();
	yMax = pr->get_MaxLat();
	xMin = pr->get_MinLong();
	xMax = pr->get_MaxLong();

	if (_transfomation)
	{
		VARIANT_BOOL vb1, vb2;
		_transfomation->Transform(&xMin, &yMin, &vb1);
		_transfomation->Transform(&xMax, &yMax, &vb2);
	}

	double pix, piy, pix2, piy2;
	ProjectionToPixel(xMin, yMin, pix, piy);
	ProjectionToPixel(xMax, yMax, pix2, piy2);

	Gdiplus::RectF rect((REAL)MIN(pix, pix2), (REAL)MIN(piy, piy2), (REAL)abs(pix2 - pix), (REAL)abs(piy2 - piy));

	Pen pen(Color::Orange, 3.0f);
	_graphics->DrawRectangle(&pen, rect);
}