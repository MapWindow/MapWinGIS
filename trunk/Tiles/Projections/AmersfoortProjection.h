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
	// EPSG 28992
	AmersfoortProjection()
	{
		bool calculateDegrees = false;
		
		if (!calculateDegrees)
		{
			// let's just use precalculated values; it unlikely that proj4 calculation will ever change
			//Lat = -0.515879655998403; Lng = 48.0405185800458
			//Lat = -1.65729159493417; Lng = 55.9136415710084
			//Lat = 12.4317273490522; Lng = 55.9136222637686
			//Lat = 11.2902580269759; Lng = 48.0405018446845

			// there is some question however as to which values to take as we don't have 
			// a horizontally oriented rectangle; let's take the maximum bounds
			MinLatitude = -0.515879655998403;
			MaxLatitude = 12.4317273490522;
			MinLongitude = 48.0405018446845;
			MaxLongitude = 55.9136415710084;
		}
		else
		{
			// in meters 
			MinLatitude = 22598.080;
			MaxLatitude = 903401.920;
			MinLongitude = -285401.920;
			MaxLongitude = 595401.92;

			VARIANT_BOOL ret1, ret2;
			projWGS84->ImportFromEPSG(4326, &ret1);
			projCustom->ImportFromEPSG(28992, &ret2);

			if (!ret1 || !ret2)
				Debug::WriteLine("Failed to initialize projection for Amersfoort");

			projWGS84->StartTransform(projCustom, &ret1);
			if (!ret1)
				Debug::WriteLine("Failed to initialize transformation 1");

			projCustom->StartTransform(projWGS84, &ret2);
			if (!ret2)
				Debug::WriteLine("Failed to initialize transformation 2");
		}

		yInverse = true;
		worldWide = false;

		serverProjection = tkTileProjection::Amersfoort;
	}
};