#pragma once

#include "MapWinGIS.h"
#include "Enumerations.h"
#include <set>
#include <vector>
using namespace std;

// Simple functions, which can be useful in several classes
// Some of them are written in a hurry and perhaps can be improved or substituted by standart solutions
namespace Utility
{
	//tkSimpleShapeType Set2SafeArray(ShpfileType shpType);
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

	bool Set2SafeArray(set<long>* selectResult, VARIANT* arr);
	template <typename T>  
	bool Vector2SafeArray(std::vector<T>* v, VARTYPE variantType, VARIANT* arr);
	bool Vector2SafeArray(std::vector<OLE_COLOR>* v, VARIANT* arr);
	bool Vector2SafeArray(vector<IShape*>* shapes, VARIANT* arr);
	CString FormatNumber(double val, CString& sFormat);
	CString GetRelativePath(CString ProjectName, CString Filename);
	CString GetPathWOExtension(CString path);
	CString GetFolderFromPath(CString path);
	double atof_custom(CString s);

	int ReadFileToBuffer(CString filename, unsigned char** buffer);
	double FloatRound(double doValue, int nPrecision);
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	bool SaveBitmap(int width, int height, unsigned char* pixels, BSTR outputName);

	
}

namespace Debug
{
	void WriteLine(CString format, ...);
}
