#include "StdAfx.h"
#include "Logger.h"

namespace Debug
{
	void Logger::Open(CStringW filename)
	{
		this->Close();

		//USES_CONVERSION;
		const CStringW path = Utility::GetFolderFromPath(filename);
		if (!Utility::DirExists(path))
			_wmkdir(path);

		m_logger.open(filename);

		m_filename = filename;
		Debug::WriteLine("Log opened: %d", m_logger.is_open());
		Debug::WriteLine("Log bad: %d", m_logger.bad());
		Debug::WriteLine("Log fail: %d", m_logger.fail());
		Debug::WriteLine("Log good: %d", m_logger.good());
		Debug::WriteLine("Log eof: %d", m_logger.eof());
	}

	bool Logger::IsOpened()
	{
		return m_logger.is_open() && m_logger.good();
	}

	void Logger::WriteLine(CString format, ...)
	{
		if (IsOpened() || Debug::IsDebugMode())
		{
			TCHAR buffer[1024];
			va_list args;
			va_start( args, format);
			vsprintf( buffer, format, args );
			CString s = buffer;

			if (format.GetLength() > 0)
			{
				SYSTEMTIME time;
				GetLocalTime(&time);
				CString s2;
				s2.Format("%02d:%02d:%02d.%-3d: ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
				m_logger << s2 << s << endl;

				Debug::WriteWithThreadId(s2 + s, DebugTiles);
			}
			else {
				m_logger << endl;
			}
			m_logger.flush();
		}
	}
		
	CStringW Logger::GetFilename()
	{
		return m_filename;
	}

	void Logger::Close()
	{
		if (m_logger.is_open())
		{
			m_logger.flush();
			m_logger.close();
		}
	}

	void Logger::Log(CString message)
	{
		WriteLine(message);
	}
}