/*************************************************************
 Author		: David A. Jones
 File Name	: MemLeakDetect.h
 Date		: July 30, 2004
 Synopsis	:		 
			A trace memory feature for source code to trace and
			find memory related bugs. 

 Future		:
				1) Memory corruption
				2) Freeing memory without allocating
				3) Freeing memory twice
				4) Not Freeing memory at all
				5) over running memory boundardies


****************************************************************/
#if !defined(MEMLEAKDETECT_H)
#define MEMLEAKDETECT_H

#define _CRTDBG_MAP_ALLOC

#include <map>
#define _CRTBLD
#include <windows.h>
#include "dbgint.h"
#include <ImageHlp.h>
#include <crtdbg.h>


#pragma comment( lib, "imagehlp.lib" )

using namespace std;

#ifdef _DEBUG

// if you want to use the custom stackwalker otherwise
// comment this line out
#define MLD_CUSTOMSTACKWALK			1
//
#define MLD_MAX_NAME_LENGTH			256
#define MLD_MAX_TRACEINFO			256
#define MLD_TRACEINFO_EMPTY			_T("")
#define MLD_TRACEINFO_NOSYMBOL		_T("?(?)")

#ifdef  MLD_CUSTOMSTACKWALK
#define MLD_STACKWALKER				symStackTrace2
#else
#define MLD_STACKWALKER				symStackTrace
#endif

#define AfxTrace MyTrace

typedef DWORD ADDR;

class CMemLeakDetect
{
	public:

		typedef struct 	{
				ADDRESS				addrPC;
				ADDRESS				addrFrame;
			
		} STACKFRAMEENTRY;

		typedef struct { 
				void*				address;
				DWORD				size;
				TCHAR				fileName[MLD_MAX_NAME_LENGTH];
				DWORD				lineNumber;
				DWORD				occurance;
				STACKFRAMEENTRY		traceinfo[MLD_MAX_TRACEINFO];

		} AllocBlockInfo;

		//typedef int POSITION;
		typedef map<LPVOID, AllocBlockInfo>				KEYMAP;
		typedef map<LPVOID, AllocBlockInfo>::iterator	POSITION;
		typedef pair<LPVOID, AllocBlockInfo>			KEYVALUE;

		class CMapMem
		{
			public:

				KEYMAP			m_Map;
				POSITION		m_Pos;

				inline BOOL Lookup(LPVOID pAddr,  AllocBlockInfo& aInfo) { 

					m_Pos = m_Map.find(pAddr);
					//
					if (m_Pos == m_Map.end())
					{
						return FALSE;
					}
					//
					pAddr = m_Pos->first;
					aInfo = m_Pos->second;

					return TRUE;
				};

				inline POSITION end() { 

					return m_Map.end(); 
				};

				inline void RemoveKey(LPVOID pAddr) { 
					
					m_Map.erase(pAddr);
				};

				inline void RemoveAll() {
					m_Map.clear();
				};

				void SetAt(LPVOID pAddr, AllocBlockInfo& aInfo) {

					m_Map[pAddr] = aInfo;
				};

				inline POSITION GetStartPosition() { 
					POSITION pos = m_Map.begin(); 
					return pos;
				};

				inline void GetNextAssoc(POSITION& pos, LPVOID& rAddr, AllocBlockInfo& aInfo) {

					rAddr = pos->first;
					aInfo = pos->second;
					pos++;
				};

				void InitHashTable(int preAllocEntries, BOOL flag) 	{
					 preAllocEntries	= NULL;
					 flag				= NULL;
				};

		};

		CMemLeakDetect();
		~CMemLeakDetect();
		void Init();
		void End();
		void addMemoryTrace(void* addr,  DWORD asize,  TCHAR *fname, DWORD lnum);
		void redoMemoryTrace(void* addr,  void* oldaddr, DWORD asize,  TCHAR *fname, DWORD lnum);
		void removeMemoryTrace(void* addr, void* realdataptr);
		void cleanupMemoryTrace();
		void dumpMemoryTrace();
		//

		//CMap<LPVOID, LPVOID, AllocBlockInfo, AllocBlockInfo> m_AllocatedMemoryList;
		CMapMem			 m_AllocatedMemoryList;
	DWORD memoccurance;
	bool  isLocked;
	bool stopped;
	//
	private:

		BOOL initSymInfo(TCHAR* lpUserPath);
		BOOL cleanupSymInfo();
		void symbolPaths( TCHAR* lpszSymbolPaths);
		void symStackTrace(STACKFRAMEENTRY* pStacktrace);
		void symStackTrace2(STACKFRAMEENTRY* pStacktrace);
		BOOL symFunctionInfoFromAddresses(ULONG fnAddress, ULONG stackAddress, char* lpszSymbol);
		BOOL symSourceInfoFromAddress(UINT address, TCHAR* lpszSourceInfo);
		BOOL symModuleNameFromAddress(UINT address, TCHAR* lpszModule);

		HANDLE				m_hProcess;
		PIMAGEHLP_SYMBOL	m_pSymbol;
		DWORD				m_dwsymBufSize;
};
#endif
#endif