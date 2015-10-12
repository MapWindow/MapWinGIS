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
  
#include "stdafx.h"
#include "CustomTileProvider.h"

// ******************************************************
//    Init()
// ******************************************************
void CustomTileProvider::Init(CString& urlPattern, tkTileProjection& projection)
{
	switch (projection)
	{
		case tkTileProjection::SphericalMercator:
			this->_projection = new MercatorProjection();
			break;
		case tkTileProjection::Amersfoort:
			this->_projection = new AmersfoortProjection();
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
	CString token[3] = { "{zoom}", "{x}", "{y}" };
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
			this->_pattern = temp.Mid(pos, pos2 - pos + 1);
			_serverLetters = _pattern.Mid(8, _pattern.GetLength() - 9);

			pos = 0;
			CString token = _serverLetters.Tokenize(",", pos);

			while (!token.IsEmpty())
			{
				_tokens.push_back(token);
				token = _serverLetters.Tokenize(",", pos);
			}
		}
	}
}

// ******************************************************
//    MakeTileImageUrl()
// ******************************************************
CString CustomTileProvider::MakeTileImageUrl(CPoint &pos, int zoom)
{
	CString url = this->_urlFormat;
	CString s;

	s.Format("%d", zoom);
	url = Utility::ReplaceNoCase(url, "{zoom}", s);

	s.Format("%d", pos.x);
	url = Utility::ReplaceNoCase(url, "{x}", s);

	s.Format("%d", pos.y);
	url = Utility::ReplaceNoCase(url, "{y}", s);

	if (_pattern.GetLength() != 0 && _tokens.size() > 0)
	{
		int val = GetServerNum(pos, _tokens.size());
		url.Replace(_pattern, _tokens[val]);
	}
	return url;
}