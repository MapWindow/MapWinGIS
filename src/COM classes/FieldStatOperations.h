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
 **************************************************************************************/
#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

struct FieldOperation
{
	int targetIndex;
	int fieldIndex;
	CStringW fieldName;
	bool hasName;
	tkFieldStatOperation operation;
	bool valid;
	FieldType targetFieldType;
	tkFieldOperationValidity isValidReason;

	FieldOperation()
	{
		targetIndex = -1;
		fieldIndex = -1;
		fieldName = L"";
		hasName = false;
		valid = true;
	}
};

// CFieldStatOperations
class ATL_NO_VTABLE CFieldStatOperations :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFieldStatOperations, &CLSID_FieldStatOperations>,
	public IDispatchImpl<IFieldStatOperations, &IID_IFieldStatOperations, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CFieldStatOperations()
	{
		_lastErrorCode = tkNO_ERROR;
		_key = SysAllocString(L"");
	}

	~CFieldStatOperations()
	{
		Clear();
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_FIELDSTATOPERATIONS)

	BEGIN_COM_MAP(CFieldStatOperations)
		COM_INTERFACE_ENTRY(IFieldStatOperations)
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
	STDMETHOD(AddFieldIndex)(int fieldIndex, tkFieldStatOperation operation);
	STDMETHOD(AddFieldName)(BSTR fieldIndex, tkFieldStatOperation operation);
	STDMETHOD(Remove)(int operationIndex, VARIANT_BOOL* retVal);
	STDMETHOD(Clear)();
	STDMETHOD(get_Count)(int* retVal);
	STDMETHOD(get_Operation)(int operationIndex, tkFieldStatOperation* retVal);
	STDMETHOD(get_FieldIndex)(int operationIndex, int* retVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_FieldName)(int operationIndex, BSTR* retVal);
	STDMETHOD(Validate)(IShapefile* sf, VARIANT_BOOL* retVal);
	STDMETHOD(get_OperationIsValid)(int operationIndex, VARIANT_BOOL* retVal);
	STDMETHOD(get_OperationIsValidReason)(int operationIndex, tkFieldOperationValidity* retVal);

private:
	void ErrorMessage(long ErrorCode);
	long _lastErrorCode;
	BSTR _key;

public:
	std::vector<FieldOperation*> _operations;
};

OBJECT_ENTRY_AUTO(__uuidof(FieldStatOperations), CFieldStatOperations)
