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
 // lsu 17 apr 2012 - created the file

#pragma once

// Point on globe with longitude and latitude in decimal degrees
class PointLatLng
{
public:
	PointLatLng(double lat, double lng): Lat(lat), Lng(lng) {};
	PointLatLng(): Lat(0.0), Lng(0.0) {};
	double Lat;
	double Lng;
};

// Size specified of rectangle specified in termas of longitude and latitude in decimal degrees
class SizeLatLng
{
public:
	SizeLatLng(double widthLng, double heightLat): WidthLng(widthLng), HeightLat(heightLat) {};
	SizeLatLng(): WidthLng(0.0), HeightLat(0.0) {};
	double WidthLng;
	double HeightLat;
};
// A rectangle specified in terms of latitude and longitude in decimal degrees
struct RectLatLng
{
public:
	double xLng;
	double yLat;
	double WidthLng;
	double HeightLat;

	double MinLat() { return yLat - HeightLat; }
	double MaxLng() { return xLng + WidthLng; }

	void SetBounds(double lng, double lat, double widthLng, double heightLat)
	{
		xLng = lng;
		yLat = lat;
		WidthLng = widthLng;
		HeightLat = heightLat;
	}

	RectLatLng(double lng, double lat, double widthLng, double heightLat): xLng(lng), yLat(lat), WidthLng(widthLng), HeightLat(heightLat){};
	RectLatLng(): xLng(0.0), yLat(0.0), WidthLng(0.0), HeightLat(0.0){};
};
