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

class GoogleBaseProvider: public BaseProvider
{
protected:
	CString server;
    CString UrlFormatServer;
    CString UrlFormatRequest;
	CString SecureWord;
	CString Sec1;
public:
	GoogleBaseProvider() 
	{
		SecureWord = "Galileo";
		Sec1 = "&s=";
		server = "google.com";
		RefererUrl.Format("http://maps.%s/", server);
		int year = Utility::GetCurrentYear();
		Copyright.Format("©%d Google - Map data ©%d Tele Atlas, Imagery ©%d TerraMetrics", year, year, year);
		this->Projection = new MercatorProjection();
		this->maxZoom = 20;
	}
	
	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		CString sec1 = "";
        CString sec2 = "";
        GetSecureWords(pos, sec1, sec2);
		
		CString s;
		s.Format(UrlFormat, UrlFormatServer, GetServerNum(pos, 4), server, UrlFormatRequest, Version, LanguageStr, pos.x, sec1, pos.y, zoom, sec2);
		return s;
	}

	void GetSecureWords(CPoint &pos, CString &sec1, CString &sec2)
	{
		sec1 = "";
		sec2 = "";
		int seclen = ((pos.x * 3) + pos.y) % 8;
		sec2 = SecureWord.Left(seclen);
		if(pos.y >= 10000 && pos.y < 100000)
		{
			sec1 = Sec1;
		}
	}
};

class GoogleMapProvider: public GoogleBaseProvider
{
public:
	GoogleMapProvider() 
	{
		UrlFormatServer = "mt";
		UrlFormatRequest = "vt";	
		Version = "m@283000000";
		Id = tkTileProvider::GoogleMaps;
		Name = "GoogleMaps";
		UrlFormat = "http://%s%d.%s/%s/lyrs=%s&hl=%s&x=%d%s&y=%d&z=%d&s=%s";
		subProviders.push_back(this);
	}
};

class GoogleSatelliteProvider: public GoogleBaseProvider
{
public:
	GoogleSatelliteProvider() 
	{
		UrlFormatServer = "khms";
		UrlFormatRequest = "kh";
		Version = "162";
		Id = tkTileProvider::GoogleSatellite;
		Name = "GoogleSatellite";
		UrlFormat = "http://%s%d.%s/%s/v=%s&hl=%s&x=%d%s&y=%d&z=%d&s=%s";
		subProviders.push_back(this);
	}
};

class GoogleHybridProvider: public GoogleBaseProvider
{
public:
	GoogleHybridProvider(CTileProviders* list) 
	{
		UrlFormatServer = "mt";
		UrlFormatRequest = "vt";	
		Version = "h@283000000";
		Id = tkTileProvider::GoogleHybrid;
		Name = "GoogleHybrid";
		UrlFormat = "http://%s%d.%s/%s/lyrs=%s&hl=%s&x=%d%s&y=%d&z=%d&s=%s";
		subProviders.push_back(list->get_Provider(tkTileProvider::GoogleSatellite));
		subProviders.push_back(this);
	}
};

class GoogleTerrainProvider: public GoogleBaseProvider
{
public:
	GoogleTerrainProvider() 
	{
		UrlFormatServer = "mt";
		UrlFormatRequest = "vt";
		Version = "t@162,r@283000000";
		Id = tkTileProvider::GoogleTerrain;
		Name = "GoogleTerrain";
		UrlFormat = "http://%s%d.%s/%s/v=%s&hl=%s&x=%d%s&y=%d&z=%d&s=%s";
		subProviders.push_back(this);
	}
};
