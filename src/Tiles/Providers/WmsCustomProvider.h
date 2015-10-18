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
#include "WmsProviderBase.h"
#include "CustomProjection.h"

// *******************************************************
//		WmsCustomProvider
// *******************************************************
// WMS provider which uses projection with arbitrary EPSG code.
class WmsCustomProvider : public WmsProviderBase
{
public:
	WmsCustomProvider()
	{
		_projection = new CustomProjection();
		_subProviders.push_back(this);
	}

	virtual ~WmsCustomProvider() { }

private:
	CString _layers;
	CString _format;

public:
	virtual CString MakeTileImageUrl(CPoint &pos, int zoom);

	CustomProjection* get_CustomProjection() { return dynamic_cast<CustomProjection*>(_projection); }
	void put_UrlFormat(CString baseUrl) { _urlFormat = baseUrl; }
	CString get_Layers() { return _layers; }
	void set_Layers(CString value) { _layers = value; }
	CString get_Format() { return _format; }
	void set_Format(CString value) { _format = value; }
};
