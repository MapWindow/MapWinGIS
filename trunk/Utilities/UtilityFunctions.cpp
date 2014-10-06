#include "stdafx.h"
#include <iterator>
#include <fstream>
#include "macros.h"

namespace Utility
{
	static _locale_t m_locale = _create_locale(LC_ALL, "C");

#pragma region String conversion
	// *************************************************************
	//		Bstr2Char()
	// *************************************************************
	CStringA Bstr2Char(BSTR& bstr)
	{
		// check if UTF-8 conversion is needed
		return Utility::ConvertToUtf8(OLE2W(bstr));
	}

	CStringA CComBstr2Char(CComBSTR& bstr)
	{
		// check if UTF-8 conversion is needed
		return Utility::ConvertToUtf8(OLE2W(bstr));
	}
	
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
		WCHAR* wText = NULL;
		int size = MultiByteToWideChar(CP_ACP, 0, s.GetString(), -1, NULL, 0);
		wText = new WCHAR[size];
		MultiByteToWideChar(CP_ACP, 0, s.GetString(), -1, wText, size);
		return wText;
	}
	
	// ********************************************************
	//		SYS2A
	// ********************************************************
	//ConservesStackMemory
	char * Utility::SYS2A(BSTR str)
	{
		USES_CONVERSION;
		char * result = NULL;
		char * stackVersion = OLE2A(str);
		if( stackVersion )
		{	
			result = new char[_tcslen(stackVersion)+1];
			memcpy(result,stackVersion,_tcslen(stackVersion));
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
	char* ConvertBSTRToLPSTR (BSTR bstrIn)
	{
	  LPSTR pszOut = NULL;
	  if (bstrIn != NULL)
	  {
		int nInputStrLen = SysStringLen (bstrIn);

		// Double NULL Termination
		int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, bstrIn, nInputStrLen, NULL, 0, 0, 0) + 2; 
		pszOut = new char [nOutputStrLen];

		if (pszOut)
		{
		  memset (pszOut, 0x00, sizeof (char)*nOutputStrLen);
		  WideCharToMultiByte (CP_ACP, 0, bstrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
		}
	  }
	  return pszOut;
	}

	// ********************************************************
	//		Variant2BSTR
	// ********************************************************
	//	Converting variant to bstr; only several values are considered
	// should be replaced by CComVariant.CopyTo
	BSTR Variant2BSTR(VARIANT* val)
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
			str.Format("%g", val->dblVal);
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
	CString ReplaceNoCase( LPCTSTR instr, LPCTSTR oldstr, LPCTSTR newstr )
	{
		CString output( instr );

		// lowercase-versions to search in.
		CString input_lower( instr );
		CString oldone_lower( oldstr );
		input_lower.MakeLower();
		oldone_lower.MakeLower();

		// search in the lowercase versions,
		// replace in the original-case version.
		int pos=0;
		while ( (pos=input_lower.Find(oldone_lower,pos))!=-1 ) {

			// need for empty "newstr" cases.
			input_lower.Delete( pos, lstrlen(oldstr) );	
			input_lower.Insert( pos, newstr );

			// actually replace.
			output.Delete( pos, lstrlen(oldstr) );
			output.Insert( pos, newstr );
		}

		return output;
	}
#pragma endregion

#pragma region Shapefile
	// **************************************************************
	//		ShapeTypeIsM()
	// **************************************************************
	bool ShapeTypeIsM(ShpfileType shpType)
	{
		return shpType == SHP_POINTM || shpType == SHP_MULTIPOINTM || shpType == SHP_POLYLINEM || shpType == SHP_POLYGONM;
	}

	// **************************************************************
	//		ShapeTypeConvert2D()
	// **************************************************************
	ShpfileType ShapeTypeConvert2D(ShpfileType shpType)
	{
		if		(shpType == SHP_NULLSHAPE)																return SHP_NULLSHAPE;
		else if (shpType == SHP_POINT || shpType == SHP_POINTM || shpType == SHP_POINTZ)				return SHP_POINT;
		else if	(shpType == SHP_MULTIPOINT || shpType == SHP_MULTIPOINTM || shpType == SHP_MULTIPOINTZ)	return SHP_MULTIPOINT;
		else if (shpType == SHP_POLYGON || shpType == SHP_POLYGONM || shpType == SHP_POLYGONZ)			return SHP_POLYGON;
		else if (shpType == SHP_POLYLINE || shpType == SHP_POLYLINEM || shpType == SHP_POLYLINEZ)		return SHP_POLYLINE;
		else																							return shpType;
	}
	
	// **************************************************************
	//		Utility::swapEndian()
	// **************************************************************
	void Utility::swapEndian(char* a,int size) 
	{
		char hold;
		for(int i = 0; i < size*.5; i++)
		{	hold = a[i];
			a[i] = a[size-i-1];
			a[size-i-1] = hold;
		}
	}
#pragma endregion

#pragma region Files
	// *******************************************************************
	//		fileExists()
	// *******************************************************************
	BOOL Utility::fileExists(CString filename)
	{
		if( filename.GetLength() <= 0 )
			return FALSE;

		FILE * file = fopen(filename, "rb");
		
		if( file == NULL )
			return FALSE;
		else
		{	fclose(file);
			return TRUE;
		}
	}

	// *******************************************************************
	//		get_FileSize()
	// *******************************************************************
	long Utility::get_FileSize(CStringW filename)
	{
		if( filename.GetLength() <= 0 )
			return FALSE;
		
		FILE * file = _wfopen(filename, L"rb");
		
		long size = 0;
		if (file)
		{
			fseek( file, 0, SEEK_END );
			size = ftell( file );
			fclose(file);
		}

		return size;
	}

	// *******************************************************************
	//		dirExists()
	// *******************************************************************
	bool dirExists(CStringW path)
	{
		DWORD ftyp = GetFileAttributesW(path);
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return false;  //something is wrong with your path!
		return (ftyp & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
	}

	// *******************************************************************
	//		Utility::fileExistsW()
	// *******************************************************************
	bool fileExistsW(CStringW filename)
	{
		if( filename.GetLength() <= 0 )
			return FALSE;

		FILE * file = _wfopen(filename, L"rb");
		
		if( file == NULL )
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
	bool Utility::fileExistsUnicode(CStringW filename)
	{
		if( filename.GetLength() <= 0 )
			return FALSE;

		USES_CONVERSION;

		// Chris Michaelis 12/19/2005 - Windows 98 doesn't support unicode and will thus crash and burn on _wfopen.
		FILE * file = NULL;
		OSVERSIONINFO OSversion;
		OSversion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
		::GetVersionEx(&OSversion);
		if (OSversion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		{
			file = fopen(W2A(filename), "rb");
		}
		else
		{
			// Running 2k, XP, NT, or other future versions
			//Changed the following code to support Asian character file name 11/5/2005 --Lailin Chen.
			file = _wfopen(filename, L"rb");
		}

		if( file == NULL )
			return false;
		else
		{	
			fclose(file);
			return true;
		}
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
			unsigned int i,j;
			for ( i = 0; i < list1.size(); i++)
			{
				for ( j = 0; j < list2.size(); j++)
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

			// exluding folder part from the path
			path += Filename.Mid(list1[i].GetLength() + 1);
			return path;
		}
	}

	// *********************************************************
	//	     GetFolderFromPath()
	// *********************************************************
	CStringW Utility::GetFolderFromPath(CStringW path)
	{
		for (int i = path.GetLength() - 1; i > 0; i--)
		{
			if (path.Mid(i, 1) == '\\')
			{
				return path.Left(i);	// -1
			}
		}
		return path;
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
			fseek( file, 0, SEEK_END );
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
			fseek( file, 0, SEEK_END );
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
		if (get_FileCreationTime(filename, time1) && get_FileCreationTime(thanFilename, time2))
		{
			// subtract several seconds
			ULONGLONG qwResult;

			// Copy the time into a quadword.
			qwResult = (((ULONGLONG) time2.dwHighDateTime) << 32) + time2.dwLowDateTime;

			// Add 30 days.
			qwResult -= 10 * _SECOND;

			// Copy the result back into the FILETIME structure.
			time2.dwLowDateTime  = (DWORD) (qwResult & 0xFFFFFFFF );
			time2.dwHighDateTime = (DWORD) (qwResult >> 32 );
			
			int val = CompareFileTime(&time1, &time2);
			return val == 1 ;
		}
		return false;
	}

	// ********************************************************
	//     Utility::get_FileCreationTime()
	// ********************************************************
	bool get_FileCreationTime(CStringW filename, FILETIME& time)
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
		if (Utility::fileExistsW(filename))
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
	CStringW getProjectionFilename( CStringW dataSourceName )
	{
		return ChangeExtension(dataSourceName, L"prj");
	}

	// ********************************************************
	//    ChangeExtension()
	// ********************************************************
	CStringW ChangeExtension( CStringW filename, CStringW ext )
	{
		int theDot = filename.ReverseFind('.');
		if (theDot < 0)
			return filename + ext;
		return filename.Left(theDot + 1) + ext;
	}
#pragma endregion
	
#pragma region Unit conversion
	// ****************************************************************
	//		GetUnitOfMeasureText
	// ****************************************************************
	// Returns the short namó for units of measure
	CString Utility::GetUnitOfMeasureText(tkUnitsOfMeasure units)
	{
		switch(units)
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
	double Utility::getConversionFactor(tkUnitsOfMeasure Units)
	{
		switch (Units)
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
		value *= Utility::getConversionFactor(source);	// in inches
		double factor = Utility::getConversionFactor(target);
		if (factor != 0.0)
		{
			value /= factor;
			return true;
		}
		else
		{
			value = 0.0;
			return false;
		}
	}
#pragma endregion

#pragma region Numbers
	// *********************************************************
	//		FormatAngle()
	// *********************************************************	
	CStringW Utility::FormatAngle(double angle, bool withDecimals)
    {
        while (angle < -180.0)
            angle += 360.0;

        while (angle > 180.0)
            angle -= 360.0;

        int degrees = (int)floor(angle);
        double delta = angle - degrees;

        int minutes = (int)floor(60.0 * delta);
        
        delta = delta - minutes/60.0;
        double seconds = delta*3600.0;

        //CString dec = withDecimals ? ((int)((seconds % 1)*100.0 + 0.5)).ToString("D2") : "";
        CStringW s;
		s.Format(L"%d° %2d' %2d\"", degrees, minutes, (int)floor(seconds)); //, dec);
		return s;
    }

	// *********************************************************
	//		FormatNumber()
	// *********************************************************
	CString Utility::FormatNumber(double val, CString& sFormat)
	{
		CString s;
		if (val > 1000000000.0 && val < 1000000000000.0)
		{
			s.Format("%.2f", val/1000000000.0);
			if (s.GetLength() > 5)
				s.Delete(5, 1);
			s += "b";
		}
		else if (val > 1000000.0)
		{
			s.Format("%.2f", val/1000000.0);
			if (s.GetLength() > 5)
				s.Delete(5, 1);
			s += "m";
		}
		else if (val > 1000.0)
		{
			s.Format("%.2f", val/1000.0);
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
		if(value < 0.0)
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
		for(int i = 1; i <= n; ++i)
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

	double FloatRound(double doValue, int nPrecision)
	{
		static const double doBase = 10.0;
		double doComplete5, doComplete5i;
	    
		doComplete5 = doValue * pow(doBase, (double) (nPrecision + 1));
	    
		if(doValue < 0.0)
			doComplete5 -= 5.0;
		else
			doComplete5 += 5.0;
	    
		doComplete5 /= doBase;
		modf(doComplete5, &doComplete5i);
	    
		return doComplete5i / pow(doBase, (double) nPrecision);
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
		
	   
	   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	   Gdiplus::GetImageEncodersSize(&num, &size);
	   if(size == 0)
		  return -1;  // Failure

	   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	   if(pImageCodecInfo == NULL)
		  return -1;  // Failure

	   GetImageEncoders(num, size, pImageCodecInfo);

	   for(UINT j = 0; j < num; ++j)
	   {
		  if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
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

	// **************************************************
	//	SaveBitmap
	// **************************************************
	// Saves provided array of pixels as png image (uses GDI+)
	bool Utility::SaveBitmap(int width, int height, unsigned char* pixels, BSTR outputName)
	{
		int pad = (width * 24) % 32;
		if(pad != 0)
		{	pad = 32 - pad;
			pad /= 8;
		}

		BITMAPINFOHEADER bih;
		bih.biCompression=0;
		bih.biXPelsPerMeter=0;
		bih.biYPelsPerMeter=0;
		bih.biClrUsed=0;
		bih.biClrImportant=0;
		bih.biPlanes=1;
		bih.biSize=sizeof(BITMAPINFOHEADER);

		bih.biBitCount=24;
		bih.biWidth= width;
		bih.biHeight= height;
		bih.biSizeImage= (width * 3 + pad) * height;
		
		BITMAPINFO bif;
		bif.bmiHeader = bih;
		
		int nBytesInRow = width * 3 + pad;
		
		// copying bits
		unsigned char* bitsNew = new unsigned char[nBytesInRow * height];
		for(int i = 0; i < height; i++)	
			memcpy(&bitsNew[i * nBytesInRow], &pixels[i * width * 3], width * 3);
		
		// saing the image
		Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(&bif, (void*)bitsNew); 
		
		CLSID pngClsid;
		GetEncoderClsid(L"png", &pngClsid);	// perhaps some other formats ?
		USES_CONVERSION;
		Gdiplus::Status status = bmp->Save(OLE2W(outputName), &pngClsid, NULL);

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

	DWORD* Utility::cvtUCharToDword(long inp, int &num)
	{   /* Chris Michaelis and Michelle Hospodarsky 2-11-2004 */
		/* this function creates a DWORD[] from a long : used to create the custom pen for a custom stipple*/
		/* the first digit in inp is the multiplier for the remainder of the digits	*/

		std::vector<long> temp;
		int multiplier;
		char inpStr[33];
		_ltoa(inp,inpStr,10);

		int iter = 0;
		for (; inpStr[iter] != 0; iter++)
		{
		  temp.push_back(inpStr[iter]);
		}

		int size = temp.size();
		DWORD * output = new DWORD[size];

		//get the multiplier
		multiplier = atoi((char*)&temp[0]);

		for (int i = 1; i < size; i++)
		{
			output[i-1] =  multiplier * atoi((char*)&temp[i]); // multiply to enhance value since range = 0 - 9
		}

		/* get the number of elements*/
		num = temp.size() - 1; // take off one for the multiplier and one for the null value
		temp.clear(); // be frugal with memory..

		return output;
	}

	// ***********************************************************
	//		DimColor
	// ***********************************************************
	Gdiplus::Color Utility::ChangeBrightness(OLE_COLOR color, int shiftValue, long alpha)
	{
		short r = GetRValue(color) + shiftValue;	if (r< 0) r = 0;	if (r> 255) r = 255;
		short g = GetGValue(color) + shiftValue;	if (g< 0) g = 0;	if (g> 255) g = 255;
		short b = GetBValue(color) + shiftValue;	if (b< 0) b = 0;	if (b> 255) b = 255;

		Gdiplus::Color clr(alpha|BGR_TO_RGB(RGB(r,g,b)));
		return clr;
	}
#pragma endregion

#pragma region Gdal
	// ********************************************************
	//     CPLCreateXMLAttributeAndValue()
	// ********************************************************
	CPLXMLNode* Utility::CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, CStringW valueW)
	{
		CStringA valueA = Utility::ConvertToUtf8(valueW);	
		CPLXMLNode* psNode = CPLCreateXMLNode(psParent, CXT_Attribute, pszName);
		CPLCreateXMLNode( psNode, CXT_Text, valueA);
		return psNode;
	}
	
	CPLXMLNode* Utility::CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, const char *pszValue)
	{
		CPLXMLNode* psNode = CPLCreateXMLNode(psParent, CXT_Attribute, pszName);
		CPLCreateXMLNode( psNode, CXT_Text, pszValue);
		return psNode;
	}

	CPLXMLNode* Utility::CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, int nValue)
	{
		CString temp;
		temp.Format ("%d", nValue);
		return CPLCreateXMLAttributeAndValue(psParent, pszName, temp);
	}

	CPLXMLNode* Utility::CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, double rValue)
	{
		CString temp;
		temp.Format ("%f", rValue);
		return CPLCreateXMLAttributeAndValue(psParent, pszName, temp);
	}
#pragma endregion

#pragma region COM
	// ********************************************************
	//	  Utility::put_ComReference
	// ********************************************************
	// Sets new instance of COM object to the given pointer. Takes care to release the old reference
	bool Utility::put_ComReference(IDispatch* newVal, IDispatch** oldVal, bool allowNull)
	{
		if (*oldVal == NULL)
		{
			if (newVal)
			{
				(*oldVal) = newVal;
				newVal->AddRef();
			}
		}
		else if ((*oldVal) != newVal)
		{
			if ( newVal == NULL && !allowNull)
			{
				// NULL reference isn't allowed
				return false;	
			}
			else
			{
				if (*oldVal)
				{
					(*oldVal)->Release();
					(*oldVal) = NULL;
				}
				(*oldVal) = newVal;
			
				if (newVal)
				{
					newVal->AddRef();
				}
			}
		}
		else
		{
			// do nothing, the reference is in place
		}
		return true;
	}

	// ********************************************************
	//     GetInterfaceName()
	// ********************************************************
	CString Utility::GetInterfaceName(tkInterface id)
	{
		switch(id)
		{
		case idChart: 			return "Chart";
		case idChartField:		return "ChartField";
		case idCharts:			return "Charts";
		case idColorScheme:		return "ColorScheme";
		case idESRIGridManager:	return "ESRIGridManager";
		case idExtents:			return "Extents";
		case idField:			return "Field";
		case idFieldStatOperations:			return "FieldStatOperations";
		case idFileManager:		return "FileManager";
		case idGeoProjection:	return "GeoProjection";
		case idGlobalSettings:	return "GlobalSettings";
		case idGrid:			return "Grid:";
		case idGridColorBreak:	return "GridColorBreak";
		case idGridColorScheme:	return "GridColorScheme";
		case idGridHeader:		return "GridHeader";
		case idImage:			return "Image";
		case idLabelCategory:	return "LabelCategory";
		case idLabel:			return "Label";
		case idLabels:			return "Labels";
		case idLinePattern:		return "LinePattern";
		case idLineSegment:		return "LineSegment";
		case idMeasuring:		return "Measuring";
		case idPoint:			return "Point";
		case idShape:			return "Shape";
		case idShapeDrawingOptions:		return "ShapeDrawingOptions";
		case idShapefile:		return "Shapefile";
		case idShapefileCategories:		return "ShapefileCategories";
		case idShapefileCategory:		return "ShapefileCategory";
		case idShapefileColorBreak:		return "ShapefileColorBreak";
		case idShapefileColorScheme:	return "ShapefileColorScheme";
		case idShapeNetwork:			return "ShapeNetwork";
		case idShapeValidationInfo:		return "ShapeValidationInfo";
		case idTable:			return "Table";
		case idTileProviders:	return "TileProviders";
		case idTiles:			return "Tiles";
		case idTin:				return "Tin";
		case idUtils:			return "Utils";
		case idVector:			return "Vector";
		case idOgrDatasource:   return "OgrDatasource";
		case idOgrLayer:		return "OgrLayer";
		default:				return "Unknown";
		}
	}

#pragma endregion

#pragma region Progress
	// ********************************************************************
	//		DisplayProgress()
	// ********************************************************************
	void DisplayProgress(ICallback* callback, int index, int count, char* message, BSTR& key, long& lastPercent)
	{
		if( callback != NULL ) 
		{
			long newpercent = (long)(((double)(index + 1)/count)*100);
			if( newpercent > lastPercent )
			{	
				lastPercent = newpercent;
				callback->Progress(OLE2BSTR(key),newpercent,A2BSTR(message));
			}
		}
	}

	// ********************************************************************
	//		DisplayProgressCompleted()
	// ********************************************************************
	void DisplayProgressCompleted(ICallback* callback, BSTR& key)
	{
		if( callback != NULL )
		{
			callback->Progress(OLE2BSTR(key),100,A2BSTR("Completed"));
			callback->Progress(OLE2BSTR(key),0,A2BSTR(""));
		}
	}

	// ********************************************************************
	//		DisplayProgressCompleted()
	// ********************************************************************
	void DisplayProgressCompleted(ICallback* callback)
	{
		if( callback != NULL )
		{
			callback->Progress(A2BSTR(""),100,A2BSTR("Completed"));
			callback->Progress(A2BSTR(""),0,A2BSTR(""));
		}
	}

	// ********************************************************************
	//		DisplayErrorMsg()
	// ********************************************************************
	void DisplayErrorMsg(ICallback* callback, BSTR& key, char* message, ...)
	{
		if( callback )
		{
			TCHAR buffer[1024];
 			va_list args;
			va_start( args, message);
			vsprintf( buffer, message, args );
			CString s = buffer;
			Debug::WriteLine(s);
			callback->Error(OLE2BSTR(key), A2BSTR(s));
		}
	}
	void DisplayErrorMsg(ICallback* callback, CString key, char* message, ...)
	{
		if( callback )
		{
			TCHAR buffer[1024];
 			va_list args;
			va_start( args, message);
			vsprintf( buffer, message, args );
			CString s = buffer;
			Debug::WriteLine(s);
			callback->Error(A2BSTR(key), A2BSTR(s));
		}
	}

#pragma endregion

	// ********************************************************
	//	  getCurrentYear
	// ********************************************************
	int getCurrentYear()
	{
		const time_t curTime = time(NULL);
		struct tm *tmData = localtime(&curTime);
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

		if (!Utility::dirExists(path))
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
		Utility::CPLCreateXMLAttributeAndValue( psTree, "OcxVersion", GetFileVersionString());
		Utility::CPLCreateXMLAttributeAndValue( psTree, "FileType", fileType);
		Utility::CPLCreateXMLAttributeAndValue( psTree, "FileVersion", CPLString().Printf("%d", m_globalSettings.xmlFileVersion));
		Utility::CPLCreateXMLAttributeAndValue( psTree, "FilenamesEncoding", CPLString().Printf(m_globalSettings.xmlFilenameEncoding));
	}

	// ****************************************************************** 
	//		GetFileVersionString
	// ****************************************************************** 
	CString GetFileVersionString()
	{
		wchar_t* path = new wchar_t[MAX_PATH + 1];
		GetModuleFileNameW(GetModuleInstance(), path, MAX_PATH);

		DWORD  verHandle = NULL;
		UINT   size      = 0;
		LPBYTE lpBuffer  = NULL;
		DWORD  verSize   = GetFileVersionInfoSizeW( path, &verHandle);
		CString result;

		if (verSize != NULL)
		{
			LPSTR verData = new char[verSize];

			if (GetFileVersionInfoW( path, verHandle, verSize, verData))
			{
				if (VerQueryValue(verData,"\\",(VOID FAR* FAR*)&lpBuffer,&size))
				{
					if (size)
					{
						VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
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
		return result;
	}

	// ****************************************************************** 
	//		ClearShapefileModifiedFlag
	// ****************************************************************** 
	void ClearShapefileModifiedFlag(IShapefile* sf)
	{
		if (!sf) return;
		long numShapes = 0;
		sf->get_NumShapes(&numShapes);
		for (int i = 0; i < numShapes; i++)
		{
			sf->put_ShapeModified(i, VARIANT_FALSE);
		}
	}

}

namespace Debug
{
	// ****************************************************************** 
	//		WriteWithTime
	// ****************************************************************** 
	void WriteWithTime(CString format, ...)
	{
		TCHAR buffer[1024];
		va_list args;
		va_start( args, format);
		vsprintf( buffer, format, args );
		
		SYSTEMTIME time;
		GetLocalTime(&time);
		CString s2;
		s2.Format("%02d:%02d:%02d.%-3d: ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		CString s = buffer;
		format = s2 + s + "\n";
		OutputDebugStringA(format);
	}
	
	// ****************************************************************** 
	//		WriteLine
	// ****************************************************************** 
	void WriteLine(CString format, ...)
	{
		TCHAR buffer[1024];
 		va_list args;
		va_start( args, format);
		vsprintf( buffer, format, args );
		CString s = buffer;
		format = "OCX: " + s + "\n";
		OutputDebugStringA(format);
	}

	// ****************************************************************** 
	//		WriteError
	// ****************************************************************** 
	void WriteError(CString format, ...)
	{
		TCHAR buffer[1024];
 		va_list args;
		va_start( args, format);
		vsprintf( buffer, format, args );
		CString s = buffer;
		format = "OCX ERROR: " + s + "\n";
		OutputDebugStringA(format);
	}
}