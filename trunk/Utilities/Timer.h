# include <stdio.h>
# include <memory.h>
# include <iostream>	//removed ".h" 2-5-2005 dpa
# include <math.h>
# include <time.h>
# include <fstream>		//removed ".h" 2-5-2005 dpa
using namespace std;
//# include <iostream.h> why was this included twice? 2-5-2005 dpa

//# define _NO_MACRO
# define _WINDOWS_TIMER

//Timer macros and functions
# ifndef _TIMER
# define _TIMER
	
	# ifdef _WINDOWS_TIMER
		# include <windows.h>
		static double start, stop, total;
		# define Init_Timer() (total=0)
		# define Start_Timer() (start=GetTickCount())
		# define Stop_Timer() (stop=GetTickCount(),\
							   total+=stop-start)
		inline double Time_Timer() {return total;}

/*		2-2-2005 dpa This function was causing several errors, so I commented it out and now the code compiles.*/
	/* 8/23/2005 Chris Michaelis -- corrected the errors mentioned above and uncommented to perform some optimization on drawing code */
		inline void Print_Timer(ostream & out, char * tname = NULL)
		{	double elapsed_clocks = total;
			double elapsed_time = elapsed_clocks/CLOCKS_PER_SEC;
			out<<"-----------"<<endl;
			if( tname )
				out<<tname<<endl;			
			out<<"Clock Ticks:"<<elapsed_clocks<<endl;
			out<<"Time:"<<elapsed_time<<endl;
			out<<"-----------"<<endl;
			flush(out);
		}

	# else
		double total;
		static clock_t start, stop;
		# define Init_Timer() (total=0)
		# define Start_Timer() (start=clock())
		# define Stop_Timer() (stop=clock(),\
							   total+=stop-start);							 
		inline double Time_Timer() {return total;}
		inline void Print_Timer(ostream & out, char * tname = NULL)
		{	double elapsed_clocks = (double)(total);
			double elapsed_time = elapsed_clocks/CLOCKS_PER_SEC;
			out<<"-----------"<<endl;
			if( tname )
				cout<<tname<<endl;
			out<<"Clock Ticks:"<<elapsed_clocks<<endl;
			out<<"Time:"<<elapsed_time<<endl;
			out<<"-----------"<<endl;
			flush(out);
		}
	# endif
# endif //_TIMER
