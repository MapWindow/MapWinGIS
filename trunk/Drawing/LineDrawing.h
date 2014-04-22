/**************************************************************************************
 * File name: LinePatternDrawing.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of the CLinePatternDrawing. 
 *
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
 // lsu 13 jan 2011 - Created the file.

#pragma once
#include "ShapeDrawingOptions.h"
#include "LinePattern.h"
#include "GeometryOperations.h"

using namespace Gdiplus;

class CLineDrawer
{
public:

	CLineDrawer(CDC* dc)
	{
		_dc = dc;
	}
	~CLineDrawer(void)
	{

	}

	bool DrawLine(Gdiplus::GraphicsPath& path, ILinePattern* linePattern);
	void DrawPart(CLinePattern* linePattern, int& beginX, int& endX);
	
protected:
	CDC* _dc;
};
