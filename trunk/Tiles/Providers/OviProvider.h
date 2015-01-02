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

class OviBaseProvider: public BaseProvider
{
private:	
	CString UrlServerLetters;
public:

	OviBaseProvider() 
	{
		RefererUrl = "https://www.here.com/";
        int year = Utility::GetCurrentYear();
	    LicenseUrl = "https://developer.here.com/get-started#/";
		Copyright.Format(L"©%d Here, DigitalGlobe", year);
		UrlServerLetters = "1234";
		this->Projection = new MercatorProjection();
		subProviders.push_back(this);
	}
	
	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		CString s;
		s.Format(UrlFormat, UrlServerLetters[GetServerNum(pos, 4)], zoom, pos.x, pos.y);
		s.Format("%s?app_id=%s&app_code=%s", s, m_globalSettings.hereAppId, m_globalSettings.hereAppCode);
		return s;
	}

	bool Initialize() 
	{
		if (m_globalSettings.hereAppCode.GetLength() == 0 ||
			m_globalSettings.hereAppId.GetLength() == 0) {
			CallbackHelper::ErrorMsg("Application ID or (and) code are not set for Here Maps tiles provider. See GlobalSettings.SetHereMapsApiKey.");
			return false;
		}
		return true;
	}
};

class OviMapProvider: public OviBaseProvider
{
public:
	OviMapProvider() 
	{
		Id = tkTileProvider::HereMaps;
		Name = "Here Maps";
		UrlFormat = "http://%c.base.maps.api.here.com/maptile/2.1/maptile/newest/normal.day/%d/%d/%d/256/png8";
	}
};

class OviSatelliteProvider: public OviBaseProvider
{
public:
	OviSatelliteProvider() 
	{
		Id = tkTileProvider::HereSatellite;
		Name = "Here Satellite";
		UrlFormat = "http://%c.aerial.maps.api.here.com/maptile/2.1/maptile/newest/satellite.day/%d/%d/%d/256/png8";
	}
};

class OviHybridProvider: public OviBaseProvider
{
public:
	OviHybridProvider() 
	{
		Id = tkTileProvider::HereHybrid;
		Name = "Here Hybrid";
		UrlFormat = "http://%c.aerial.maps.api.here.com/maptile/2.1/maptile/newest/hybrid.day/%d/%d/%d/256/png8";
	}
};

class OviTerrainProvider: public OviBaseProvider
{
public:
	OviTerrainProvider() 
	{
		Id = tkTileProvider::HereTerrain;
		Name = "Here Terrain";
		UrlFormat = "http://%c.aerial.maps.api.here.com/maptile/2.1/maptile/newest/terrain.day/%d/%d/%d/256/png8";
	}
};

