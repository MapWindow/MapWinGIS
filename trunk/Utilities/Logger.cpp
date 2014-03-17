#include "stdafx.h"
#include "Logger.h"

namespace Debug
{
	void Logger::Open(CStringW filename)
	{
		this->Close();

		USES_CONVERSION;
		CStringA filenameA = W2A(filename);
		CStringA pathA = Utility::GetFolderFromPath(filenameA);

		if (!Utility::dirExists(pathA))
			_mkdir(pathA);

		m_logger.open(filenameA);

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
		TCHAR buffer[1024];
		va_list args;
		va_start( args, format);
		vsprintf( buffer, format, args );
		CString s = buffer;
		m_logger << s;
		m_logger.flush();
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
		if (this->IsOpened())
		{
			m_logger << message << endl;   m_logger.flush();
		}
	}
}