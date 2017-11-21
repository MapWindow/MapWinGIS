//********************************************************************************************************
// File name: Utils_Projections.cpp
// Description: CUtils class implementation of functions related to Projection strings.
//********************************************************************************************************

#include "stdafx.h"
#include "Utils.h"

// path to projection csv
static CString csvPath;
// single instance of Projection Strings mapping
static unordered_map<int, CString> projectionStrings;

// determine the path of this OCX, independent of the calling executable
// https://stackoverflow.com/questions/6924195/get-dll-path-at-runtime
//
CStringW thisOcxPath()
{
	CStringW thisPath = L"";
	WCHAR path[MAX_PATH];
	HMODULE hm;
	if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPWSTR) &thisOcxPath, &hm))
	{
		GetModuleFileNameW(hm, path, sizeof(path));
		PathRemoveFileSpecW(path);
		thisPath = CStringW(path);
		if (!thisPath.IsEmpty() &&
			thisPath.GetAt(thisPath.GetLength() - 1) != '\\')
			thisPath += L"\\";
	}

	return thisPath;
}

bool CUtils::LoadProjectionStrings()
{
	static bool bLoaded = false;

	if (!bLoaded)
	{
		// in the release distribution, the gdal-data directory is right below us
		CString releasePath = thisOcxPath() + "gdal-data\\pcs.csv";
		// in the dev environment, it actually depends on the runtime version, and the bit-ness
		CString developPath = thisOcxPath() + "..\\..\\..\\support\\GDAL_SDK\\v120\\bin\\win32\\gdal-data\\pcs.csv";
								//ocx path = C:\dev\MapWinGIS\src\bin\Win32
								//csv path = C:\dev\MapWinGIS\support\GDAL_SDK\v120\bin\win32\gdal-data

		CStdioFile file;
		// first try standard release path
		csvPath = releasePath;
		if (!file.Open(csvPath, CFile::modeRead | CFile::typeText))
		{
			// for developers, try source-code path
			csvPath = developPath;
			if (!file.Open(csvPath, CFile::modeRead | CFile::typeText))
			{
				// for the purposes of error message, put path back to release
				csvPath = releasePath;
				// no error message here. instead, send error to callback when user calls lookup functions
				return false;
			}
		}

		CString nextLine;
		// throw away the first line
		file.ReadString(nextLine);
		// now read all definitions
		while (file.ReadString(nextLine))
		{
			// continue on error
			try
			{
				CString code, name;
				// find the first comma
				int commaPosition = nextLine.Find(",", 0);
				// strip off the SRID
				code = nextLine.Left(commaPosition);
				// the rest of the line (account for leading quote symbol)
				name = nextLine.Right(nextLine.GetLength() - commaPosition - 1 - 1);
				// strip off all to the right of the name (using the closing quote symbol)
				int quotePosition = name.Find("\"", 0);
				name = name.Left(quotePosition);

				// add to mapping
				projectionStrings.insert(std::pair<int, CString>(atoi((LPCSTR)code), name));
			}
			catch (...)
			{
			}
		}
		//
		bLoaded = true;
	}
	return bLoaded;
}

CString CUtils::customErrorMessage()
{
	return Debug::Format("Unable to reference projection list. Failed to load GDAL Projection list '%s'", csvPath);
}

// return the name of the projection specified by the Nad83 enumeration
STDMETHODIMP CUtils::GetNAD83ProjectionName(tkNad83Projection projectionID, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	try
	{
		// list check
		if (projectionStrings.empty())
		{
			ErrorMessage(tkFILE_NOT_OPEN, customErrorMessage());
			*retVal = A2BSTR("");
		}
		else
		{
			// return string mapped to specified ID (use 'at' rather than [] to check existence)
			*retVal = A2BSTR((LPCSTR)projectionStrings.at(projectionID));
		}
	}
	catch (...)
	{
		// 'at' method will throw exception if key does not exist
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS, Debug::Format("Invalid Projection ID specified: {0}", projectionID));
		*retVal = A2BSTR("");
	}

	return S_OK;
}

