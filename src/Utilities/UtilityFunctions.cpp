#include "StdAfx.h"
#include <iterator>
#include <fstream>
#include "macros.h"
#include <vector>
#include <algorithm>
#include <string>

// ReSharper disable CppUseAuto

namespace Utility
{
	_locale_t m_locale = nullptr;
	_locale_t GetLocale()
	{
		if (!m_locale) {
			m_locale = _create_locale(LC_ALL, "C");
		}
		return m_locale;
	}

#pragma region String conversion

	// ********************************************************
	//    XmlFilenameToUnicode()
	// ********************************************************
	CStringW XmlFilenameToUnicode(CStringA s, bool utf8) {
		USES_CONVERSION;
		CStringW unicode = utf8 ? CA2W(s, CP_UTF8) : A2W(s);
		return unicode;
	}

	// ********************************************************
	//    ConvertToUtf8()
	// ********************************************************
	CStringA ConvertToUtf8(CStringW unicode) {
		USES_CONVERSION;
		CStringA utf8 = CW2A(unicode, CP_UTF8);
		return utf8;
	}

	// ********************************************************
	//    ConvertFromUtf8()
	// ********************************************************
	CStringW ConvertFromUtf8(CStringA utf8) {
		USES_CONVERSION;
		CStringW unicode = CA2W(utf8, CP_UTF8);
		return unicode;
	}

	// ********************************************************
	//    StringToWideChar()
	// ********************************************************
	WCHAR* StringToWideChar(CString s)
	{
		WCHAR* wText = nullptr;
		int size = MultiByteToWideChar(CP_ACP, 0, s.GetString(), -1, nullptr, 0);
		wText = new WCHAR[size];
		MultiByteToWideChar(CP_ACP, 0, s.GetString(), -1, wText, size);
		return wText;
	}

	// ********************************************************
	//		SYS2A
	// ********************************************************
	//ConservesStackMemory
	char* Utility::SYS2A(BSTR str)
	{
		USES_CONVERSION;
		char* result = nullptr;
		char* stackVersion = OLE2A(str);
		if (stackVersion)
		{
			result = new char[_tcslen(stackVersion) + 1];
			memcpy(result, stackVersion, _tcslen(stackVersion));
			result[_tcslen(stackVersion)] = '\0';
		}
		else
		{
			result = new char[1];
			result[0] = '\0';
		}
		return result;
	}

	// ********************************************************
	//		ConvertBSTRToLPSTR
	// ********************************************************
	//Rob Cairns 29-Aug-2009
	char* ConvertBSTRToLPSTR(BSTR bstrIn, UINT codePage /* = CP_ACP */)
	{
		LPSTR pszOut = nullptr;
		if (bstrIn != nullptr)
		{
			int nInputStrLen = SysStringLen(bstrIn);

			// Double NULL Termination
			int nOutputStrLen = WideCharToMultiByte(codePage, 0, bstrIn, nInputStrLen, nullptr, 0, nullptr, nullptr) + 2;
			pszOut = new char[nOutputStrLen];

			if (pszOut)
			{
				memset(pszOut, 0x00, sizeof(char) * nOutputStrLen);
				WideCharToMultiByte(codePage, 0, bstrIn, nInputStrLen, pszOut, nOutputStrLen, nullptr, nullptr);
			}
		}
		return pszOut;
	}

	// ********************************************************
	//		Variant2BSTR
	// ********************************************************
	//	Converting variant to bstr; only several values are considered
	// should be replaced by CComVariant.CopyTo
	BSTR Variant2BSTR(VARIANT* val, CString floatFormat)
	{
		if (val->vt == VT_BSTR)
		{
			return OLE2BSTR(val->bstrVal);
		}
		else if (val->vt == VT_I4)
		{
			CString str;
			str.Format("%d", val->lVal);
			return str.AllocSysString();
		}
		else if (val->vt == VT_R8)
		{
			CString str;
			str.Format(floatFormat, val->dblVal);
			return str.AllocSysString();
		}
		else //if( val.vt == VT_NULL )
		{
			return A2BSTR("");
		}
	}

	//from http://www.codeproject.com/Articles/260/Case-Insensitive-String-Replace
	// instr:  string to search in.
	// oldstr: string to search for, ignoring the case.
	// newstr: string replacing the occurrences of oldstr.
	CString ReplaceNoCase(LPCTSTR instr, LPCTSTR oldstr, LPCTSTR newstr)
	{
		CString output(instr);

		// lowercase-versions to search in.
		CString input_lower(instr);
		CString oldone_lower(oldstr);
		input_lower.MakeLower();
		oldone_lower.MakeLower();

		// search in the lowercase versions,
		// replace in the original-case version.
		int pos = 0;
		while ((pos = input_lower.Find(oldone_lower, pos)) != -1) {

			// need for empty "newstr" cases.
			input_lower.Delete(pos, lstrlen(oldstr));
			input_lower.Insert(pos, newstr);

			// actually replace.
			output.Delete(pos, lstrlen(oldstr));
			output.Insert(pos, newstr);
		}

		return output;
	}
#pragma endregion

#pragma region Files
	// *******************************************************************
	//		fileExists()
	// *******************************************************************
	BOOL Utility::FileExists(CString filename)
	{
		if (filename.GetLength() <= 0)
			return FALSE;

		FILE* file = fopen(filename, "rb");

		if (file == nullptr)
			return FALSE;
		else
		{
			fclose(file);
			return TRUE;
		}
	}

