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

class BingBaseProvider: public BaseProvider
{
    CString ClientKey;		// Bing Maps Customer Identification, more info here: http://msdn.microsoft.com/en-us/library/bb924353.aspx
public:

	BingBaseProvider() 
	{
		Version = "790";
		RefererUrl = "http://www.bing.com/maps/";
		int year = Utility::GetCurrentYear();
		Copyright.Format(L"©%d Microsoft Corporation, ©%d NAVTEQ, ©%d Image courtesy of NASA", year, year, year);
		this->Projection = new MercatorProjection();
		subProviders.push_back(this);
	}

	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		CString key = TileXYToQuadKey(pos.x, pos.y, zoom);
		CString client = ClientKey == "" ? "" : "&key=" + ClientKey;
		int num = GetServerNum(pos, 4);
		CString s;
		s.Format(UrlFormat, num, key, Version, LanguageStr, client);
		return s;
	}

	// Converts tile XY coordinates into a QuadKey at a specified level of detail.
	// LevelOfDetail: Level of detail, from 1 (lowest detail) to 23 (highest detail).
	CString TileXYToQuadKey(int tileX, int tileY, int levelOfDetail)
	{
		CString s;
		for(int i = levelOfDetail; i > 0; i--)
		{
			char digit = '0';
			int mask = 1 << (i - 1);
			if((tileX & mask) != 0)
			{
			   digit++;
			}
			if((tileY & mask) != 0)
			{
			   digit++;
			   digit++;
			}

			s.AppendChar(digit);
		}
		return s;
	}
};

class BingMapProvider: public BingBaseProvider
{
public:
	BingMapProvider() 
	{
		Id = tkTileProvider::BingMaps;
		Name = "BingMaps";
		UrlFormat = "http://ecn.t%d.tiles.virtualearth.net/tiles/r%s.png?g=%s&mkt=%s&lbl=l1&stl=h&shading=hill&n=z%s";
	}
};

class BingSatelliteProvider: public BingBaseProvider
{
public:
	BingSatelliteProvider() 
	{
		Id = tkTileProvider::BingSatellite;
		Name = "BingSatellite";
		UrlFormat = "http://ecn.t%d.tiles.virtualearth.net/tiles/a%s.jpeg?g=%s&mkt=%s%s";		// the last %s is actually not needed; added for uniformity with BingMap
	}
};

class BingHybridProvider: public BingBaseProvider
{
public:
	BingHybridProvider() 
	{
		Id = tkTileProvider::BingHybrid;
		Name = "BingHybrid";
		UrlFormat = "http://ecn.t%d.tiles.virtualearth.net/tiles/h%s.jpeg?g=%s&mkt=%s%s";
		
	}
};

