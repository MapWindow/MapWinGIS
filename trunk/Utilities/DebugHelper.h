#pragma once

namespace Debug
{
	void CPL_STDCALL CustomCPLErrorHandler(CPLErr error, int errorCode, const char* message);
	void Init();
	bool IsDebugMode();
	void WriteWithThreadId(CString format, DebugTarget target = DebugAny);
	void WriteWithTime(CString msg, DebugTarget target = DebugAny);
	void WriteLine(CString format, ...);
	void WriteError(CString format, ...);
	CString Format(char* format, ...);
	bool LogTiles();
}

