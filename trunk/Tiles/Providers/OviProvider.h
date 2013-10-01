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
		RefererUrl = "http://maps.ovi.com/";
        int year = Utility::getCurrentYear();
		Copyright.Format("©%d OVI Nokia - Map data ©%d NAVTEQ, Imagery ©%d DigitalGlobe", year, year, year);
		UrlServerLetters = "bcde";
		this->Projection = new MercatorProjection();
		subProviders.push_back(this);
	}
	
	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		CString s;
		s.Format(UrlFormat, UrlServerLetters[GetServerNum(pos, 4)], zoom, pos.x, pos.y);
		return s;
	}
};

class OviMapProvider: public OviBaseProvider
{
public:
	OviMapProvider() 
	{
		Id = tkTileProvider::OviMaps;
		Name = "OviMaps";
		UrlFormat = "http://%c.maptile.maps.svc.ovi.com/maptiler/v2/maptile/newest/normal.day/%d/%d/%d/256/png8";
	}
};

class OviSatelliteProvider: public OviBaseProvider
{
public:
	OviSatelliteProvider() 
	{
		Id = tkTileProvider::OviSatellite;
		Name = "OviSatellite";
		UrlFormat = "http://%c.maptile.maps.svc.ovi.com/maptiler/v2/maptile/newest/satellite.day/%d/%d/%d/256/png8";
	}
};

class OviHybridProvider: public OviBaseProvider
{
public:
	OviHybridProvider() 
	{
		Id = tkTileProvider::OviHybrid;
		Name = "OviHybrid";
		UrlFormat = "http://%c.maptile.maps.svc.ovi.com/maptiler/v2/maptile/newest/hybrid.day/%d/%d/%d/256/png8";
	}
};

class OviTerrainProvider: public OviBaseProvider
{
public:
	OviTerrainProvider() 
	{
		Id = tkTileProvider::OviTerrain;
		Name = "OviTerrain";
		UrlFormat = "http://%c.maptile.maps.svc.ovi.com/maptiler/v2/maptile/newest/terrain.day/%d/%d/%d/256/png8";
	}
};