	// *******************************************************************
	//		get_FileSize()
	// *******************************************************************
	long Utility::GetFileSize(CStringW filename)
	{
		if (filename.GetLength() <= 0)
			return FALSE;

		FILE* file = _wfopen(filename, L"rb");

		long size = 0;
		if (file)
		{
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			fclose(file);
		}

		return size;
	}

	// *******************************************************************
	//		dirExists()
	// *******************************************************************
	bool DirExists(CStringW path)
	{
		DWORD ftyp = GetFileAttributesW(path);
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return false;  //something is wrong with your path!
		return (ftyp & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
	}

	// *******************************************************************
	//		Utility::fileExistsW()
	// *******************************************************************
	bool FileExistsW(CStringW filename)
	{
		if (filename.GetLength() <= 0)
			return FALSE;

		FILE* file = _wfopen(filename, L"rb");

		if (file == nullptr)
		{
			return false;
		}
		else
		{
			fclose(file);
			return true;
		}
	}

	// *******************************************************************
	//		Utility::fileExistsUnicode()
	// *******************************************************************
	bool Utility::FileExistsUnicode(CStringW filename)
	{
		if (filename.GetLength() <= 0)
			return FALSE;

		USES_CONVERSION;

		FILE* file = _wfopen(filename, L"rb");

		if (file == nullptr) {
			return false;
		}

		fclose(file);
		return true;

	}

	// returns list paths for all parent folders for current file from inner most to top most
	void GetFoldersList(CStringW path, std::vector<CStringW>& list)
	{
		for (int i = path.GetLength() - 1; i >= 0; i--)
		{
			CStringW mid = path.Mid(i, 1);
			if (mid == '\\' && i - 1 > 0)
			{
				list.push_back(path.Left(i));
			}
		}
	}

	// *********************************************************
	//	     get_RelativePath()
	// *********************************************************
	CStringW Utility::GetRelativePath(CStringW ProjectName, CStringW Filename)
	{
		CStringW drive1, drive2;
		wchar_t drive[1024];
		wchar_t dir[1024];
		wchar_t name[1024];
		wchar_t ext[1024];

		_wsplitpath(ProjectName, drive, dir, name, ext);
		drive1 = drive;

		_wsplitpath(Filename, drive, dir, name, ext);
		drive2 = drive;

		if (drive1 != drive2)
		{
			// files are on the different drives, no way to get a relative path
			return Filename;
		}
		else
		{
			std::vector<CStringW> list1;
			std::vector<CStringW> list2;

			GetFoldersList(ProjectName, list1);
			GetFoldersList(Filename, list2);

			// searching for the match (inner-most folder common to both files)
			unsigned int i, j;
			for (i = 0; i < list1.size(); i++)
			{
				for (j = 0; j < list2.size(); j++)
				{
					if (_wcsicmp(list1[i], list2[j]) == 0)
					{
						goto match;
					}
				}
			}

		match:
			CStringW path = L"";
			for (unsigned int k = 0; k < i; k++)
			{
				// going to the parent folder 
				path += L"..\\";
			}

			// excluding folder part from the path
			path += Filename.Mid(list1[i].GetLength() + 1);
			return path;
		}
	}

	// *********************************************************
	//	     GetFolderFromPath()
	// *********************************************************
	CStringW Utility::GetFolderFromPath(CStringW path)
	{
		CStringW result = path;
		for (int i = path.GetLength() - 1; i > 0; i--)
		{
			if (path.Mid(i, 1) == '\\')
			{
				result = path.Left(i);	// -1
				if (result.GetLength() == 2)
					result += "\\";
				break;
			}
		}
		return result;
	}

	// *********************************************************
	//	     GetNameFromPathWoExtension()
	// *********************************************************
	CStringW Utility::GetNameFromPathWoExtension(CStringW path)
	{
		path = GetNameFromPath(path);
		return GetPathWOExtension(path);
	}

	// *********************************************************
	//	     GetNameFromPath()
	// *********************************************************
	CStringW Utility::GetNameFromPath(CStringW path)
	{
		for (int i = path.GetLength() - 1; i > 0; i--)
		{
			if (path.Mid(i, 1) == '\\')
			{
				int length = path.GetLength() - 1 - i;
				return length > 0 ? path.Right(length) : L"";
			}
		}
		return path;
	}

	// *********************************************************
	//	     EndsWith()
	// *********************************************************
	bool EndsWith(CStringW path, CStringW ext)
	{
		if (path.GetLength() < ext.GetLength() + 1 || ext.GetLength() < 2) {
			return false;
		}
		else
		{
			return path.Right(ext.GetLength()).CompareNoCase(ext) == 0;
		}
	}

	// *********************************************************
	//	     GetPathWOExtension()
	// *********************************************************
	CStringW GetPathWOExtension(CStringW path)
	{
		for (int i = path.GetLength() - 1; i > 0; i--)
		{
			if (path.Mid(i, 1) == ".")
			{
				return path.Left(i);
			}
		}
		return path;
	}

	// ****************************************************
	//   ReadFileToBuffer
	// ****************************************************
	// Reads the content of the file to buffer, return the number of bytes read
	int ReadFileToBuffer(CStringW filename, unsigned char** buffer)
	{
		FILE* file = _wfopen(filename, L"rb");

		long size = 0;
		if (file)
		{
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			if (size > 0)
			{
				*buffer = new unsigned char[size];
				rewind(file);
				size = fread(*buffer, sizeof(unsigned char), size, file);
			}
			fclose(file);
		}
		return size;
	}

	int ReadFileToBuffer(CStringW filename, char** buffer)
	{
		FILE* file = _wfopen(filename, L"rb");

		long size = 0;
		if (file)
		{
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			if (size > 0)
			{
				*buffer = new char[size];
				rewind(file);
				size = fread(*buffer, sizeof(char), size, file);
			}
			fclose(file);
		}
		return size;
	}

#define _SECOND ((__int64) 10000000)

	// ********************************************************
	//     Utility::CompareCreationTime()
	// ********************************************************
	// returns: 1 = first file younger, -1 = vice versa, 0 = equal age; any other value = error
	bool IsFileYounger(CStringW filename, CStringW thanFilename)
	{
		FILETIME time1, time2;
		if (GetFileCreationTime(filename, time1) && GetFileCreationTime(thanFilename, time2))
		{
			// subtract several seconds
			ULONGLONG qwResult;

			// Copy the time into a quadword.
			qwResult = (((ULONGLONG)time2.dwHighDateTime) << 32) + time2.dwLowDateTime;

			// Add 30 days.
			qwResult -= 10 * _SECOND;

			// Copy the result back into the FILETIME structure.
			time2.dwLowDateTime = (DWORD)(qwResult & 0xFFFFFFFF);
			time2.dwHighDateTime = (DWORD)(qwResult >> 32);

			int val = CompareFileTime(&time1, &time2);
			return val == 1;
		}
		return false;
	}

	// ********************************************************
	//     Utility::get_FileCreationTime()
	// ********************************************************
	bool GetFileCreationTime(CStringW filename, FILETIME& time)
	{
		_WIN32_FILE_ATTRIBUTE_DATA data;
		if (GetFileAttributesExW(filename, GetFileExInfoStandard, &data))
		{
			time = data.ftCreationTime;
			return true;
		}
		return false;
	}

	// ********************************************************
	//    RemoveFile()
	// ********************************************************
	bool RemoveFile(CStringW filename)
	{
		if (Utility::FileExistsW(filename))
		{
			return _wremove(filename) == 0;
		}
		else {
			return true;	// no file and therefore no problem
		}
	}

	// ********************************************************
	//    getProjectionFileName()
	// ********************************************************
	CStringW GetProjectionFilename(CStringW dataSourceName)
	{
		return ChangeExtension(dataSourceName, L"prj");
	}

	// ********************************************************
	//    ChangeExtension()
	// ********************************************************
	CStringW ChangeExtension(CStringW filename, CStringW ext)
	{
		int theDot = filename.ReverseFind('.');
		if (theDot < 0)
			return filename + ext;
		return filename.Left(theDot + 1) + ext;
	}

	// ********************************************************
	//    GetTempFilename()
	// ********************************************************
	CString Utility::GetTempFilename(CString extensionWithLeadingPoint)
	{
		char* tmpfname = new char[MAX_BUFFER];
		char* tmppath = new char[MAX_PATH + MAX_BUFFER + 1];

		GetTempPath(MAX_PATH, tmppath);

		//tmpnam(tmpfname);
		// replacing tmpnam with the Windows call GetTempFileName
		// because, at least under certain circumstances, tmpnam was
		// returning a name including a path, which when concatenated
		// with tmppath, resulted in an invalid filename.
		::GetTempFileName(tmppath, "", 0, tmpfname);

		//strcat(tmppath, tmpfname);
		//strcat(tmppath, extensionWithLeadingPoint);

		CString result = tmpfname;
		result.MakeLower().Replace(".tmp", extensionWithLeadingPoint);

		delete[] tmpfname;
		delete[] tmppath;

		return result;
	}
#pragma endregion

#pragma region Unit conversion

	// ****************************************************************
	//		GetLocalizedUnitsText()
	// ****************************************************************
	CStringW Utility::GetLocalizedUnitsText(tkUnitsOfMeasure units)
	{
		switch (units)
		{
		case umMiles:
			return m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsMiles);
		case umFeets:
			return m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsFeet);
		case umMeters:
			return m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsMeters);
		case umKilometers:
			return m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsKilometers);
		default:
			USES_CONVERSION;
			return A2W(Utility::GetUnitOfMeasureText(units));
		}
	}

	// ****************************************************************
	//		GetUnitOfMeasureText
	// ****************************************************************
	// Returns the short name for units of measure
	CString Utility::GetUnitOfMeasureText(tkUnitsOfMeasure units)
	{
		switch (units)
		{
		case umDecimalDegrees:
			return "deg.";
		case umMiliMeters:
			return "mm";
		case umCentimeters:
			return "cm";
		case umInches:
			return "inches";
		case umFeets:
			return "feet";
		case umYards:
			return "yards";
		case umMeters:
			return "m";
		case umMiles:
			return "miles";
		case umKilometers:
			return "km";
		default:
			return "units";
		}
	}

	// **********************************************************
	//			get_ConversionFactor()
	// **********************************************************
	/// Returns the conversion factor between the map units and inches
	double Utility::GetConversionFactor(tkUnitsOfMeasure units)
	{
		switch (units)
		{
			// calculated considering sphere with radius 6378137 km, i.e. the one used WGS84/Google Mercator projection
			// http://spatialreference.org/ref/sr-org/7483/html/
			// cf = (2 * pi * R) / 360 / 0.0254
		case umDecimalDegrees:	return 4382657.117845416246;

		case umMeters:			return 39.3700787;
		case umCentimeters:		return 0.393700787;
		case umFeets:			return 12.0;
		case umInches:			return 1.0;
		case umKilometers:		return 39370.0787;
		case umMiles:			return 63360;
		case umMiliMeters:		return 0.0393700787;
		case umYards:			return 36.0;
		default:				return 0.0;
		}
	}

	// **********************************************************
	//			ConvertDistance()
	// **********************************************************
	bool Utility::ConvertDistance(tkUnitsOfMeasure source, tkUnitsOfMeasure target, double& value)
	{
		value *= Utility::GetConversionFactor(source);	// in inches
		const double factor = Utility::GetConversionFactor(target);
		if (factor != 0.0)
		{
			value /= factor;
			return true;
		}

		value = 0.0;
		return false;
	}

