#include "StdAfx.h"
#include "DebugHelper.h"

namespace Debug
{
	static const int TARGETS_COUNT = 5;
	bool targets[TARGETS_COUNT];

	// ****************************************************************** 
	//		CustomCPLErrorHandler
	// ****************************************************************** 
	void CPL_STDCALL CustomCPLErrorHandler(const CPLErr error, int errorCode, const char* message)
	{
		if (m_globalSettings.suppressGdalErrors && !targets[DebugForceGdal]) return;

		CString s = message;
		switch (error)
		{
			case CE_Debug:
				s = "GDAL DEBUG: " + s;
				break;
			case CE_Warning:
				s = "GDAL WARNING: " + s;
				break;
			case CE_Failure:
				s = "GDAL FAILURE: " + s;
				break;
			case CE_Fatal:
				s = "GDAL FATAL: " + s;
				break;
			default:
				s = "GDAL: " + s;
		}

		if (m_globalSettings.suppressGdalErrors)
			s = "SUPPRESSED " + s;

		if (m_globalSettings.callback && !targets[DebugForceGdal])
		{
			const CComBSTR bstr(s);
			m_globalSettings.callback->Error(m_globalSettings.gdalBstr, bstr);
		}
		else
		{
			if (Debug::IsDebugMode())
				Debug::WriteError(s);
		}
	}

	bool LogTiles()	{ return targets[DebugTiles]; }

	void Init() {
		targets[DebugAny] = true;
		targets[DebugOgrLoading] = false;
		targets[DebugPanning] = false;
		targets[DebugTiles] = false;
		targets[DebugForceGdal] = false;
		CPLSetErrorHandler(CustomCPLErrorHandler);
	}

	bool CheckDebugTarget(DebugTarget target)
	{
		return targets[target];
	}

	CString GetDebugTargetName(DebugTarget target)
	{
		switch (target)
		{
			case DebugAny: return "";
			case DebugOgrLoading: return "OGR: ";
			case DebugPanning: return "PANNING: ";
			case DebugTiles: return "TILES: ";
		}
		return "";
	}

	void PrintOutput(DebugTarget target, CString& msg, bool error = false)
	{
		CString s;
		if (target == DebugAny)
			s = (error ? "OCX ERROR: " : "OCX: ");
		else 
			s = GetDebugTargetName(target);
		s += msg + "\n";
		OutputDebugStringA(s);
	}

	// ****************************************************************** 
	//		Format
	// ****************************************************************** 
	CString Format(char* format, ...)
	{
#ifndef RELEASE_MODE		
		TCHAR buffer[1024];
		va_list args;
		va_start(args, format);
		int len = vsnprintf(buffer, _countof(buffer), format, args);
		va_end(args);
		CString s = buffer;
		return s;
#else
		return "";
#endif
	}

	// ****************************************************************** 
	//		WriteWithTime
	// ****************************************************************** 
	void WriteWithTime(CString msg, DebugTarget target)
	{
#ifndef RELEASE_MODE		
		SYSTEMTIME time;
		GetLocalTime(&time);
		CString sTime;
		sTime.Format("%02d:%02d:%02d.%-3d: ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		msg = sTime + msg;
		PrintOutput(target, msg);
#endif
	}

	// ****************************************************************** 
	//		WriteWithThreadId
	// ****************************************************************** 
	void WriteWithThreadId(CString msg, const DebugTarget target)
	{
#ifndef RELEASE_MODE
		if (!CheckDebugTarget(target)) return;

		CString s;
		s.Format("Id=%d: ", GetCurrentThreadId());
		msg = s + msg;
		WriteWithTime(msg, target);
#endif
	}

	// ****************************************************************** 
	//		WriteLine
	// ****************************************************************** 
	void WriteLine(CString format, ...)
	{
	#ifndef RELEASE_MODE	
		if (!CheckDebugTarget(DebugAny)) return;

		TCHAR buffer[1024];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, _countof(buffer), format, args);
		va_end(args);

		format = buffer;
		PrintOutput(DebugAny, format);
	#endif
	}

	// ****************************************************************** 
	//		WriteError
	// ****************************************************************** 
	void WriteError(CString format, ...)
	{
	#ifndef RELEASE_MODE	
		if (!CheckDebugTarget(DebugAny)) return;
		TCHAR buffer[1024];
		va_list args;
		va_start(args, format);
		int len = vsnprintf(buffer, _countof(buffer), format, args);
		va_end(args);

		format = buffer;
		PrintOutput(DebugAny, format, true);
	#endif
	}

	// ***************************************************
	//		IsDebugMode
	// ***************************************************
	bool IsDebugMode()
	{
		bool debug = true;
	#ifdef RELEASE_MODE
		debug = false;
	#endif
		return debug;
	}
}