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
#include "WMSGMercProvider.h"

class TopomapperProvider: public WMSGMercProvider
{
private: 
	CString url2;
public:
	TopomapperProvider() 
	{
		Id = tkTileProvider::Topomapper;
		Name = "Topomapper";
		Copyright = "";
		RefererUrl = "http://www.topomapper.com/";
		//jpeg
		UrlFormat = "http://www.topomapper.com/cgi-bin/tilecache-2.11b/tilecache.py?LAYERS=topomapper_gmerc&FORMAT=image%2Fpng&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&SRS=EPSG%3A900913&BBOX=";
		url2 = "&WIDTH=256&HEIGHT=256";
		subProviders.push_back(this);
		this->maxZoom = 13;
	}
	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		CString s = UrlFormat + GetBoundingBox(pos, zoom) + url2; 
		return s;
	}
};