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
#include "AmersfoortProjection.h"

// *******************************************************
//		CustomTileProvider
// *******************************************************
// TMS provider with specific projection and URL pattern.
class CustomTileProvider: public BaseProvider
{
public:
	CustomTileProvider(int id, CString name, CString urlPattern, tkTileProjection projection, int minZoom, int maxZoom) throw(...)
	{
		this->Id = (tkTileProvider)id;
		this->Name = name;
		this->_minZoom = minZoom;
		this->_maxZoom = maxZoom;
		_projectionId = projection;
		_subProviders.push_back(this);

		Init(urlPattern, projection);
		_urlFormat = urlPattern;
	}
private:
	CString _pattern;
	std::vector<CString> _tokens;		// varying parts of pattern to be chosen randomly
	tkTileProjection _projectionId;

private:
	void Init(CString& urlPattern, tkTileProjection& projection);
	CString MakeTileImageUrl(CPoint &pos, int zoom);

public:
	tkTileProjection get_Projection() { return _projectionId; }

};



