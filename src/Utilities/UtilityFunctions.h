#pragma once
#include "Enumerations.h"
#include <gdiplus.h>
#include "cpl_minixml.h"
#include "cpl_string.h"
using namespace std;

// Simple functions, which can be useful in several classes
// Some of them are written in a hurry and perhaps can be improved or substituted by standard solutions
namespace Utility
{
	// string conversion
	BSTR Variant2BSTR(VARIANT* val, CString floatFormat);
	char * SYS2A(BSTR str);
	WCHAR* StringToWideChar(CString s);
	char* ConvertBSTRToLPSTR (BSTR bstrIn, UINT codePage = CP_ACP);
	CString ReplaceNoCase( LPCTSTR instr, LPCTSTR oldstr, LPCTSTR newstr );
	
	CString UrlEncode(CString s);
	CStringW XmlFilenameToUnicode(CStringA s, bool utf8);
	CStringA ConvertToUtf8(CStringW unicode); 
	CStringW ConvertFromUtf8(CStringA utf8);
	CString GetSocketErrorMessage(DWORD socketError);

	// numbers
	CStringW GetUnitsFormat(int precision);
	CString FormatNumber(double val, CString& sFormat);
	int Rint(double value);
	int Factorial(int n);
	double atof_custom(CString s);
	double wtof_custom(CStringW s);
	double FloatRound(double doValue, int nPrecision);
	bool FloatsEqual(const float &a, const float &b);
	CStringW FormatArea(double area, bool unknownUnits, tkAreaDisplayMode units, int precision);

	// files, !!! all parameters should be CStringW; we use Unicode !!!
	bool FileExistsW(CStringW filename);
	bool FileExistsUnicode(CStringW filename);		// obsolete; with check for Win98
	bool GetFileCreationTime(CStringW filename, FILETIME& time);
	bool IsFileYounger(CStringW filename1, CStringW thanFilename);
	bool RemoveFile(CStringW filename);
	bool DirExists(CStringW path);
	bool EndsWith(CStringW path, CStringW ext);
	CStringW GetRelativePath(CStringW ProjectName, CStringW Filename);
	CStringW GetPathWOExtension(CStringW path);
	int ReadFileToBuffer(CStringW filename, char** buffer);
	int ReadFileToBuffer(CStringW filename, unsigned char** buffer);
	long GetFileSize(CStringW filename);
	CStringW GetProjectionFilename( CStringW dataSourceName );
	CStringW GetFolderFromPath(CStringW path);
	CStringW GetNameFromPath(CStringW path);
	CStringW ChangeExtension( CStringW filename, CStringW ext);
	CStringW GetNameFromPathWoExtension(CStringW path);
	CStringW GetMapWinGISPath();
	
	// these are still ANSI, there is some cases of LPCTString in API which can't be changed
	BOOL FileExists(CString filename);	
	CString GetTempFilename(CString extensionWithLeadingPoint);	 // only ASCII chars are used in this name; no need for CStringW

	// GDAL XML
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, CStringW valueW);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, const char *pszValue);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, int nValue);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, double rValue);
	int CPLXMLChildrentCount(CPLXMLNode *psParent);

	void SerializeVariant(CPLXMLNode* node, CString elementName, VARIANT* val);
	void DeserializeVariant(CString sValue, FieldType fieldType, VARIANT* var);
	void WriteXmlHeaderAttributes(CPLXMLNode* psTree, CString fileType);
	CString GetFileVersionString();
	bool SerializeAndDestroyXmlTree(CPLXMLNode* psTree, BSTR* bstr);

	// GDI
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	Gdiplus::Font* GetGdiPlusFont(CString name, float size);
	bool SaveBitmap(int width, int height, unsigned char* pixels, BSTR outputName);
	DWORD* cvtUCharToDword(long inp, int &num);
	Gdiplus::Color ChangeBrightness(OLE_COLOR color, int shiftValue, long alpha);
	OLE_COLOR ChangeBrightness(OLE_COLOR color, int shiftValue);
	Gdiplus::Color OleColor2GdiPlus(OLE_COLOR color, BYTE alpha);
	Gdiplus::Color OleColor2GdiPlus(OLE_COLOR color);
	byte GetBrightness(OLE_COLOR color);
	void InitGdiPlusFont(Gdiplus::Font** font, CStringW name, float size);
	
	// units conversions
	CStringW GetLocalizedUnitsText(tkUnitsOfMeasure units);
	double GetConversionFactor(tkUnitsOfMeasure units);
	bool ConvertDistance(tkUnitsOfMeasure source, tkUnitsOfMeasure target, double& value);
	CString GetUnitOfMeasureText(tkUnitsOfMeasure units);

	void ClosePointer(Gdiplus::Bitmap** bitmap);
	void ClosePointer(Gdiplus::Font** font);

	// time
	int GetCurrentYear();

	size_t split(const std::string &txt, std::vector<std::string> &strs, const char splittingChar);
}