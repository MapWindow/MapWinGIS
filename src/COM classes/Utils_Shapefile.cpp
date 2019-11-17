#include "stdafx.h"
#include "Utils.h"
#include "Shapefile.h"
#include "ShapefileHelper.h"

// ********************************************************
//		SaveOutputShapefile ()
// ********************************************************
VARIANT_BOOL CUtils::SaveOutputShapefile(BSTR outputFilename, IShapefile* sf, VARIANT_BOOL overwrite)
{
	if (Utility::FileExistsW(outputFilename))
	{
		if (!overwrite) {
			ErrorMessage(tkFILE_EXISTS);
			return VARIANT_FALSE;
		}
		else {
			if(!ShapefileHelper::Delete(OLE2W(outputFilename))) {
				return VARIANT_FALSE;
			}
		}
	}

	VARIANT_BOOL vb;
	sf->SaveAsEx(outputFilename, VARIANT_TRUE, VARIANT_FALSE, &vb);

	if (vb) {
		sf->StartAppendMode(&vb);
	}

	if (!vb) {
		sf->Release();
	}

	return vb;
}

// ********************************************************
//		CloseOutputShapefile ()
// ********************************************************
void CUtils::CloseOutputShapefile(IShapefile* sf)
{
	sf->StopAppendMode();

	VARIANT_BOOL vb;
	sf->Close(&vb);
}

// ********************************************************
//		CheckInputShapefile ()
// ********************************************************
bool CUtils::CheckInputShapefile(IShapefile* input)
{
	if (!input)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return false;
	}

	return true;
}

// ********************************************************
//		CloneInput ()
// ********************************************************
IShapefile* CUtils::CloneInput(IShapefile* input, BSTR outputFilename, VARIANT_BOOL overwrite)
{
	if (!CheckInputShapefile(input)) return S_OK;

	IShapefile* result = NULL;
	input->Clone(&result);

	if (!SaveOutputShapefile(outputFilename, result, overwrite)) {
		result->Release();
		return NULL;
	}

	return result;
}

// ********************************************************
//		FixUpShapes ()
// ********************************************************
STDMETHODIMP CUtils::FixUpShapes(IShapefile* subject, VARIANT_BOOL SelectedOnly, BSTR outputFilename, VARIANT_BOOL overwrite, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;
	
	IShapefile* result = CloneInput(subject, outputFilename, overwrite);
	if (!result)  {
		return S_OK;
	}

	*retVal = ((CShapefile*)subject)->FixupShapesCore(SelectedOnly, result);

	CloseOutputShapefile(result);

	return S_OK;
}

// ********************************************************
//		BufferByDistance ()
// ********************************************************
STDMETHODIMP CUtils::BufferByDistance(IShapefile* subject, DOUBLE Distance, LONG nSegments, VARIANT_BOOL SelectedOnly, 
						VARIANT_BOOL MergeResults, BSTR outputFilename, VARIANT_BOOL Overwrite, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!CheckInputShapefile(subject)) return S_OK;

	IShapefile* result = NULL;

	if (MergeResults) {
		// ShapefileHelper::CloneNoFields(subject, &result, SHP_POLYGON, true);
		// -----------------------------------------------
		//	 Creating output
		// -----------------------------------------------
		if (!ShapefileHelper::CloneNoFields(subject, &result, SHP_POLYGON, true))
		{
			// Get errorcode and pass the source:
			long errorCode;
			result->get_LastErrorCode(&errorCode);
			result = NULL;
			ErrorMessage(errorCode);
			return S_OK;
		}
	}
	else  {
		ShapefileHelper::CloneCore(subject, &result, SHP_POLYGON, false);
	}

	if (!SaveOutputShapefile(outputFilename, result, Overwrite)) {
		return S_OK;
	}

	*retVal = ((CShapefile*)subject)->BufferByDistanceCore(Distance, nSegments, SelectedOnly, MergeResults, result);

	CloseOutputShapefile(result);

	return S_OK;
}

// ********************************************************
//		ExplodeShapes ()
// ********************************************************
STDMETHODIMP CUtils::ExplodeShapes(IShapefile* subject, VARIANT_BOOL SelectedOnly, BSTR outputFilename, VARIANT_BOOL Overwrite, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	IShapefile* result = CloneInput(subject, outputFilename, Overwrite);
	if (!result)  {
		return S_OK;
	}

	*retVal = ((CShapefile*)subject)->ExplodeShapesCore(SelectedOnly, result);

	CloseOutputShapefile(result);

	return S_OK;
}

// ********************************************************
//		ExportSelection ()
// ********************************************************
STDMETHODIMP CUtils::ExportSelection(IShapefile* subject, BSTR outputFilename, VARIANT_BOOL Overwrite, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	IShapefile* result = CloneInput(subject, outputFilename, Overwrite);
	if (!result)  {
		return S_OK;
	}

	*retVal = ((CShapefile*)subject)->ExportSelectionCore(result);

	CloseOutputShapefile(result);

	return S_OK;
}
