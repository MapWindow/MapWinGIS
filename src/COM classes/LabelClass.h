/**************************************************************************************
 * File name: LabelClass.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CLabelClass
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
 // lsu: jan 2010 - created the file.

#pragma once
#include "LabelOptions.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CLabelClass
class ATL_NO_VTABLE CLabelClass :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CLabelClass, &CLSID_Label>,
	public IDispatchImpl<ILabel, &IID_ILabel, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CLabelClass()
	{
		_pUnkMarshaler = NULL;
		_canDelete = true;
		_label = new CLabelInfo();
		gReferenceCounter.AddRef(tkInterface::idLabel);
	}
	~CLabelClass()
	{
		if (_label != NULL && _canDelete) 
			delete _label;
		gReferenceCounter.Release(tkInterface::idLabel);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_LABELCLASS)

	BEGIN_COM_MAP(CLabelClass)
		COM_INTERFACE_ENTRY(ILabel)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, _pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &_pUnkMarshaler.p);
		return S_OK;
	}

	void FinalRelease()
	{
		_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> _pUnkMarshaler;

public:
	STDMETHOD(get_Visible)(VARIANT_BOOL* retval)			{*retval = _label->visible ? VARIANT_TRUE : VARIANT_FALSE;	return S_OK;};
	STDMETHOD(put_Visible)(VARIANT_BOOL newVal)				{_label->visible = newVal?true:false;	return S_OK;};		
	
	STDMETHOD(get_Rotation)(double* retval)					{*retval = _label->rotation;		return S_OK;};
	STDMETHOD(put_Rotation)(double newVal)					{_label->rotation = newVal;		    return S_OK;};
	
	STDMETHOD(get_Text)(BSTR* retval);						
	STDMETHOD(put_Text)(BSTR newVal);
	
	STDMETHOD(get_X)(double* retval)						{*retval = _label->x;				return S_OK;};
	STDMETHOD(put_X)(double newVal)							{_label->x = newVal;				return S_OK;};
	
	STDMETHOD(get_Y)(double* retval)						{*retval = _label->y;				return S_OK;};
	STDMETHOD(put_Y)(double newVal)							{_label->y = newVal;				return S_OK;};

    STDMETHOD(get_OffsetX)(double* retval)                  {*retval = _label->offsetX;     	return S_OK;};
    STDMETHOD(put_OffsetX)(double newVal)                   {_label->offsetX = newVal;			return S_OK;};

    STDMETHOD(get_OffsetY)(double* retval)                  {*retval = _label->offsetY;			return S_OK;};
    STDMETHOD(put_OffsetY)(double newVal)                   {_label->offsetY = newVal;			return S_OK;};

	STDMETHOD(get_Category)(long* retval)					{*retval = _label->category;		return S_OK;};
	STDMETHOD(put_Category)(long newVal)					{_label->category = newVal;		    return S_OK;};

	STDMETHOD(get_IsDrawn)(VARIANT_BOOL* retval)			{*retval = _label->isDrawn;		    return S_OK;};

	STDMETHOD(get_ScreenExtents)(IExtents** retval);

private:
	CLabelInfo* _label;
	bool _canDelete;		// CLabelInfo can be allocated locally, then we need to delete it
							// For example, when client create CLabel class with new command.
							// In case CLabelInfo is a pointer to the element of CLabels vector we must not delete it
							// For a example if a client receive reference to some existing label and then releases 
							// CLabelClass.

public:
	char* get_LabelData();
	void put_LabelData(char* newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(Label), CLabelClass)
