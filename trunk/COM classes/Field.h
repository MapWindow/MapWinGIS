/**************************************************************************************
 * File name: Field.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CField
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

// CField
class ATL_NO_VTABLE CField : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CField, &CLSID_Field>,
	public IDispatchImpl<IField, &IID_IField, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CField()
	{
		USES_CONVERSION;
		key = A2BSTR("");
		name = A2BSTR("IField");
		globalCallback = NULL;
		lastErrorCode = tkNO_ERROR;
		width = 10;
		precision = 10;
		type = STRING_FIELD;
		isUpdated = false;
		_table = NULL;
		gReferenceCounter.AddRef(tkInterface::idField);
	}

	~CField()
	{
		::SysFreeString(key);
		::SysFreeString(name);

		if( globalCallback )
			globalCallback->Release();
		globalCallback = NULL;
		gReferenceCounter.Release(tkInterface::idField);
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_FIELD)

DECLARE_NOT_AGGREGATABLE(CField)

BEGIN_COM_MAP(CField)
	COM_INTERFACE_ENTRY(IField)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


// IField
public:
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Type)(/*[out, retval]*/ FieldType *pVal);
	STDMETHOD(put_Type)(/*[in]*/ FieldType newVal);
	STDMETHOD(get_Precision)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Precision)(/*[in]*/ long newVal);
	STDMETHOD(get_Width)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Width)(/*[in]*/ long newVal);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
	STDMETHOD(Clone)(/*[out, retval]*/ IField** retVal);
	
	// to use from table class
	// If any changes were made that imply the rewriting of dbf file, it should be set to true
	bool isUpdated;
	ITable* _table;		// no references should be added here
	void CField::ErrorMessage(long ErrorCode);

private:
	BSTR key;
	long lastErrorCode;
	ICallback * globalCallback;

	long precision;
	long width;
	FieldType type;
	BSTR name;

	bool CheckTableEditingState();
};

OBJECT_ENTRY_AUTO(__uuidof(Field), CField)