#pragma endregion

#pragma region Numbers

	double SquareMetersPerSquareMile()
	{
		return 2589975.2356;
	}

	double SquareMetersPerAcre()
	{
		return 4046.8564224;		// according to International yard and pound agreement (1959)
	}

	double SquareMetersPerSquareFoot()
	{
		return 0.09290304;
	}

	// ****************************************************************
	//		FormatArea()
	// ****************************************************************
	CStringW Utility::FormatArea(double area, bool unknownUnits, tkAreaDisplayMode units, int precision)
	{
		CStringW str;
		area = abs(area);

		CStringW format = GetUnitsFormat(precision);

		if (!unknownUnits)
		{
			tkLocalizedStrings localizedUnits;

			switch (units)
			{
			case admMetric:
			{
				if (area < 1000.0)
				{
					localizedUnits = lsSquareMeters;
				}
				else if (area < 10000000.0)
				{
					area /= 10000.0;
					localizedUnits = lsHectars;
				}
				else
				{
					area /= 1000000.0;
					localizedUnits = lsSquareKilometers;
				}
				break;
			}
			case admHectars:
			{
				area /= 10000.0;
				localizedUnits = lsHectars;
				break;
			}
			case admAmerican:
			{
				double area2 = area / SquareMetersPerSquareMile();
				localizedUnits = lsSquareMiles;

				if (area2 < 100.0)
				{
					area2 = area / SquareMetersPerAcre();
					localizedUnits = lsAcres;

					if (area2 < 1.0)
					{
						area2 = area / SquareMetersPerSquareFoot();
						localizedUnits = lsSquareFeet;
					}
				}

				area = area2;

				break;
			}
			default:
				return str;
			}

			str.Format(format, area, m_globalSettings.GetLocalizedString(localizedUnits));
		}
		else
		{
			str.Format(format, area, m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsSquareMapUnits));
		}

		return str;
	}

	// *********************************************************
	//		GetNumberFormat()
	// *********************************************************
	CStringW Utility::GetUnitsFormat(int precision)
	{
		CStringW temp;
		temp.Format(L"%d", precision);
		return L"%." + temp + L"f %s";
	}

	// *********************************************************
	//		FormatNumber()
	// *********************************************************
	CString Utility::FormatNumber(double val, CString& sFormat)
	{
		CString s;
		if (val > 1000000000.0 && val < 1000000000000.0)
		{
			s.Format("%.2f", val / 1000000000.0);
			if (s.GetLength() > 5)
				s.Delete(5, 1);
			s += "b";
		}
		else if (val > 1000000.0)
		{
			s.Format("%.2f", val / 1000000.0);
			if (s.GetLength() > 5)
				s.Delete(5, 1);
			s += "m";
		}
		else if (val > 1000.0)
		{
			s.Format("%.2f", val / 1000.0);
			if (s.GetLength() > 5)
				s.Delete(5, 1);
			s += "k";
		}
		else
		{
			s.Format(sFormat, val);
		}
		return s;
	}

	// *********************************************************
	// Shade1974 Jan 10, 2006
	// Explicit casting to int using rounding
	// *********************************************************
	int Utility::Rint(double value)
	{
		if (value < 0.0)
			value -= 0.5;
		else
			value += 0.5;
		int val = static_cast<int>(value);
		return val;
	}

	// *********************************************************
	//	     Factorial()
	// *********************************************************
	int Utility::Factorial(int n)
	{
		int ret = 1;
		for (int i = 1; i <= n; ++i)
			ret *= i;
		return ret;
	}

	// *********************************************************
	//	     atof_custom()
	// *********************************************************
	double atof_custom(CString s)
	{
		// as long as global locale set to std::locale("C") in MapWinGIS.cpp
		// it's enough just to replace , by .
		// if user defined locale would be used std::locale("") the logic should be more complex
		s.Replace(',', '.');
		double val = _atof_l(s, m_locale);
		return val;
	}
	double wtof_custom(CStringW s)
	{
		// as long as global locale set to std::locale("C") in MapWinGIS.cpp
		// it's enough just to replace , by .
		// if user defined locale would be used std::locale("") the logic should be more complex
		s.Replace(',', '.');
		double val = _wtof_l(s, m_locale);
		return val;
	}

	double FloatRound(double doValue, int nPrecision)
	{
		static const double doBase = 10.0;
		double doComplete5, doComplete5i;

		doComplete5 = doValue * pow(doBase, (double)(nPrecision + 1));

		if (doValue < 0.0)
			doComplete5 -= 5.0;
		else
			doComplete5 += 5.0;

		doComplete5 /= doBase;
		modf(doComplete5, &doComplete5i);

		return doComplete5i / pow(doBase, (double)nPrecision);
	}

	bool FloatsEqual(const float& a, const float& b)
	{
		return (fabs(a - b) <= 1.0e-20f);
	}
