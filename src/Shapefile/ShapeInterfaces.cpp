/**************************************************************************************
 * File name: ShapeInterfaces.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of ShapeInterfaces.cpp
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
 #pragma once
#include "StdAfx.h"
#include "ShapeInterfaces.h"

//********************************************************************
//*		get_ContentLength()
//********************************************************************
int IShapeWrapper::get_ContentLength() 
{ 
	return ShapeUtility::get_ContentLength(get_ShapeType(), get_PointCount(), get_PartCount()); 
}

//********************************************************************
//*		CopyTo()
//********************************************************************
void IShapeWrapper::CopyTo(IShapeWrapper* target)
{
	if (!target) return;
	target->Clear();

	ShpfileType shpType = get_ShapeType();
	bool isM = ShapeUtility::IsM(shpType);
	bool isZ = ShapeUtility::IsZ(shpType);

	for (int i = 0; i < get_PartCount(); i++)
	{
		int part = get_PartStartPoint(i);
		target->InsertPart(i, part);
	}

	double x, y, z, m;
	for (int i = 0; i < get_PointCount(); i++)
	{
		get_PointXY(i, x, y);
		target->InsertPointXY(i, x, y);

		if (isM || isZ)
		{
			get_PointM(i, m);
			target->put_PointM(i, m);

			if (isZ)
			{
				get_PointZ(i, z);
				target->put_PointZ(i, z);
			}
		}
	}
}