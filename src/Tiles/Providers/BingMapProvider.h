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
#include "MercatorProjection.h"

class BingBaseProvider: public BaseProvider
{
protected:
	CString _imagerySet;
public:
	BingBaseProvider() 
	{
		RefererUrl = "http://www.bing.com/maps/";
		Copyright = L"Copyright © 2014 Microsoft and its suppliers";
		this->Projection = new MercatorProjection();
		subProviders.push_back(this);
	}
	
	bool Initialize();
	CString TileXYToQuadKey(int tileX, int tileY, int levelOfDetail);

	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		// http://ecn.{subdomain}.tiles.virtualearth.net/tiles/r{quadkey}.jpeg?g=3179&mkt={culture}&shading=hill
		CString key = TileXYToQuadKey(pos.x, pos.y, zoom);
		CString subDomain;
		subDomain.Format("t%d", GetServerNum(pos, 4));

		CString temp = UrlFormat;
		temp.Replace("{quadkey}", key);
		temp.Replace("{culture}", LanguageStr);
		temp.Replace("{subdomain}", subDomain);

		return temp;
	}

	virtual CStringW GetCopyright()
	{
		if (UrlFormat.GetLength() == 0) {
			return "INVALID BING MAPS API KEY";
		}
		else {
			return Copyright;
		}
	}
};

class BingMapProvider: public BingBaseProvider
{
public:
	BingMapProvider() 
	{
		_imagerySet = "Road";
		Id = tkTileProvider::BingMaps;
		Name = "BingMaps";
	}
};

class BingSatelliteProvider: public BingBaseProvider
{
public:
	BingSatelliteProvider() 
	{
		_imagerySet = "Aerial";
		Id = tkTileProvider::BingSatellite;
		Name = "BingSatellite";
	}
};

class BingHybridProvider: public BingBaseProvider
{
public:
	BingHybridProvider() 
	{
		_imagerySet = "AerialWithLabels";
		Id = tkTileProvider::BingHybrid;
		Name = "BingHybrid";
	}
};