#pragma endregion

#pragma region Gdi	

	// ***********************************************************
	//		GetEncoderClsid()
	// ***********************************************************
	// Returns encoder for the specified image format
	// The following call should be used for PNG fromat, for example: GetEncoderClsid(L"png", &pngClsid);
	int Utility::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes


		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;  // Failure

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == nullptr)
			return -1;  // Failure

		GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}
		}

		free(pImageCodecInfo);
		return -1;  // Failure
	}

	Gdiplus::Font* GetGdiPlusFont(CString name, float size)
	{
		WCHAR* wFontName = StringToWideChar("Arial");
		Gdiplus::FontFamily family(wFontName);
		Gdiplus::Font* font = new Gdiplus::Font(&family, (Gdiplus::REAL)size);
		delete wFontName;
		return font;
	}

	void Utility::ClosePointer(Gdiplus::Bitmap** ptr)
	{
		if (*ptr) {
			delete* ptr;
			*ptr = nullptr;
		}
	}

	void Utility::ClosePointer(Gdiplus::Font** ptr)
	{
		if (*ptr) {
			delete* ptr;
			*ptr = nullptr;
		}
	}

	// **************************************************
	//	SaveBitmap
	// **************************************************
	// Saves provided array of pixels as png image (uses GDI+)
	bool Utility::SaveBitmap(int width, int height, unsigned char* pixels, BSTR outputName)
	{
		int pad = (width * 24) % 32;
		if (pad != 0)
		{
			pad = 32 - pad;
			pad /= 8;
		}

		BITMAPINFOHEADER bih;
		bih.biCompression = 0;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;
		bih.biPlanes = 1;
		bih.biSize = sizeof(BITMAPINFOHEADER);

		bih.biBitCount = 24;
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biSizeImage = (width * 3 + pad) * height;

		BITMAPINFO bif;
		bif.bmiHeader = bih;

		int nBytesInRow = width * 3 + pad;

		// copying bits
		unsigned char* bitsNew = new unsigned char[nBytesInRow * height];
		for (int i = 0; i < height; i++)
			memcpy(&bitsNew[i * nBytesInRow], &pixels[i * width * 3], width * 3);

		// saing the image
		Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(&bif, (void*)bitsNew);

		CLSID pngClsid;
		GetEncoderClsid(L"png", &pngClsid);	// perhaps some other formats ?
		USES_CONVERSION;
		Gdiplus::Status status = bmp->Save(OLE2W(outputName), &pngClsid, nullptr);

		if (bmp)
		{
			delete bmp;
		}
		if (bitsNew)
		{
			delete[] bitsNew;
		}

		return (status == Gdiplus::Ok);
	}

	DWORD* Utility::cvtUCharToDword(long inp, int& num)
	{   /* Chris Michaelis and Michelle Hospodarsky 2-11-2004 */
		/* this function creates a DWORD[] from a long : used to create the custom pen for a custom stipple*/
		/* the first digit in inp is the multiplier for the remainder of the digits	*/

		std::vector<long> temp;
		int multiplier;
		char inpStr[33];
		_ltoa(inp, inpStr, 10);

		int iter = 0;
		for (; inpStr[iter] != 0; iter++)
		{
			temp.push_back(inpStr[iter]);
		}

		int size = temp.size();
		DWORD* output = new DWORD[size];

		//get the multiplier
		multiplier = atoi((char*)&temp[0]);

		for (int i = 1; i < size; i++)
		{
			output[i - 1] = multiplier * atoi((char*)&temp[i]); // multiply to enhance value since range = 0 - 9
		}

		/* get the number of elements*/
		num = temp.size() - 1; // take off one for the multiplier and one for the null value
		temp.clear(); // be frugal with memory..

		return output;
	}

	// ***********************************************************
	//		ChangeBrightness
	// ***********************************************************
	OLE_COLOR Utility::ChangeBrightness(OLE_COLOR color, int shiftValue)
	{
		short r = GetRValue(color) + shiftValue;	if (r < 0) r = 0;	if (r > 255) r = 255;
		short g = GetGValue(color) + shiftValue;	if (g < 0) g = 0;	if (g > 255) g = 255;
		short b = GetBValue(color) + shiftValue;	if (b < 0) b = 0;	if (b > 255) b = 255;
		return RGB(r, g, b);
	}

	byte Utility::GetBrightness(OLE_COLOR color)
	{
		return ((short)GetRValue(color) + (short)GetGValue(color) + (short)GetBValue(color)) / 3;
	}

	// ***********************************************************
	//		ChangeBrightness
	// ***********************************************************
	Gdiplus::Color Utility::ChangeBrightness(OLE_COLOR color, int shiftValue, long alpha)
	{
		OLE_COLOR clr2 = ChangeBrightness(color, shiftValue);
		Gdiplus::Color clr(alpha | BGR_TO_RGB(clr2));
		return clr;
	}

	// ***********************************************************
	//		OleColor2GdiPlus
	// ***********************************************************
	Gdiplus::Color Utility::OleColor2GdiPlus(OLE_COLOR color, BYTE alpha)
	{
		Gdiplus::Color clr(alpha << 24 | BGR_TO_RGB(color));
		return clr;
	}

	Gdiplus::Color Utility::OleColor2GdiPlus(OLE_COLOR color)
	{
		Gdiplus::Color clr(255 << 24 | BGR_TO_RGB(color));
		return clr;
	}

