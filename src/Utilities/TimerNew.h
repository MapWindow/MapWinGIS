#pragma once
#include <time.h>
#include <windows.h>
#include <iostream>
#include <fstream>
using namespace std;

class CTimer
{
private:
	double start_time;
	bool started;
	ofstream out;
public:
	CTimer()
	{
		started = false;
	};
	~CTimer()
	{
		if (out.good())
			out.close();
	};
	
	void Init(const char* filename)
	{
		out.open(filename, ios_base::out);
	};

	void Start()
	{
		out << "\n------Starting timer-------" << endl;
		start_time = GetTickCount();
		started = true;
	};
	void PrintTime(const char* message)
	{
		if (started)
		{
			double elapsed_seconds = (GetTickCount() - start_time)/CLOCKS_PER_SEC;
			out << message << ":\t" << elapsed_seconds << " sec" << endl;
		}
		else
		{
			out << message << ":\t" << "timer wasn't started" << endl;
		}
		out.flush();
	};
	void Stop()
	{
		started = false;
	};
};
