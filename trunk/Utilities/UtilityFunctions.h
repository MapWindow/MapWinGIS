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
	BSTR Variant2BSTR(VARIANT* val);
	char * SYS2A(BSTR str);
	WCHAR* StringToWideChar(CString s);
	char* ConvertBSTRToLPSTR (BSTR bstrIn);
	CString ReplaceNoCase( LPCTSTR instr, LPCTSTR oldstr, LPCTSTR newstr );
	
	CStringW XmlFilenameToUnicode(CStringA s, bool utf8);
	CStringA ConvertToUtf8(CStringW unicode); 
	CStringW ConvertFromUtf8(CStringA utf8);
	
	// numbers
	CString FormatNumber(double val, CString& sFormat);
	int Rint(double value);
	int Factorial(int n);
	double atof_custom(CString s);
	double FloatRound(double doValue, int nPrecision);
	CStringW FormatAngle(double angle, bool withDecimals = false);
	bool FloatsEqual(const float &a, const float &b);

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
	
	// these are still ANSI, there is some cases of LPCTString in API which can't be changed
	BOOL FileExists(CString filename);	

	// GDAL XML
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, CStringW valueW);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, const char *pszValue);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, int nValue);
	CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent, const char *pszName, double rValue);
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
	byte GetBrightness(OLE_COLOR color);
	void InitGdiPlusFont(Gdiplus::Font** font, CStringW name, float size);

	// shapefile
	void SwapEndian(char* a,int size);
	bool ShapeTypeIsM(ShpfileType shpType);
	ShpfileType ShapeTypeConvert2D(ShpfileType shpType);
	void ClearShapefileModifiedFlag(IShapefile* sf);
	tkLabelPositioning LabelPositionForShapeType(ShpfileType shpType);

	// units conversions
	double GetConversionFactor(tkUnitsOfMeasure Units);
	bool ConvertDistance(tkUnitsOfMeasure source, tkUnitsOfMeasure target, double& value);
	CString GetUnitOfMeasureText(tkUnitsOfMeasure units);

	void ClosePointer(Gdiplus::Bitmap** bitmap);

	// time
	int GetCurrentYear();
}