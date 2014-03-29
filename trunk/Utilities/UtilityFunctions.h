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
	// string conversion
	BSTR Variant2BSTR(VARIANT* val);
	char * SYS2A(BSTR str);
	WCHAR* StringToWideChar(CString s);
	char* ConvertBSTRToLPSTR (BSTR bstrIn);
	
	CStringA ConvertToUtf8(CStringW unicode); 
	CStringW ConvertFromUtf8(CStringA utf8);
	
	// numbers
	CString FormatNumber(double val, CString& sFormat);
	int Rint(double value);
	double atof_custom(CString s);
	double FloatRound(double doValue, int nPrecision);

	// files, !!! all parameters should be CStringW; we use Unicode !!!
	bool fileExistsW(CStringW filename);
	bool fileExistsUnicode(CStringW filename);		// obsolete; with check for Win98
	bool get_FileCreationTime(CStringW filename, FILETIME& time);
	bool IsFileYounger(CStringW filename1, CStringW thanFilename);
	bool RemoveFile(CStringW filename);
	bool dirExists(CStringW path);
	bool EndsWith(CStringW path, CStringW ext);
	CStringW GetRelativePath(CStringW ProjectName, CStringW Filename);
	CStringW GetPathWOExtension(CStringW path);
	int ReadFileToBuffer(CStringW filename, char** buffer);
	int ReadFileToBuffer(CStringW filename, unsigned char** buffer);		// TODO: use template
	long get_FileSize(CStringW filename);
	CStringW getProjectionFilename( CStringW dataSourceName );
	CStringW GetFolderFromPath(CStringW path);
	CStringW ChangeExtension( CStringW filename, CStringW ext);
	// these are still ANSI, there is some cases of LPCTString in API which can't be changed
	BOOL fileExists(CString filename);	

	// GDAL XML
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, CStringW valueW);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, const char *pszValue);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, int nValue);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, double rValue);

	// GDI
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	Gdiplus::Font* GetGdiPlusFont(CString name, float size);
	bool SaveBitmap(int width, int height, unsigned char* pixels, BSTR outputName);
	DWORD* cvtUCharToDword(long inp, int &num);

	// shapefile
	void swapEndian(char* a,int size);
	bool ShapeTypeIsM(ShpfileType shpType);
	ShpfileType ShapeTypeConvert2D(ShpfileType shpType);

	// units conversions
	double getConversionFactor(tkUnitsOfMeasure Units);
	bool ConvertDistance(tkUnitsOfMeasure source, tkUnitsOfMeasure target, double& value);
	CString GetUnitOfMeasureText(tkUnitsOfMeasure units);

	// display progress
	void DisplayProgress(ICallback* callback, int index, int count, char* message, BSTR& key, long& lastPercent);
	void DisplayProgressCompleted(ICallback* callback, BSTR& key);

	// Sets new instance of COM object to the given pointer
	bool put_ComReference(IDispatch* newVal, IDispatch** oldVal, bool allowNull = true);
	
	// time
	int getCurrentYear();
}

namespace Debug
{
	void WriteLine(CString format, ...);
	//void WriteLine(CString message);
}
