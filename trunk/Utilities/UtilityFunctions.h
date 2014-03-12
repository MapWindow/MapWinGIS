#pragma once
#include "MapWinGIS.h"
#include "Enumerations.h"
#include <gdiplus.h>
#include "cpl_minixml.h"
#include "cpl_string.h"
using namespace std;

// Simple functions, which can be useful in several classes
// Some of them are written in a hurry and perhaps can be improved or substituted by standart solutions
namespace Utility
{
	bool ShapeTypeIsM(ShpfileType shpType);
	ShpfileType ShapeTypeConvert2D(ShpfileType shpType);
	BSTR Variant2BSTR(VARIANT* val);
	BOOL fileExists(CString filename);
	long get_FileSize(CString filename);
	BOOL fileExistsw(CStringW filename);
	char * SYS2A(BSTR str);
	BOOL fileExistsUnicode(CString filename);
	void swapEndian(char* a,int size);
	double getConversionFactor(tkUnitsOfMeasure Units);
	bool ConvertDistance(tkUnitsOfMeasure source, tkUnitsOfMeasure target, double& value);
	int Rint(double value);
	char* ConvertBSTRToLPSTR (BSTR bstrIn);
	DWORD* cvtUCharToDword(long inp, int &num);
	bool dirExists(CString path);

	CString FormatNumber(double val, CString& sFormat);
	CString GetRelativePath(CString ProjectName, CString Filename);
	CString GetPathWOExtension(CString path);
	bool EndsWith(CString path, CString ext);
	CString GetFolderFromPath(CString path);
	double atof_custom(CString s);

	int ReadFileToBuffer(CString filename, unsigned char** buffer);
	double FloatRound(double doValue, int nPrecision);

	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

	WCHAR* StringToWideChar(CString s);
	
	Gdiplus::Font* GetGdiPlusFont(CString name, float size);
	int getCurrentYear();
	bool SaveBitmap(int width, int height, unsigned char* pixels, BSTR outputName);

	// Sets new instance of COM object to the given pointer
	bool put_ComReference(IDispatch* newVal, IDispatch** oldVal, bool allowNull = true);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, const char *pszValue);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, int nValue);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, double rValue);
	bool get_FileCreationTime(CString filename, FILETIME& time);
	bool IsFileYounger(CString filename1, CString thanFilename);
	bool RemoveFile(CString filename);

	void OpenLog(ofstream& logger, CStringW path, CStringW name);
}

namespace Debug
{
	void WriteLine(CString format, ...);
	//void WriteLine(CString message);
}
