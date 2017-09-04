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
#include "WmsProviderBase.h"

// ************************************************************
//		RosreestrProvider()
// ************************************************************
class RosreestrProvider : public BaseProvider
{
public:
	RosreestrProvider(bool labels)
	{
		Id = tkTileProvider::Rosreestr;
		Name = "Rosreestr";
		_copyright = "©Росреестр";
		_serverLetters = "abc";
		_licenseUrl = "http://maps.rosreestr.ru/PortalOnline/terms.html";
		_refererUrl = "http://maps.rosreestr.ru/";
		CString type = labels ? "Anno" : "BaseMap";
		_urlFormat = "http://%c.maps.rosreestr.ru/arcgis/rest/services/BaseMaps/" + type + "/MapServer/tile/%d/%d/%d";
		_projection = new MercatorProjection();
		_maxZoom = 19;
	}
	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		char letter = _serverLetters[GetServerNum(pos, 3)];
		CString s;
		s.Format(_urlFormat, letter, zoom, pos.y, pos.x);
		return s;
	}
};

// ************************************************************
//		RosreestrBordersProvider()
// ************************************************************
class RosreestrBordersProvider : public WmsMercatorProvider
{
private:
	CString _url2;
public:
	RosreestrBordersProvider()
	{
		Id = tkTileProvider::Rosreestr;
		Name = "Rosreestr";
		_copyright = "";
		_licenseUrl = "http://maps.rosreestr.ru/PortalOnline/terms.html";
		_refererUrl = "http://maps.rosreestr.ru/";
		_urlFormat = "http://maps.rosreestr.ru/arcgis/rest/services/Cadastre/Cadastre/MapServer/export?dpi=96&transparent=true&format=png32&bbox=";
		_url2 = "&bboxSR=102100&imageSR=102100&size=256%2C256&f=image";
		_subProviders.push_back(this);
	}

	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		CString s = _urlFormat + GetBoundingBox(pos, zoom) + _url2;
		return s;
	}
};