/**************************************************************************************
 * File name: ShapePoint.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CShapefileDrawer
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
 // Sergei Leschinski (lsu) 8 feb 2011 - created the file

#pragma once
#include "MapWinGis.h"

class IShapePoint
{
public:
	// Interface with methods common to CShapePoint and CShapePointCOM
	// function = 0 - pure virtual function, which makes class abstract; 
	// all the virtual function must be overriden in the derived class
	// abstract class can not be instantiated
	virtual BSTR* get_Key() = 0;
	virtual void put_Key(BSTR newVal) = 0;
	virtual double get_X() = 0;
	virtual double get_Y() = 0;
	virtual double get_Z() = 0;
	virtual double get_M() = 0;
	virtual void put_X(double newVal) = 0;
	virtual void put_Y(double newVal) = 0;
	virtual void put_Z(double newVal) = 0;
	virtual void put_M(double newVal) = 0;
	
	virtual bool put_Point(IPoint* newPoint) = 0;
	virtual IPoint* get_Point() = 0;
	virtual void get_XY(double& x, double& y) = 0;
	virtual void put_XY(double x, double y) = 0;
};

// Fast point without using COM
class CShapePoint: public IShapePoint
{
public:
	CShapePoint(void)
	{
		x = y = z = m = 0;
		pointExists = false;
	}
	~CShapePoint(void);

	double x;
	double y;
	double z;
	double m;
	CStringW key;
	bool pointExists;

	void get_Key(BSTR* pVal)
	{
		USES_CONVERSION;
		BSTR s = OLE2BSTR(key);
		*pVal = s;
	}
	void put_Key(BSTR newVal)
	{
		// TODO: find conversion
		key = newVal;
	}
	double get_X(){return x;}
	double get_Y(){return y;}
	double get_Z(){return z;}
	double get_M(){return m;}
	void put_X(double newVal){x = newVal;}
	void put_Y(double newVal){y = newVal;}
	void put_Z(double newVal){z = newVal;}
	void put_M(double newVal){m = newVal;}
	
	IPoint* get_Point();
	bool put_Point(IPoint* newPoint);
	void get_XY(double& x, double& y);
	void put_XY(double x, double y);
};

// Wrapper for COM point
class CShapePointCOM: public IShapePoint
{
public:
	CShapePointCOM(void)
	{
		CoCreateInstance( CLSID_Point, NULL, CLSCTX_INPROC_SERVER, IID_IPoint, (void**)&pnt);
	}
	~CShapePointCOM(void)
	{
		if (pnt)
		{
			pnt->Release();
		}
	}
	IPoint* pnt;

	void get_Key(BSTR* pVal);
	void put_Key(BSTR newVal);
	double get_X();
	double get_Y();
	double get_Z();
	double get_M();

	void put_X(double newVal){pnt->put_X(newVal);}
	void put_Y(double newVal){pnt->put_Y(newVal);}
	void put_Z(double newVal){pnt->put_Z(newVal);}
	void put_M(double newVal){pnt->put_M(newVal);}

	bool put_Point(IPoint* newPoint);
	IPoint* get_Point();
	void get_XY(double& x, double& y);
	void put_XY(double x, double y);
};