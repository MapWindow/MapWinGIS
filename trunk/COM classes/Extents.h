/**************************************************************************************
 * File name: Extents.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CExtents
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

// CExtents
class ATL_NO_VTABLE CExtents : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CExtents, &CLSID_Extents>,
	public IDispatchImpl<IExtents, &IID_IExtents, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CExtents()
	{
		_xmin = 0.0;
		_max = 0.0;
		_ymin = 0.0;
		_ymax = 0.0;
		_zmin = 0.0;
		_zmax = 0.0;
		_mmin = 0.0;
		_mmax = 0.0;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_EXTENTS)

	DECLARE_NOT_AGGREGATABLE(CExtents)

	BEGIN_COM_MAP(CExtents)
		COM_INTERFACE_ENTRY(IExtents)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

// IExtents
public:
	STDMETHOD(SetMeasureBounds)(/*[in]*/double mMin, /*[in]*/double mMax);
	STDMETHOD(GetMeasureBounds)(/*[out]*/ double * mMin, /*[out]*/ double * mMax);
	STDMETHOD(get_mMax)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_mMin)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_zMax)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_zMin)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_yMax)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_yMin)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_xMax)(/*[out, retval]*/ double *pVal);
	STDMETHOD(get_xMin)(/*[out, retval]*/ double *pVal);
	STDMETHOD(SetBounds)(/*[in]*/ double xMin, /*[in]*/ double yMin, /*[in]*/ double zMin, /*[in]*/ double xMax, /*[in]*/ double yMax, /*[in]*/ double zMax);
	STDMETHOD(GetBounds)(/*[out]*/ double * xMout, /*[out]*/ double * yMout, /*[out]*/ double * zMout, /*[out]*/ double * xMax, /*[out]*/ double * yMax, /*[out]*/ double * zMax);
	STDMETHOD(ToShape)(IShape** retVal);
	STDMETHOD(Disjoint)(IExtents* ext, VARIANT_BOOL* retVal);
	STDMETHOD(GetIntersection)(IExtents* ext, IExtents** retVal);
	STDMETHOD(Union)(IExtents* ext);
	STDMETHOD(Intersects)(IExtents* ext, VARIANT_BOOL* retVal);
	STDMETHOD(ToDebugString)(BSTR* retVal);
	STDMETHOD(PointIsWithin)(double x, double y, VARIANT_BOOL* retVal);
	STDMETHOD(get_Center)(IPoint** retVal);
	STDMETHOD(MoveTo)(double x, double y);
	
private:
	double _xmin;
	double _max;
	double _ymin;
	double _ymax;
	double _zmin;
	double _zmax;
	double _mmin;
	double _mmax;
};

OBJECT_ENTRY_AUTO(__uuidof(Extents), CExtents)