#pragma endregion

#pragma region Gdal

	// ********************************************************
	//     CPLCreateXMLAttributeAndValue()
	// ********************************************************
	CPLXMLNode* Utility::CPLCreateXMLAttributeAndValue(CPLXMLNode* psParent, const char* pszName, CStringW valueW)
	{
		CStringA valueA = Utility::ConvertToUtf8(valueW);
		CPLXMLNode* psNode = CPLCreateXMLNode(psParent, CXT_Attribute, pszName);
		CPLCreateXMLNode(psNode, CXT_Text, valueA);
		return psNode;
	}

	CPLXMLNode* Utility::CPLCreateXMLAttributeAndValue(CPLXMLNode* psParent, const char* pszName, const char* pszValue)
	{
		CPLXMLNode* psNode = CPLCreateXMLNode(psParent, CXT_Attribute, pszName);
		CPLCreateXMLNode(psNode, CXT_Text, pszValue);
		return psNode;
	}

	CPLXMLNode* Utility::CPLCreateXMLAttributeAndValue(CPLXMLNode* psParent, const char* pszName, int nValue)
	{
		CString temp;
		temp.Format("%d", nValue);
		return CPLCreateXMLAttributeAndValue(psParent, pszName, temp);
	}

	CPLXMLNode* Utility::CPLCreateXMLAttributeAndValue(CPLXMLNode* psParent, const char* pszName, double rValue)
	{
		CString temp;
		temp.Format("%f", rValue);
		return CPLCreateXMLAttributeAndValue(psParent, pszName, temp);
	}

	// ********************************************************
	//     CPLXMLChildrentCount()
	// ********************************************************
	int Utility::CPLXMLChildrentCount(CPLXMLNode* psParent)
	{
		if (!psParent) {
			return 0;
		}

		int count = 0;
		CPLXMLNode* node = psParent->psChild;
		while (node)
		{
			count++;
			node = node->psNext;
		}

		return count;
	}

