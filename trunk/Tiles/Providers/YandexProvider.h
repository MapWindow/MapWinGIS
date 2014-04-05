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
#include "MercatorProjectionYandex.h"

class YandexBaseProvider: public BaseProvider
{
protected:
	CString UrlServer;
public:
	YandexBaseProvider() 
	{
		RefererUrl = "http://maps.yandex.ru/";
		Copyright = "";
		this->Projection = new MercatorProjectionYandex();
	}
	
	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		CString s;
		s.Format(UrlFormat, UrlServer, GetServerNum(pos, 4) + 1, Version, pos.x, pos.y, zoom);
		return s;
	}

	void Initialize();	// overrides method of the BaseProvider
};

class YandexMapProvider: public YandexBaseProvider
{

public:
	YandexMapProvider() 
	{
		Version = "4.13.1";
		UrlServer = "vec";
		Id = tkTileProvider::YandexMaps;
		Name = "YandexMaps";
		UrlFormat = "http://%s0%d.maps.yandex.net/tiles?l=map&v=%s&x=%d&y=%d&z=%d";
		subProviders.push_back(this);		
	}
};

class YandexSatelliteProvider: public YandexBaseProvider
{
public:
	YandexSatelliteProvider() 
	{
		UrlServer = "sat";
		Version = "3.154.0";
		Id = tkTileProvider::YandexSatellite;
		Name = "YandexSatellite";
		UrlFormat = "http://%s0%d.maps.yandex.net/tiles?l=sat&v=%s&x=%d&y=%d&z=%d";		
		subProviders.push_back(this);
	}
};

class YandexHybridProvider: public YandexBaseProvider
{
public:
	YandexHybridProvider(CTileProviders* list) 
	{
		UrlServer = "vec";
		Version = "4.13.1";
		Id = tkTileProvider::YandexHybrid;
		Name = "YandexHybrid";
		UrlFormat = "http://%s0%d.maps.yandex.net/tiles?l=skl&v=%s&x=%d&y=%d&z=%d";		
		subProviders.push_back(list->get_Provider(tkTileProvider::YandexSatellite));
		subProviders.push_back(this);
	}
};



