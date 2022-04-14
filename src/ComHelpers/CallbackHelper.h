#pragma once
// ReSharper disable once CppInconsistentNaming
namespace CallbackHelper
{
	void Progress(ICallback* callback, int index, double count, const char* message, BSTR& key, long& lastPercent);
	void Progress(ICallback* callback, int index, int count, const char* message, BSTR& key, long& lastPercent);
	void Progress(ICallback* callback, int index, int count, const char* message, long& lastPercent);
	void Progress(ICallback* callback, int percent, const char* message, BSTR& key);
	void Progress(ICallback* callback, int percent, const char* message);
	void ProgressCompleted(ICallback* callback, BSTR& key);
	void ProgressCompleted(ICallback* callback = nullptr);
	void ErrorMsg(CString className, ICallback* localCback, BSTR& key, const char* message, ...);
	void ErrorMsg(CString className, ICallback* localCback, CString key, const char* message, ...);
	void ErrorMsg(CString message);
	void ErrorMsg(const char* message);
	void AssertionFailed(CString message);
	ICallback* GetCurrent(ICallback* localCallback);
};

int CPL_STDCALL GDALProgressCallback(double dfComplete, const char* pszMessage, void *pData);