#pragma endregion

	// ********************************************************
	//	  getCurrentYear
	// ********************************************************
	int GetCurrentYear()
	{
		const time_t curTime = time(nullptr);
		struct tm* tmData = localtime(&curTime);
		return tmData->tm_year + 1900;
	}

	// ********************************************************
	//    OpenLog()
	// ********************************************************
	void OpenLog(ofstream& logger, CStringW path, CStringW name)
	{
		if (logger.is_open())
		{
			logger.flush();
			logger.close();
		}

		if (!Utility::DirExists(path))
			_wmkdir(path);

		path += name;
		logger.open(path);

		Debug::WriteLine("Log opened: %d", logger.is_open());
		Debug::WriteLine("Log good: %d", logger.good());
	}

	// ************************************************************
	//		WriteXmlHeader()
	// ************************************************************
	void WriteXmlHeaderAttributes(CPLXMLNode* psTree, CString fileType)
	{
		USES_CONVERSION;
		Utility::CPLCreateXMLAttributeAndValue(psTree, "OcxVersion", GetFileVersionString());
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FileType", fileType);
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FileVersion", CPLString().Printf("%d", m_globalSettings.xmlFileVersion));
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FilenamesEncoding", CPLString().Printf(m_globalSettings.xmlFilenameEncoding));
	}

	// ****************************************************************** 
	//		GetFileVersionString
	// ****************************************************************** 
	CString GetFileVersionString()
	{
		wchar_t* path = new wchar_t[MAX_PATH + 1];
		GetModuleFileNameW(GetModuleInstance(), path, MAX_PATH);

		DWORD  verHandle = NULL;
		UINT   size = 0;
		LPBYTE lpBuffer = nullptr;
		DWORD  verSize = GetFileVersionInfoSizeW(path, &verHandle);
		CString result;

		if (verSize != NULL)
		{
			LPSTR verData = new char[verSize];

			if (GetFileVersionInfoW(path, verHandle, verSize, verData))
			{
				if (VerQueryValue(verData, "\\", (VOID FAR * FAR*) & lpBuffer, &size))
				{
					if (size)
					{
						VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
						if (verInfo->dwSignature == 0xfeef04bd)
						{
							int major = HIWORD(verInfo->dwFileVersionMS);
							int minor = LOWORD(verInfo->dwFileVersionMS);
							int build = HIWORD(verInfo->dwFileVersionLS);
							int sub = LOWORD(verInfo->dwFileVersionLS);
							result.Format("%d.%d.%d.%d", major, minor, build, sub);
						}
					}
				}
			}
			delete[] verData;
		}
		delete[] path;

		return result;
	}




	// ****************************************************************** 
	//		SerializeVariant
	// ****************************************************************** 
	void SerializeVariant(CPLXMLNode* node, CString elementName, VARIANT* val)
	{
		if (!node || !val) return;
		if (val->vt == VT_BSTR)
		{
			USES_CONVERSION;
			Utility::CPLCreateXMLAttributeAndValue(node, elementName, OLE2CA(val->bstrVal));
		}
		else if (val->vt == VT_R8)
		{
			Utility::CPLCreateXMLAttributeAndValue(node, elementName, CPLString().Printf("%f", val->dblVal));
		}
		else if (val->vt == VT_I4)
		{
			Utility::CPLCreateXMLAttributeAndValue(node, elementName, CPLString().Printf("%d", val->lVal));
		}
		else if (val->vt == VT_BOOL)
		{
			Utility::CPLCreateXMLAttributeAndValue(node, elementName, (val->boolVal == VARIANT_TRUE) ? "T" : "F");
		}
		else if (val->vt == VT_DATE)
		{
			CString cval;
			COleDateTime dt(val->date);
			cval.Format("%4d%2d%2d", dt.GetYear(), dt.GetMonth(), dt.GetDay());
			Utility::CPLCreateXMLAttributeAndValue(node, elementName, cval);
		}
	}

	// ****************************************************************** 
	//		DeserializeVariant
	// ****************************************************************** 
	void DeserializeVariant(CString sValue, FieldType fieldType, VARIANT* var)
	{
		switch (fieldType)
		{
		case STRING_FIELD:
			var->vt = VT_BSTR;
			var->bstrVal = A2BSTR(sValue);
			break;
		case INTEGER_FIELD:
			var->vt = VT_I4;
			var->lVal = atoi(sValue);
			break;
		case DOUBLE_FIELD:
			var->vt = VT_R8;
			var->dblVal = Utility::atof_custom(sValue);
			break;
		case BOOLEAN_FIELD:
			var->vt = VT_BOOL;
			var->boolVal = (sValue[0] == 'T') ? VARIANT_TRUE : VARIANT_FALSE;
		case DATE_FIELD:
			int m, d, y;
			y = atoi(sValue.Mid(0, 4));
			m = atoi(sValue.Mid(4, 2));
			d = atoi(sValue.Mid(6, 2));
			COleDateTime dt(y, m, d, 0, 0, 0);
			var->vt = VT_DATE;
			var->date = dt;
			break;
		}
	}

	// ************************************************
	//		InitGdiPlusFont
	// ************************************************
	void InitGdiPlusFont(Gdiplus::Font** font, CStringW name, float size)
	{
		*font = new Gdiplus::Font(name, size);
	}

	// ************************************************
	//		SerializeXmlTree
	// ************************************************
	bool SerializeAndDestroyXmlTree(CPLXMLNode* psTree, BSTR* retVal)
	{
		if (psTree)
		{
			char* buffer = CPLSerializeXMLTree(psTree);
			CPLDestroyXMLNode(psTree);
			if (buffer) {
				*retVal = A2BSTR(buffer);
				CPLFree(buffer);
				return true;
			}
		}
		*retVal = A2BSTR("");
		return false;
	}

	// ************************************************
	//		GetMapWinGISPath
	// ************************************************
	CStringW GetMapWinGISPath()
	{
		wchar_t* path = new wchar_t[MAX_PATH + 1];
		GetModuleFileNameW(GetModuleInstance(), path, MAX_PATH);
		CStringW pathW = path;
		delete[] path;
		return pathW;
	}

	inline BYTE toHex(const BYTE& x)
	{
		return x > 9 ? x + 55 : x + 48;
	}

	// ************************************************
	//		UrlEncode
	// ************************************************
	// http ://www.codeproject.com/Articles/1206/URLEncode
	CString UrlEncode(CString sIn)
	{
		CString sOut;

		const int nLen = sIn.GetLength() + 1;

		register LPBYTE pOutTmp = nullptr;
		LPBYTE pOutBuf = nullptr;
		register LPBYTE pInTmp = nullptr;
		LPBYTE pInBuf = (LPBYTE)sIn.GetBuffer(nLen);
		BYTE b = 0;

		//alloc out buffer
		pOutBuf = (LPBYTE)sOut.GetBuffer(nLen * 3 - 2);//new BYTE [nLen  * 3];

		if (pOutBuf)
		{
			pInTmp = pInBuf;
			pOutTmp = pOutBuf;

			// do encoding
			while (*pInTmp)
			{
				if (isalnum(*pInTmp))
					*pOutTmp++ = *pInTmp;
				else
					if (isspace(*pInTmp))
						*pOutTmp++ = '+';
					else
					{
						*pOutTmp++ = '%';
						*pOutTmp++ = toHex(*pInTmp >> 4);
						*pOutTmp++ = toHex(*pInTmp % 16);
					}
				pInTmp++;
			}
			*pOutTmp = '\0';
			//sOut=pOutBuf;
			//delete [] pOutBuf;
			sOut.ReleaseBuffer();
		}
		sIn.ReleaseBuffer();
		return sOut;
	}

	// ************************************************
	//		GetSocketErrorMessage
	// ************************************************
	CString GetSocketErrorMessage(DWORD socketError)
	{
		// msdn.microsoft.com/en-us/library/windows/desktop/ms680582(v=vs.85).aspx
		LPVOID lpMsgBuf;

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			socketError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, nullptr);

		CString s = (char*)lpMsgBuf;
		LocalFree(lpMsgBuf);

		return s;
	}

	// https://ideone.com/1RCZM8
	size_t split(const std::string& txt, std::vector<std::string>& strs, const char splittingChar)
	{
		std::string::size_type pos = txt.find(splittingChar);
		size_t initialPos = 0;
		strs.clear();

		// Decompose statement
		while (pos != std::string::npos) {
			strs.push_back(txt.substr(initialPos, pos - initialPos));
			initialPos = pos + 1;

			pos = txt.find(splittingChar, initialPos);
		}

		// Add the last one
		strs.push_back(txt.substr(initialPos, (((pos) < (txt.size())) ? (pos) : (txt.size())) - initialPos + 1));

		return strs.size();
	}
}

// ReSharper restore CppUseAuto
