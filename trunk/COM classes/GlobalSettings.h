/**************************************************************************************
 * File name: GlobalSettings.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CGlobalSettings
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
 // Sergei Leschinski (lsu) 08 aug 2011 - created the file.

#pragma once
#include "MapWinGIS.h"
#include <map>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CGlobalSettings
class ATL_NO_VTABLE CGlobalSettings :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CGlobalSettings, &CLSID_GlobalSettings>,
	public IDispatchImpl<IGlobalSettings, &IID_IGlobalSettings, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CGlobalSettings()
	{
		
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_GLOBALSETTINGS)

	BEGIN_COM_MAP(CGlobalSettings)
		COM_INTERFACE_ENTRY(IGlobalSettings)
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
	STDMETHOD(get_MinPolygonArea)(double* retVal);
	STDMETHOD(put_MinPolygonArea)(double newVal);
	STDMETHOD(get_MinAreaToPerimeterRatio)(double* retVal);
	STDMETHOD(put_MinAreaToPerimeterRatio)(double newVal);
	STDMETHOD(get_ClipperGcsMultiplicationFactor)(DOUBLE* pVal);
	STDMETHOD(put_ClipperGcsMultiplicationFactor)(DOUBLE newVal);
	STDMETHOD(CreatePoint)(IPoint** retVal);
	STDMETHOD(get_ShapefileFastMode)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ShapefileFastMode)(VARIANT_BOOL newVal);
	STDMETHOD(get_InvalidShapesBufferDistance)(DOUBLE* pVal);
	STDMETHOD(put_InvalidShapesBufferDistance)(DOUBLE newVal);
	STDMETHOD(get_ShapefileFastUnion)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ShapefileFastUnion)(VARIANT_BOOL newVal);
	STDMETHOD(get_GdalLastErrorNo)(tkGdalError* pVal);
	STDMETHOD(get_GdalLastErrorType)(tkGdalErrorType* pVal);
	STDMETHOD(get_GdalLastErrorMsg)(BSTR* pVal);
	STDMETHOD(ResetGdalError)(void);
	STDMETHOD(get_GdalReprojectionErrorMsg)(BSTR* pVal);
	STDMETHOD(get_LabelsSmoothingMode)(tkSmoothingMode* pVal);
	STDMETHOD(put_LabelsSmoothingMode)(tkSmoothingMode newVal);
	STDMETHOD(get_LabelsCompositingQuality)(tkCompositingQuality* pVal);
	STDMETHOD(put_LabelsCompositingQuality)(tkCompositingQuality newVal);
	STDMETHOD(put_ShortUnitsString)(tkLocalizedStrings unit, BSTR localizedString);
	STDMETHOD(get_ShortUnitsString)(tkLocalizedStrings unit, BSTR* retVal);
	STDMETHOD(put_ZoomToFirstLayer)(VARIANT_BOOL newVal);
	STDMETHOD(get_ZoomToFirstLayer)(VARIANT_BOOL* pVal);
	STDMETHOD(put_LabelsCollisionMode)(tkCollisionMode newVal);
	STDMETHOD(get_LabelsCollisionMode)(tkCollisionMode* pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(GlobalSettings), CGlobalSettings)
