#include "stdafx.h"
#include "CallbackHelper.h"

// *****************************************************************
//	   GDALProgressCallback()
// *****************************************************************
int CPL_STDCALL GDALProgressCallback(double dfComplete, const char* pszMessage, void *pData)
{
	CallbackParams* params = (CallbackParams*)pData;

	if (params != NULL && params->cBack != NULL)
	{
		long percent = long(dfComplete * 100.0);
		CallbackHelper::Progress(params->cBack, percent, params->sMsg);
	}
	return TRUE;
}

// ********************************************************************
//		DisplayProgress()
// ********************************************************************
void CallbackHelper::Progress(ICallback* localCback, int index, int count, const char* message, long& lastPercent)
{
	Progress(localCback, index, (double)count, message, m_globalSettings.emptyBstr, lastPercent);
}

void CallbackHelper::Progress(ICallback* localCback, int index, int count, const char* message, BSTR& key, long& lastPercent)
{
	Progress(localCback, index, (double)count, message, key, lastPercent);
}

// ********************************************************************
//		DisplayProgress()
// ********************************************************************
void CallbackHelper::Progress(ICallback* localCback, int index, double count, const char* message, BSTR& key, long& lastPercent)
{
	ICallback* callback = m_globalSettings.callback ? m_globalSettings.callback : localCback;
	if (!callback) return;

	long newpercent = (long)(((double)(index + 1) / count) * 100);
	if (newpercent > lastPercent)
	{
		lastPercent = newpercent;
		CComBSTR bstrMsg(message);
		callback->Progress(key, newpercent, bstrMsg);
	}
}

// ********************************************************************
//		DisplayProgress()
// ********************************************************************
void CallbackHelper::Progress(ICallback* localCback, int percent, const char* message, BSTR& key)
{
	ICallback* callback = m_globalSettings.callback ? m_globalSettings.callback : localCback;
	if (!callback) return;

	CComBSTR bstrMsg(message);
	callback->Progress(key, percent, bstrMsg);
}

// ********************************************************************
//		DisplayProgress()
// ********************************************************************
void CallbackHelper::Progress(ICallback* localCback, int percent, const char* message)
{
	ICallback* callback = m_globalSettings.callback ? m_globalSettings.callback : localCback;
	if (!callback) return;

	if (!message) message = "";
	CComBSTR bstrMsg(message);
	callback->Progress(m_globalSettings.emptyBstr, percent, bstrMsg);
}

// ********************************************************************
//		DisplayProgressCompleted()
// ********************************************************************
void CallbackHelper::ProgressCompleted(ICallback* localCback, BSTR& key)
{
	ICallback* callback = m_globalSettings.callback ? m_globalSettings.callback : localCback;
	if (!callback) return;

	CComBSTR bstrMsg("Completed");
	callback->Progress(key, 100, bstrMsg);
	callback->Progress(key, 0, m_globalSettings.emptyBstr);
}

// ********************************************************************
//		DisplayProgressCompleted()
// ********************************************************************
void CallbackHelper::ProgressCompleted(ICallback* localCback)
{
	ProgressCompleted(localCback, m_globalSettings.emptyBstr);
}

// ********************************************************************
//		DisplayErrorMsg()
// ********************************************************************
void CallbackHelper::ErrorMsg(CString className, ICallback* localCback, BSTR& key, const char* message, ...)
{
	ICallback* callback = m_globalSettings.callback ? m_globalSettings.callback : localCback;

	if (callback || Debug::IsDebugMode())
	{
		if (strcmp(message, "No Error") == 0) return;

		TCHAR buffer[1024];
		va_list args;
		va_start(args, message);
		vsprintf(buffer, message, args);
		CString s = buffer;

		s = className + ": " + s;
		CComBSTR bstr(s);

		if (callback) {
			callback->Error(key, bstr);
		}
		else {
			Debug::WriteError(s);
		}
	}
}
void CallbackHelper::ErrorMsg(CString className, ICallback* localCallback, CString key, const char* message, ...)
{
	if (localCallback || Debug::IsDebugMode())
	{
		CComBSTR bstrKey(key);
		ErrorMsg(className, localCallback, bstrKey.m_str, message);
	}
}

void CallbackHelper::ErrorMsg(const char* message)
{
	CString s = message;
	ErrorMsg(s);
}

void CallbackHelper::ErrorMsg(CString message)
{
	if (m_globalSettings.callback)
	{
		CComBSTR bstr(message);
		m_globalSettings.callback->Error(m_globalSettings.emptyBstr, bstr);
	}
	else {
		if (Debug::IsDebugMode())
			Debug::WriteError(message);
	}
}

// ****************************************************************
//		AssertionFailed
// ****************************************************************
void CallbackHelper::AssertionFailed(CString message)
{
	message = "Assertion failed: " + message;
	if (m_globalSettings.callback)
	{
		CComBSTR bstr(message);
		m_globalSettings.callback->Error(m_globalSettings.emptyBstr, bstr);
	}
	else {
		if (Debug::IsDebugMode())
			Debug::WriteError(message);
	}
}

// ****************************************************************
//		FillGdalCallbackParams
// ****************************************************************
void CallbackHelper::FillGdalCallbackParams(CallbackParams& params, ICallback* localCallback, const char* message)
{
	ICallback* callback = m_globalSettings.callback ? m_globalSettings.callback : localCallback;
	params.cBack = callback;
	params.sMsg = message;
}