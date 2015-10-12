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
#include "CustomProjection.h"

// Amersfoort projection for Netherlands, EPSG 28992
class AmersfoortProjection: public CustomProjection
{
public:	
	AmersfoortProjection()
	{
		_yInverse = true;
		_worldWide = false;
		_serverProjection = tkTileProjection::Amersfoort;

		_yMin = 22598.080;
		_yMax = 903401.920;
		_xMin = -285401.920;
		_xMax = 595401.92;

		VARIANT_BOOL vb;
		_projCustom->ImportFromEPSG(28992, &vb);

		if (!vb) {
			CallbackHelper::ErrorMsg("Failed to initialize projection for Amersfoort.");
			return;
		}

		UpdateBounds();
	}

private:
};