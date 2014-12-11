#pragma once
namespace CallbackHelper
{
	void Progress(ICallback* callback, int index, double count, const char* message, BSTR& key, long& lastPercent);
	void Progress(ICallback* callback, int index, int count, const char* message, BSTR& key, long& lastPercent);
	void Progress(ICallback* callback, int index, int count, const char* message, long& lastPercent);
	void Progress(ICallback* callback, int percent, const char* message, BSTR& key);
	void Progress(ICallback* callback, int percent, const char* message);
	void ProgressCompleted(ICallback* callback, BSTR& key);
	void ProgressCompleted(ICallback* callback);
	void ErrorMsg(CString className, ICallback* localCback, BSTR& key, const char* message, ...);
	void ErrorMsg(CString className, ICallback* localCback, CString key, const char* message, ...);
	void ErrorMsg(CString message);
	void FillGdalCallbackParams(CallbackParams& params, ICallback* localCallback, const char* message);
};

int CPL_STDCALL GDALProgressCallback(double dfComplete, const char* pszMessage, void *pData);

