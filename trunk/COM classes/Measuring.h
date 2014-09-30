/**************************************************************************************
 * File name: Measuring.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CMeasuring
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
#include "MeasuringBase.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CMeasuring
class ATL_NO_VTABLE CMeasuring :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMeasuring, &CLSID_Measuring>,
	public IDispatchImpl<IMeasuring, &IID_IMeasuring, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CMeasuring() {
		_measuring = new MeasuringBase();
		_lastErrorCode = tkNO_ERROR;
		_globalCallback = NULL;
		USES_CONVERSION;
		_key = A2BSTR("");
	}

	~CMeasuring() {
		delete _measuring;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_MEASURING)

	BEGIN_COM_MAP(CMeasuring)
		COM_INTERFACE_ENTRY(IMeasuring)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Length)(double* retVal);
	STDMETHOD(UndoPoint)(VARIANT_BOOL* retVal);
	STDMETHOD(get_PointCount)(long* retVal);
	STDMETHOD(get_PointXY)(long pointIndex, double* x, double* y, VARIANT_BOOL* retVal);
	STDMETHOD(get_AreaWithClosingVertex)(double lastPointProjX, double lastPointProjY, double* retVal);
	STDMETHOD(get_MeasuringType)(tkMeasuringType* retVal);
	STDMETHOD(put_MeasuringType)(tkMeasuringType newVal);
	STDMETHOD(FinishMeasuring)();
	STDMETHOD(get_Area)(double* retVal);
	STDMETHOD(get_IsStopped)(VARIANT_BOOL* retVal);
	STDMETHOD(Clear)();
	STDMETHOD(get_SegmentLength)(int segmentIndex, double* retVal);
	STDMETHOD(get_Persistent)(VARIANT_BOOL* retVal);
	STDMETHOD(put_Persistent)(VARIANT_BOOL newVal);
	STDMETHOD(get_DisplayAngles)(VARIANT_BOOL* retVal);
	STDMETHOD(put_DisplayAngles)(VARIANT_BOOL newVal);
	STDMETHOD(get_IsUsingEllipsoid)(VARIANT_BOOL* retVal);
	STDMETHOD(get_AreaDisplayMode)(tkAreaDisplayMode* retVal);
	STDMETHOD(put_AreaDisplayMode)(tkAreaDisplayMode newVal);
	
private:
	long _lastErrorCode;
	ICallback * _globalCallback;
	BSTR _key;
	MeasuringBase* _measuring;
public:
	MeasuringBase* GetBase() { return _measuring; }
};

OBJECT_ENTRY_AUTO(__uuidof(Measuring), CMeasuring)
