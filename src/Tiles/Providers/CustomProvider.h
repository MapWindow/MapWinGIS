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

class CustomProvider: public BaseProvider
{
public:
	tkTileProjection m_projectionId;
	std::vector<CString> tokens;		// varying parts of pattern to be chosen randomly
	CString pattern;

	CustomProvider(int id, CString name, CString urlPattern, tkTileProjection projection, int minZoom, int maxZoom) throw(...)
	{
		this->Id = (tkTileProvider)id;
		this->Name = name;
		this->minZoom = minZoom;
		this->maxZoom = maxZoom;
		this->m_projectionId = projection;
		subProviders.push_back(this);

		Init(urlPattern, projection);
		UrlFormat = urlPattern;
	}

	void Init(CString& urlPattern, tkTileProjection& projection)
	{
		switch(projection)
		{
			case tkTileProjection::SphericalMercator:
				this->Projection = new MercatorProjection();
				break;
			case tkTileProjection::Amersfoort:
				this->Projection = new AmersfoortProjection();
				break;
			default:
				CallbackHelper::ErrorMsg("Invalid projection is set for custom tile provider.");
				throw 1;
				return;
		}
		
		// these 3 parts are required in any pattern
		CString temp = urlPattern;
		temp.MakeLower();
		temp.Replace(" ", "");
		CString token[3] = {"{zoom}", "{x}", "{y}"};
		for (int i = 0; i < 3; i++)	{
			int pos = temp.Find(token[i]); 
			if (pos < 0)
				throw 2;
		}

		// varying server numbers
		int pos = temp.Find("{switch:"); 
		if (pos >= 0)
		{
			int pos2 = temp.Find("}", pos);
			if (pos2 < 0) {
				throw 2;	// no closing bracket for switch pattern
			}
			else
			{
				this->pattern = temp.Mid(pos, pos2 - pos + 1);
				ServerLetters = pattern.Mid(8, pattern.GetLength() - 9);
				
				pos = 0;
				CString token = ServerLetters.Tokenize(",", pos);

				while (!token.IsEmpty())
				{
					tokens.push_back(token);
					token = ServerLetters.Tokenize(",", pos);
				}
			}
		}
	}
	
	CString MakeTileImageUrl(CPoint &pos, int zoom)
	{
		CString url = this->UrlFormat;
		CString s;

		s.Format("%d", zoom);
		url = Utility::ReplaceNoCase(url, "{zoom}", s);

		s.Format("%d", pos.x);
		url = Utility::ReplaceNoCase(url, "{x}", s);

		s.Format("%d", pos.y);
		url = Utility::ReplaceNoCase(url, "{y}", s);

		if (pattern.GetLength() != 0 && tokens.size() > 0)
		{
			int val = GetServerNum(pos, tokens.size());
			url.Replace(pattern, tokens[val]);
		}
		return url;
	}
};



