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
#include "TileCore.h"

Debug::Logger tilesLogger;

// ******************************************************
//    TileCore
// ******************************************************
// gets relative path of tile disk storage
CStringW TileCore::get_Path(CStringW root, CStringW ext)
{
	CStringW path;
	path.Format(L"%s%d\\%d\\%d%s", root, this->_scale, this->_tileX, this->_tileY, ext);
	return path;
}

// ******************************************************
//    ClearOverlays
// ******************************************************
void TileCore::ClearOverlays()
{
	for (size_t i = 0; i < Overlays.size(); i++)
	{
		delete Overlays[i];
	}
	Overlays.clear();
}

// ******************************************************
//    getByteSize
// ******************************************************
// returns combined size of bitmaps for all overlays
int TileCore::get_ByteSize()
{
	int size = 0;
	for (size_t i = 0; i < Overlays.size(); i++)
	{
		size += Overlays[i]->get_Size();
	}
	return size;
}

// ******************************************************
//    AddRef
// ******************************************************
long TileCore::AddRef()
{
	InterlockedIncrement(&_refCount);
	return _refCount;
}

// ******************************************************
//    Release
// ******************************************************
// Attention: object is deleted automatically when reference count is equal to 0
long TileCore::Release()
{
	InterlockedDecrement(&_refCount);

	if (_refCount < 0)
		CallbackHelper::AssertionFailed("Invalid reference count for a tile.");

	if (this->_refCount == 0)
	{
		delete this;
		return 0;
	}
	else {
		return _refCount;
	}
}

// ******************************************************
//    operator==
// ******************************************************
bool TileCore::operator==(const TileCore &t2)
{
	return (this->_tileX == t2._tileX &&
		this->_tileY == t2._tileY &&
		this->_scale == t2._scale &&
		this->_providerId == t2._providerId);
}

// ******************************************************
//    getBitmap
// ******************************************************
CMemoryBitmap* TileCore::get_Bitmap(int index)
{ 
	return (index >= 0 && index < (int)Overlays.size()) ? Overlays[index] : NULL; 
}