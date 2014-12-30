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

class OpenStreetMapProvider: public BaseProvider
{
public:
	~OpenStreetMapProvider(void){};
	int count;

	OpenStreetMapProvider() 
	{
		count = 0;
		Id = tkTileProvider::OpenStreetMap;
		Name = "OpenStreetMap";
		Copyright.Format(L"© OpenStreetMap contributors");
		this->Projection = new MercatorProjection();
		subProviders.push_back(this);
		ServerLetters = "abc";
		RefererUrl = "http://www.openstreetmap.org/";
		UrlFormat = "http://%c.tile.openstreetmap.org/%d/%d/%d.png";
		
		maxZoom = 19;

		//_clientLock.Lock();
		//for(int i = 0; i < 3; i++)
		//{
		//	CString url;
		//	url.Format("http://%c.tile.openstreetmap.org/", ServerLetters[i]);
		//	for(int j = 0; j < 2; j++) {
		//		MyHttpClient* client = new MyHttpClient();
		//		//client->SetDefaultUrl(url);
		//		_httpClients.push_back(client);
		//	}
		//}
		//_clientLock.Unlock();
	}

	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		//char letter = ServerLetters[GetServerNum(pos, 3)];
		char letter = ServerLetters[count % 3];
		count++;
		CString s; 
		s.Format(UrlFormat, letter, zoom, pos.x, pos.y);
		return s;
	}
};

class OpenCycleMapProvider: public OpenStreetMapProvider
{
public:	
	OpenCycleMapProvider() 
	{
		Id = tkTileProvider::OpenCycleMap;
		Name = "OpenCycleMap";
		RefererUrl = "http://www.opencyclemap.org/";
		UrlFormat = "http://%c.tile.opencyclemap.org/cycle/%d/%d/%d.png";
	}
};

class OpenTransportMapProvider: public OpenStreetMapProvider
{
public:	
	OpenTransportMapProvider() 
	{
		Id = tkTileProvider::OpenTransportMap;
		Name = "OpenTransportMap";
		RefererUrl = "http://www.opencyclemap.org/";
		UrlFormat = "http://%c.tile2.opencyclemap.org/transport/%d/%d/%d.png";
	}
};


class OpenHumanitarianMapProvider: public OpenStreetMapProvider
{
public:	
	OpenHumanitarianMapProvider() 
	{
		Id = tkTileProvider::OpenHumanitarianMap;
		Name = "OpenHumanitarianMap";
		RefererUrl = "http://www.openstreetmap.org/";
		UrlFormat = "http://%c.tile.openstreetmap.fr/hot/%d/%d/%d.png";
	}
};


class MapQuestProvider : public OpenStreetMapProvider
{
public:
	MapQuestProvider()
	{
		Id = tkTileProvider::MapQuestAerial;
		Name = "MapQuest";
		RefererUrl = "http://www.mapquest.com/";
		UrlFormat = "http://otile%c.mqcdn.com/tiles/1.0.0/sat/%d/%d/%d.png";
		ServerLetters = "1234";
		Copyright = "Tiles Courtesy of MapQuest";
	}
};