// return the name of the projection specified by the Wgs84 enumeration
STDMETHODIMP CUtils::GetWGS84ProjectionName(tkWgs84Projection projectionID, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	try
	{
		// list check
		if (projectionStrings.empty())
		{
			ErrorMessage(tkFILE_NOT_OPEN, customErrorMessage());
			*retVal = A2BSTR("");
		}
		else
		{
			// return string mapped to specified ID (use 'at' rather than [] to check existence)
			*retVal = A2BSTR((LPCSTR)projectionStrings.at(projectionID));
		}
	}
	catch (...)
	{
		// 'at' method will throw exception if key does not exist
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS, Debug::Format("Invalid Projection ID specified: {0}", projectionID));
		*retVal = A2BSTR("");
	}

	return S_OK;
}

// return the name of the projection specified by the SRID, which may be Nad83, Wgs84, or a Coordinate System ID
STDMETHODIMP CUtils::GetProjectionNameByID(int SRID, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	try
	{
		// list check
		if (projectionStrings.empty())
		{
			ErrorMessage(tkFILE_NOT_OPEN, customErrorMessage());
			*retVal = A2BSTR("");
		}
		else
		{
			// return string mapped to specified ID (use 'at' rather than [] to check existence)
			*retVal = A2BSTR((LPCSTR)projectionStrings.at(SRID));
		}
	}
	catch (...)
	{
		// 'at' method will throw exception if key does not exist
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS, Debug::Format("Invalid SRID specified: {0}", SRID));
		*retVal = A2BSTR("");
	}

	return S_OK;
}

STDMETHODIMP CUtils::GetProjectionList(tkProjectionSet projectionSets, VARIANT* list, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// guilty until proven innocent
	*retVal = VARIANT_FALSE;

	// list check
	if (projectionStrings.empty())
	{
		ErrorMessage(tkFILE_NOT_OPEN, customErrorMessage());
		*retVal = VARIANT_FALSE;
	}
	else
	{
		SAFEARRAY FAR* psa = NULL;
		SAFEARRAYBOUND sabound[1];
		sabound[0].lLbound = 0;

		int theSize = 0;
		if ((projectionSets & psAll_Projections) == psAll_Projections)
		{
			theSize = projectionStrings.size();
		}
		else
		{
			// may include NAD 83 strings
			if ((projectionSets & psNAD83_Subset) == psNAD83_Subset)
			{
				for each (pair<int, CString> p in projectionStrings)
				{
					if (p.second.Left(5) == "NAD83") theSize++;
				}
				//theSize += 881;
			}
			// may also include WGS 84 strings
			if ((projectionSets & psWGS84_Subset) == psWGS84_Subset)
			{
				for each (pair<int, CString> p in projectionStrings)
				{
					if (p.second.Left(6) == "WGS 84") theSize++;
				}
				//theSize += 248;
			}
		}

		// allocate the array
		sabound[0].cElements = theSize;
		psa = SafeArrayCreate(VT_BSTR, 1, sabound);

		if (psa)
		{
			BSTR* pBSTR = NULL;
			SafeArrayAccessData(psa, (void **)&pBSTR);

			CComBSTR comBSTR;
			int j = 0;
			for each (pair<int, CString> p in projectionStrings)
			{
				// include ?
				if (((projectionSets & psAll_Projections) == psAll_Projections) ||
					(((projectionSets & psNAD83_Subset) == psNAD83_Subset) && (p.second.Left(5) == "NAD83")) ||
					(((projectionSets & psWGS84_Subset) == psWGS84_Subset) && (p.second.Left(6) == "WGS 84")))
				{
					// create concatenated string
					comBSTR.Append((LPCSTR)Debug::Format("%d,%s", p.first, p.second));
					// copy to array
					pBSTR[j++] = comBSTR.Copy();
					// empty the string
					comBSTR.Empty();
				}
			}

			SafeArrayUnaccessData(psa);

			list->vt = VT_ARRAY | VT_BSTR;
			list->parray = psa;

			// something is there
			*retVal = VARIANT_TRUE;
		}
		else
		{
			// failed to allocate array
			ErrorMessage(tkCANT_ALLOC_MEMORY);
		}
	}

	return S_OK;
}
