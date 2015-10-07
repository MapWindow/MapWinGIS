#include "stdafx.h"
#include "Utils.h"
#include "Shapefile.h"

// ********************************************************
//		CreateOutputShapefile ()
// ********************************************************
IShapefile* CUtils::CreateOutputShapefile(BSTR outputFilename, IShapefile* source)
{
	if (Utility::FileExistsW(outputFilename))
	{
		ErrorMessage(tkFILE_EXISTS);
		return false;
	}

	IShapefile* result = NULL;
	source->Clone(&result);

	VARIANT_BOOL vb;
	result->SaveAsEx(outputFilename, VARIANT_TRUE, VARIANT_FALSE, &vb);

	if (vb) {
		result->StartAppendMode(&vb);
	}

	if (!vb) {
		result->Release();
		result = NULL;
	}

	return result;
}

// ********************************************************
//		FixUpShapes ()
// ********************************************************
STDMETHODIMP CUtils::FixUpShapes(IShapefile* subject, VARIANT_BOOL SelectedOnly, BSTR outputFilename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!subject) 
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;	
	}

	IShapefile* result = CreateOutputShapefile(outputFilename, subject);
	if (!result) {
		return S_OK;
	}

	*retVal = ((CShapefile*)subject)->FixupShapesCore(SelectedOnly, result);

	result->StopAppendMode();

	VARIANT_BOOL vb;
	result->Close(&vb);

	return S_OK;
}