#include "stdafx.h"
#include <iterator>
#include <fstream>
#include <gdiplus.h>
#include <vector>

namespace Utility
{
	static _locale_t m_locale = _create_locale(LC_ALL, "C");
	
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
	
	bool ShapeTypeIsM(ShpfileType shpType)
	{
		return shpType == SHP_POINTM || shpType == SHP_MULTIPOINTM || shpType == SHP_POLYLINEM || shpType == SHP_POLYGONM;
	}

	ShpfileType ShapeTypeConvert2D(ShpfileType shpType)
	{
		if		(shpType == SHP_NULLSHAPE)																return SHP_NULLSHAPE;
		else if (shpType == SHP_POINT || shpType == SHP_POINTM || shpType == SHP_POINTZ)				return SHP_POINT;
		else if	(shpType == SHP_MULTIPOINT || shpType == SHP_MULTIPOINTM || shpType == SHP_MULTIPOINTZ)	return SHP_MULTIPOINT;
		else if (shpType == SHP_POLYGON || shpType == SHP_POLYGONM || shpType == SHP_POLYGONZ)			return SHP_POLYGON;
		else if (shpType == SHP_POLYLINE || shpType == SHP_POLYLINEM || shpType == SHP_POLYLINEZ)		return SHP_POLYLINE;
		else																							return shpType;
	}

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

	long Utility::get_FileSize(CString filename)
	{
		if( filename.GetLength() <= 0 )
			return FALSE;

		FILE * file = fopen(filename, "rb");
		
		long size = 0;
		if (file)
		{
			fseek( file, 0, SEEK_END );
			size = ftell( file );
			fclose(file);
		}

		return size;
	}

	BOOL fileExistsw(CStringW filename)
	{
		if( filename.GetLength() <= 0 )
			return FALSE;

		FILE * file = _wfopen(filename, L"rb");
		
		if( file == NULL )
		{
			return FALSE;
		}
		else
		{	
			fclose(file);
			return TRUE;
		}
	}

	// *******************************************************************
	//		Utility::dirExists()
	// *******************************************************************
	bool Utility::dirExists(CString path)
	{
		DWORD ftyp = GetFileAttributesA(path);
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return false;  //something is wrong with your path!
		return (ftyp & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
	}

	// *******************************************************************
	//		Utility::fileExistsUnicode()
	// *******************************************************************
	BOOL Utility::fileExistsUnicode(CString filename)
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
			file = fopen(filename, "rb");
		}
		else
		{
			// Running 2k, XP, NT, or other future versions
			//Changed the following code to support Asian character file name 11/5/2005 --Lailin Chen.
			file = _wfopen(T2W(filename), L"rb");
		}

		if( file == NULL )
			return FALSE;
		else
		{	
			fclose(file);
			return TRUE;
		}
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

