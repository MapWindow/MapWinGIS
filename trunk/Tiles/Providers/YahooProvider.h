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

class YahooBaseProvider: public BaseProvider
{
protected:
	
public:
	YahooBaseProvider() 
	{
		RefererUrl = "http://maps.yahoo.com/";
		Copyright.Format(L"© Yahoo! Inc. - Map data & Imagery ©%d NAVTEQ", Utility::GetCurrentYear());
		this->Projection = new MercatorProjection();
	}
	
	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		CString s;
		s.Format(UrlFormat, ((GetServerNum(pos, 2)) + 1), Version, LanguageStr, pos.x, (((1 << zoom) >> 1) - 1 - pos.y), (zoom + 1));
		return s;
	}
};

class YahooMapProvider: public YahooBaseProvider
{
public:
	YahooMapProvider() 
	{
		Version = "4.3";
		Id = tkTileProvider::YahooMaps;
		Name = "YahooMaps";
		UrlFormat = "http://maps%d.yimg.com/hx/tl?v=%s&.intl=%s&x=%d&y=%d&z=%d&r=1";
		subProviders.push_back(this);					
	}
};

class YahooSatelliteProvider: public YahooBaseProvider
{
public:
	YahooSatelliteProvider() 
	{
		Version = "1.9";
		Id = tkTileProvider::YahooSatellite;
		Name = "YahooSatellite";
		UrlFormat = "http://maps%d.yimg.com/ae/ximg?v=%s&t=a&s=256&.intl=%s&x=%d&y=%d&z=%d&r=1";		
		subProviders.push_back(this);
	}
};

class YahooHybridProvider: public YahooBaseProvider
{
public:
	YahooHybridProvider(CTileProviders* list) 
	{
		Version = "4.3";
		Id = tkTileProvider::YahooHybrid;
		Name = "YahooHybrid";
		UrlFormat = "http://maps%d.yimg.com/hx/tl?v=%s&t=h&.intl=%s&x=%d&y=%d&z=%d&r=1";		
		subProviders.push_back(list->get_Provider(tkTileProvider::YahooSatellite));
		subProviders.push_back(this);
	}
};


