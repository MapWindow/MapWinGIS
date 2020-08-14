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

// *******************************************************
//		WmsProviderBase
// *******************************************************
// Base class for WMS providers.
class WmsProviderBase : public BaseProvider
{
public:
	virtual ~WmsProviderBase() { }

	// gets bounding box in Google mercator projection (meters; EPSG:3857)
	virtual CString GetBoundingBox(CPoint &pos, int zoom, tkWmsVersion version, tkWmsBoundingBoxOrder bbo);

	virtual bool IsWms() { return true; }
};

// *******************************************************
//		WmsMercatorProvider
// *******************************************************
// WMS provider which uses spherical Mercator projection.
class WmsMercatorProvider : public WmsProviderBase
{
public:
	WmsMercatorProvider()
	{
		_projection = new MercatorProjection();
	}

	virtual ~WmsMercatorProvider(void) { };
};

