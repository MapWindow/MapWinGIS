#pragma once
#include <iostream>
#include <fstream>

using namespace std;

namespace Debug
{
	class Logger
	{
		private:
			ofstream m_logger;
			CStringW m_filename;
		public:
			Logger(void) {};
			~Logger(void) {};
			
			bool errorsOnly;
			ofstream& out() { return m_logger; }
			void Open(CStringW filename);
			void WriteLine(CString format, ...);
			bool IsOpened();
			CStringW GetFilename();
			void Close();
			void Log(CString message);
	};
}