	// **********************************************************
	//			get_ConversionFactor()
	// **********************************************************
	/// Returns the conversion factor between the map units and inches
	double Utility::getConversionFactor(tkUnitsOfMeasure Units)
	{
		switch (Units)
		{
			case umDecimalDegrees:	return 4366141.73;
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

	// Shade1974 Jan 10, 2006
	// Explicit casting to int using rounding
	int Utility::Rint(double value)
	{
		if(value < 0.0)
			value -= 0.5;
		else
			value += 0.5;
		int val = static_cast<int>(value);
		return val;
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



	// *********************************************************
	//		Format()
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

	// returns list paths for all parent folders for current file
	// from inner most to top most
	void GetFoldersList(CString path, std::vector<CString>& list)
	{
		for (int i = path.GetLength() - 1; i >= 0; i--)
		{
			CString mid = path.Mid(i, 1);
			if (mid == '\\' && i - 1 > 0)
			{
				list.push_back(path.Left(i));
			}
		}
	}

	// *********************************************************
	//	     get_RelativePath()
	// *********************************************************
	CString Utility::GetRelativePath(CString ProjectName, CString Filename)
	{
		CString drive1, drive2;
		char drive[1024];
		char dir[1024];
		char name[1024];
		char ext[1024];

		_splitpath(ProjectName, drive, dir, name, ext);
		drive1 = drive;

		_splitpath(Filename, drive, dir, name, ext);
		drive2 = drive;

		if (drive1 != drive2)
		{
			// files are on the different drives, no way to get a relative path
			return Filename;
		}
		else
		{
			std::vector<CString> list1;
			std::vector<CString> list2;
			
			GetFoldersList(ProjectName, list1);
			GetFoldersList(Filename, list2);

			// searching for the match (inner-most folder common to both files)
			unsigned int i,j;
			for ( i = 0; i < list1.size(); i++)
			{
				for ( j = 0; j < list2.size(); j++)
				{
					if (_stricmp(list1[i], list2[j]) == 0)
					{
						goto match;
					}
				}
			}

			match:
			CString path = "";
			for (unsigned int k = 0; k < i; k++)
			{
				// going to the parent folder 
				path += "..\\";
			}

			// exluding folder part from the path
			path += Filename.Mid(list1[i].GetLength() + 1);
			return path;
		}
	}

	// *********************************************************
	//	     GetPathWOExtension()
	// *********************************************************
	CString Utility::GetFolderFromPath(CString path)
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
	//	     GetFileExtension()
	// *********************************************************
	bool EndsWith(CString path, CString ext)
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
	CString GetPathWOExtension(CString path)
	{
		for (int i = path.GetLength() - 1; i > 0; i--)
		{
			if (path.Mid(i, 1) == ".")
			{
				return path.Left(i);	// -1
			}
		}
		return path;
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
		return _atof_l(s, m_locale);
	}
	
	// ****************************************************
	//   ReadFileToBuffer
	// ****************************************************
	// Reads the content of the file to buffer, return the number of bytes read
	int ReadFileToBuffer(CString filename, unsigned char** buffer)
	{
		USES_CONVERSION;
		
		FILE* file = fopen(filename,"rb");
		
		long size = 0;
		if (file)
		{
			fseek( file, 0, SEEK_END );
			size = ftell(file);
			if (size > 0)
			{
				*buffer = new unsigned char[size];
				rewind(file);
				size = fread(*buffer, sizeof(char), size, file);
			}
			fclose(file);
		}
		return size;

		//ifstream
		/*std::basic_ifstream<unsigned char, std::char_traits<unsigned char>> file(filename, ios::in|ios::binary|ios::ate);
		if (file.is_open())
		{
			int size = file.tellg();
			*buffer = new unsigned char[size];
			file.seekg (0, ios::beg);
			file.read(*buffer, size);
			file.close();
			return size;
		}*/
		//return 0;
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

	WCHAR* StringToWideChar(CString s)
	{
		WCHAR* wText = NULL;
		int size = MultiByteToWideChar(CP_ACP, 0, s.GetString(), -1, NULL, 0);
		wText = new WCHAR[size];
		MultiByteToWideChar(CP_ACP, 0, s.GetString(), -1, wText, size);
		return wText;
	}

	Gdiplus::Font* GetGdiPlusFont(CString name, float size)
	{
		WCHAR* wFontName = StringToWideChar("Arial");
		Gdiplus::FontFamily family(wFontName);
		Gdiplus::Font* font = new Gdiplus::Font(&family, (Gdiplus::REAL)size);
		delete wFontName;
		return font;
	}

	int getCurrentYear()
	{
		const time_t curTime = time(NULL);
		struct tm *tmData = localtime(&curTime);
		return tmData->tm_year + 1900;
	}

	// --------------------------------------------------
	//	SaveBitmap
	// --------------------------------------------------
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
	//     Utility::CPLCreateXMLAttributeAndValue()
	// ********************************************************
	CPLXMLNode* Utility::CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, const char *pszValue)
	{
		CPLXMLNode* psNode = CPLCreateXMLNode(psParent, CXT_Attribute, pszName);
		CPLCreateXMLNode( psNode, CXT_Text, pszValue);
		return psNode;
	}

	 #define _SECOND ((__int64) 10000000)

	// ********************************************************
	//     Utility::CompareCreationTime()
	// ********************************************************
	// returns: 1 = first file younger, -1 = vice versa, 0 = equal age; any other value = error
	bool IsFileYounger(CString filename, CString thanFilename)
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
	bool get_FileCreationTime(CString filename, FILETIME& time)
	{
		_WIN32_FILE_ATTRIBUTE_DATA data;
		if (GetFileAttributesEx(filename, GetFileExInfoStandard, &data))
		{
			time = data.ftCreationTime;
			return true;
		}
		return false;
	}

	// ********************************************************
	//    RemoveFile()
	// ********************************************************
	bool RemoveFile(CString filename)
	{
		if (Utility::fileExists(filename))
		{
			return remove(filename) == 0;
		}
		else {
			return true;	// no file and therefore no problem
		}
	}
}

namespace Debug
{
	//#include <time.h>
	/*time_t now;
		time(&now);
		CString temp;
		temp = ctime(&now);*/
	void WriteLine(CString format, ...)
	{
		//#ifdef _DEBUG
		TCHAR buffer[1024];
 		va_list args;
		va_start( args, format);
		vsprintf( buffer, format, args );
		CString s = buffer;
		format = "OCX: " + s + "\n";
		OutputDebugStringA(format);
		//#endif
		//return _CrtDbgReport(_CRT_WARN,NULL,NULL,NULL,buffer);
	}

	void Write(CString message)
	{
		OutputDebugStringA(message);
	}
}