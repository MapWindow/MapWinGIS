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

class RosreestrProvider: public BaseProvider
{
public:
	RosreestrProvider(bool labels) 
	{
		Id = tkTileProvider::Rosreestr;
		Name = "Rosreestr";
		Copyright = "";
		ServerLetters = "abc";
		RefererUrl = "http://maps.rosreestr.ru/";
		CString type = labels ? "Anno" : "BaseMap";
		UrlFormat = "http://%c.maps.rosreestr.ru/ArcGIS/rest/services/BaseMaps/" + type + "/MapServer/tile/%d/%d/%d";
		this->Projection = new MercatorProjection();
		this->maxZoom = 19;
	}
	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		char letter = ServerLetters[GetServerNum(pos, 3)];
		CString s; 
		s.Format(UrlFormat, letter, zoom,  pos.y, pos.x);
		return s;
	}
};

class RosreestrBordersProvider: public WMSGMercProvider
{
private:	
	CString url2;
public:
	RosreestrBordersProvider() 
	{
		Id = tkTileProvider::Rosreestr;
		Name = "Rosreestr";
		Copyright = "";
		RefererUrl = "http://maps.rosreestr.ru/";
		UrlFormat = "http://maps.rosreestr.ru/ArcGIS/rest/services/CadastreNew/Cadastre/MapServer/export?dpi=96&transparent=true&format=png32&bbox=";
		url2 = "&bboxSR=102100&imageSR=102100&size=256%2C256&f=image";
		this->subProviders.push_back(this);
	}

	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		CString s = UrlFormat + GetBoundingBox(pos, zoom) + url2; 
		return s;
	}
};