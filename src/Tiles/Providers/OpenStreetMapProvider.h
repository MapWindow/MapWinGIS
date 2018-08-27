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
#include "BaseProvider.h"

// *******************************************************
//			OpenStreetMapProvider
// *******************************************************
class OpenStreetMapProvider : public BaseProvider
{
public:
	int count;

	OpenStreetMapProvider()
	{
		count = 0;
		Id = tkTileProvider::OpenStreetMap;
		Name = "OpenStreetMap";
		_copyright.Format(L"© OpenStreetMap contributors");
		this->_projection = new MercatorProjection();
		_subProviders.push_back(this);
		_serverLetters = "abc";
		_refererUrl = "https://www.openstreetmap.org/";
		_urlFormat = "https://%c.tile.openstreetmap.org/%d/%d/%d.png";
		_maxZoom = 19;
		_licenseUrl = "https://operations.osmfoundation.org/policies/tiles/";
	}

	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		char letter = _serverLetters[count % 3];
		count++;
		CString s;
		s.Format(_urlFormat, letter, zoom, pos.x, pos.y);
		return s;
	}
};

class OpenCycleMapProvider : public OpenStreetMapProvider
{
public:
	OpenCycleMapProvider()
	{
		Id = tkTileProvider::OpenCycleMap;
		Name = "OpenCycleMap";
		_refererUrl = "http://www.opencyclemap.org/";
		_licenseUrl = "http://www.opencyclemap.org/docs/";
		_urlFormat = "https://%c.tile.opencyclemap.org/cycle/%d/%d/%d.png";
	}
};

class OpenTransportMapProvider : public OpenStreetMapProvider
{
public:
	OpenTransportMapProvider()
	{
		Id = tkTileProvider::OpenTransportMap;
		Name = "OpenTransportMap";
		_refererUrl = "http://www.opencyclemap.org/";
		_urlFormat = "https://%c.tile2.opencyclemap.org/transport/%d/%d/%d.png";
	}
};


class OpenHumanitarianMapProvider : public OpenStreetMapProvider
{
public:
	OpenHumanitarianMapProvider()
	{
		Id = tkTileProvider::OpenHumanitarianMap;
		Name = "OpenHumanitarianMap";
		_refererUrl = "https://www.openstreetmap.org/";
		_urlFormat = "https://%c.tile.openstreetmap.fr/hot/%d/%d/%d.png";
	}
};


class MapQuestProvider : public OpenStreetMapProvider
{
public:
	MapQuestProvider()
	{
		Id = tkTileProvider::MapQuestAerial;
		Name = "MapQuest Aerial";
		_refererUrl = "http://www.mapquest.com/";
		_licenseUrl = "http://developer.mapquest.com/web/products/open/map";
		_urlFormat = "http://otile%c.mqcdn.com/tiles/1.0.0/sat/%d/%d/%d.png";
		_serverLetters = "1234";
		_copyright = "Tiles Courtesy of MapQuest";
	}
};
