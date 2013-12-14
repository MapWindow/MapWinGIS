/*************************************************************
 Author		: David A. Jones
 File Name	: MemLeakDetect.h
 Date		: July 30, 2004
 Synopsis		 
			A trace memory feature for source code to trace and
			find memory related bugs. 


****************************************************************/
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//
#pragma warning(disable:4312)
#pragma warning(disable:4313)
#pragma warning(disable:4267)
#pragma warning(disable:4100)

#include "stdafx.h"
#include "MemLeakDetect.h"

#ifdef _DEBUG
static CMemLeakDetect*	g_pMemTrace			= NULL;
static _CRT_ALLOC_HOOK	pfnOldCrtAllocHook	= NULL;
extern int _crtDgbFlag;


int MyTrace(LPCTSTR lpszFormat, ...)
{
	TCHAR buffer[1024];
 	va_list args;
	va_start( args, lpszFormat);
	vsprintf( buffer, lpszFormat, args );

	return _CrtDbgReport(_CRT_WARN,NULL,NULL,NULL,buffer);

}

int catchMemoryAllocHook(int	allocType, 
						 void	*userData, 
						 size_t size, 
						 int	blockType, 
						 long	requestNumber, 
		  const unsigned char	*filename, 
						 int	lineNumber)
{
	
	_CrtMemBlockHeader *pCrtHead;
	long prevRequestNumber;

	// internal C library internal allocations
	if ( blockType == _CRT_BLOCK || gMemLeakDetect.stopped)
	{
		return( TRUE );
	}
	// check if someone has turned off mem tracing
	if  ((( _CRTDBG_ALLOC_MEM_DF & _crtDbgFlag) == 0) && 
		(( allocType			== _HOOK_ALLOC)		|| 
			( allocType			== _HOOK_REALLOC)))
	{
		if (pfnOldCrtAllocHook)
		{
			pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
		}
		return TRUE;
	}

	// protect if mem trace is not initialized
	if (g_pMemTrace == NULL)
	{
		if (pfnOldCrtAllocHook)
		{
			pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
		}
		return TRUE;
	}

	// protect internal mem trace allocs
	if (g_pMemTrace->isLocked)
	{
		if (pfnOldCrtAllocHook)
		{
			pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
		}
		return( TRUE);
	}
	// lock the function
	g_pMemTrace->isLocked = true;
	//
	if (allocType == _HOOK_ALLOC)
	{
		g_pMemTrace->addMemoryTrace((void *) requestNumber, size, (char*)filename, lineNumber);
	}
	else
	if (allocType == _HOOK_REALLOC)
	{
		if (_CrtIsValidHeapPointer(userData))
		{
			pCrtHead = pHdr(userData);
			prevRequestNumber = pCrtHead->lRequest;
			//
			if (pCrtHead->nBlockUse == _IGNORE_BLOCK)
			{
				if (pfnOldCrtAllocHook)
				{
					pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
				}
				goto END;
			}
	   		g_pMemTrace->redoMemoryTrace((void *) requestNumber, (void *) prevRequestNumber, size, (char*)filename, lineNumber);
		}
	}
	else
	if (allocType == _HOOK_FREE)
	{
		if (_CrtIsValidHeapPointer(userData))
		{
			pCrtHead = pHdr(userData);
			requestNumber = pCrtHead->lRequest;
			//
			if (pCrtHead->nBlockUse == _IGNORE_BLOCK)
			{
				if (pfnOldCrtAllocHook)
				{
					pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
				}
				goto END;
			}
	   		g_pMemTrace->removeMemoryTrace((void *) requestNumber, userData);
		}
	}
END:
	// unlock the function
	g_pMemTrace->isLocked = false;
	return TRUE;
}

void CMemLeakDetect::addMemoryTrace(void* addr,  DWORD asize,  TCHAR *fname, DWORD lnum)
{
	if (this->stopped)
		return;
	
	AllocBlockInfo ainfo;
	//
	if (m_AllocatedMemoryList.Lookup(addr, ainfo))
	{
		// already allocated
		AfxTrace("ERROR!CMemLeakDetect::addMemoryTrace() Address(0x%08X) already allocated\n", addr);
		return;
	}
	//
	ainfo.address		= addr;
	ainfo.lineNumber	= lnum;
	ainfo.size			= asize;
	ainfo.occurance		= memoccurance++;
	MLD_STACKWALKER(&ainfo.traceinfo[0]);
	//
	if (fname)
		_tcsncpy(&ainfo.fileName[0], fname, MLD_MAX_NAME_LENGTH);
	else
	  ainfo.fileName[0] = 0;
	//
	m_AllocatedMemoryList.SetAt(addr, ainfo);
};
void CMemLeakDetect::redoMemoryTrace(void* addr,  void* oldaddr, DWORD asize,  char *fname, DWORD lnum)
{
	if (this->stopped)
		return;
	
	AllocBlockInfo ainfo;

	if (m_AllocatedMemoryList.Lookup(oldaddr,(AllocBlockInfo &) ainfo))
	{
		m_AllocatedMemoryList.RemoveKey(oldaddr);
	}
	else
	{
		AfxTrace(_T("ERROR!CMemLeakDetect::redoMemoryTrace() didnt find Address(0x%08X) to free\n"), oldaddr);
	}
	//
	ainfo.address		= addr;
	ainfo.lineNumber	= lnum;
	ainfo.size			= asize;
	ainfo.occurance		= memoccurance++;
	MLD_STACKWALKER(&ainfo.traceinfo[0]);
	//
	if (fname)
		_tcsncpy(&ainfo.fileName[0], fname, MLD_MAX_NAME_LENGTH);
	else
	  ainfo.fileName[0] = 0;

	m_AllocatedMemoryList.SetAt(addr, ainfo);
};
void CMemLeakDetect::removeMemoryTrace(void* addr, void* realdataptr)
{
	AllocBlockInfo ainfo;
	//
	if (m_AllocatedMemoryList.Lookup(addr,(AllocBlockInfo &) ainfo))
	{
		m_AllocatedMemoryList.RemoveKey(addr);
	}
	else
	{
	   //freeing unallocated memory
		//AfxTrace(_T("ERROR!CMemLeakDetect::removeMemoryTrace() didnt find Address(0x%08X) to free\n"), addr);
	}
};
void CMemLeakDetect::cleanupMemoryTrace()
{
	m_AllocatedMemoryList.RemoveAll();
};

void CMemLeakDetect::dumpMemoryTrace()
{
	POSITION			pos;
	LPVOID				addr;
	AllocBlockInfo		ainfo;
	TCHAR				buf[MLD_MAX_NAME_LENGTH];
	TCHAR				symInfo[MLD_MAX_NAME_LENGTH];
	TCHAR				srcInfo[MLD_MAX_NAME_LENGTH];
	int					totalSize						= 0;
	int					numLeaks						= 0;
	STACKFRAMEENTRY*	p								= 0;

	//
	_tcscpy(symInfo, MLD_TRACEINFO_NOSYMBOL);
	_tcscpy(srcInfo, MLD_TRACEINFO_NOSYMBOL);
	//
	pos = m_AllocatedMemoryList.GetStartPosition();
	//
	while(pos != m_AllocatedMemoryList.end())
	{
		numLeaks++;
		sprintf(buf, "Memory Leak(%d)------------------->\n", numLeaks);
		AfxTrace(buf);
		//
		m_AllocatedMemoryList.GetNextAssoc(pos, (LPVOID &) addr, (AllocBlockInfo&) ainfo);

		if (ainfo.fileName[0] != NULL)
		{
			sprintf(buf, "Memory Leak <0x%X> bytes(%d) occurance(%d) %s(%d)\n", 
					ainfo.address, ainfo.size, ainfo.occurance, ainfo.fileName, ainfo.lineNumber);
		}
		else
		{
			sprintf(buf, "Memory Leak <0x%X> bytes(%d) occurance(%d)\n", 
					ainfo.address, ainfo.size, ainfo.occurance);
		}
		//
		AfxTrace(buf);
		//
		p = &ainfo.traceinfo[0];
		while(p[0].addrPC.Offset)
		{
			symFunctionInfoFromAddresses( p[0].addrPC.Offset, p[0].addrFrame.Offset, symInfo );
			symSourceInfoFromAddress(     p[0].addrPC.Offset, srcInfo );
			AfxTrace("%s->%s()\n", srcInfo, symInfo);
			p++;
		}
		totalSize += ainfo.size;
	}
	_stprintf(buf, _T("\n-----------------------------------------------------------\n"));
	AfxTrace(buf);
	if(!totalSize) 
	{
		_stprintf(buf,_T("No Memory Leaks Detected for %d Allocations\n\n"), memoccurance);
		AfxTrace(buf);
	}
	else
	{
		_stprintf(buf, _T("Total %d Memory Leaks: %d bytes Total Alocations %d\n\n"), numLeaks, totalSize, memoccurance);
		AfxTrace(buf);
	}
}

void CMemLeakDetect::Init()
{
	  m_dwsymBufSize		= (MLD_MAX_NAME_LENGTH + sizeof(PIMAGEHLP_SYMBOL));
	  m_hProcess			= GetCurrentProcess();
	  m_pSymbol				= (PIMAGEHLP_SYMBOL)GlobalAlloc( GMEM_FIXED, m_dwsymBufSize);

	  m_AllocatedMemoryList.InitHashTable(10211, TRUE);
	  initSymInfo( NULL );
	  isLocked				= false;
	  g_pMemTrace			= this;
	  pfnOldCrtAllocHook	= _CrtSetAllocHook( catchMemoryAllocHook ); 
}
void CMemLeakDetect::End()
{
	isLocked				= true;
	_CrtSetAllocHook(pfnOldCrtAllocHook);
	dumpMemoryTrace();
	cleanupMemoryTrace();
	cleanupSymInfo();
	GlobalFree(m_pSymbol);
	g_pMemTrace				= NULL;
}
CMemLeakDetect::CMemLeakDetect()
{
	stopped = false;
	Init();
}

CMemLeakDetect::~CMemLeakDetect()
{
	Debug::WriteLine("CMemLeakDetect destructor");
	End();
}

// PRIVATE STUFF
void CMemLeakDetect::symbolPaths( TCHAR* lpszSymbolPath)
{
	TCHAR lpszPath[MLD_MAX_NAME_LENGTH];

   // Creating the default path where the dgbhelp.dll is located
   // ".;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;%SYSTEMROOT%;%SYSTEMROOT%\System32;"
	_tcscpy( lpszSymbolPath, _T(".;..\\;..\\..\\"));

	// environment variable _NT_SYMBOL_PATH
	if ( GetEnvironmentVariable(_T("_NT_SYMBOL_PATH"), lpszPath, MLD_MAX_NAME_LENGTH ))
	{
	    strcat( lpszSymbolPath, _T(";"));
		strcat( lpszSymbolPath, lpszPath );
	}

	// environment variable _NT_ALTERNATE_SYMBOL_PATH
	if ( GetEnvironmentVariable( _T("_NT_ALTERNATE_SYMBOL_PATH"), lpszPath, MLD_MAX_NAME_LENGTH ))
	{
		_tcscat( lpszSymbolPath, _T(";"));
		_tcscat( lpszSymbolPath, lpszPath );
	}

	// environment variable SYSTEMROOT
	if ( GetEnvironmentVariableA( "SYSTEMROOT", lpszPath, MLD_MAX_NAME_LENGTH ) )
	{
	    _tcscat( lpszSymbolPath, _T(";"));
		_tcscat( lpszSymbolPath, lpszPath);
		_tcscat( lpszSymbolPath, _T(";"));

		// SYSTEMROOT\System32
		_tcscat( lpszSymbolPath, lpszPath );
		_tcscat( lpszSymbolPath, _T("\\System32"));
	}
}

BOOL CMemLeakDetect::cleanupSymInfo()
{
	return SymCleanup( GetCurrentProcess() );
}

// Initializes the symbol files
BOOL CMemLeakDetect::initSymInfo( TCHAR* lpszUserSymbolPath )
{
	CHAR    lpszSymbolPath[MLD_MAX_NAME_LENGTH];
    DWORD   symOptions = SymGetOptions();

	symOptions |= SYMOPT_LOAD_LINES; 
	symOptions &= ~SYMOPT_UNDNAME;
	SymSetOptions( symOptions );

    // Get the search path for the symbol files
	symbolPaths( lpszSymbolPath);
	//
	if (lpszUserSymbolPath)
	{
		_tcscat(lpszSymbolPath, _T(";"));
		_tcscat(lpszSymbolPath, lpszUserSymbolPath);
	}
	return SymInitialize( GetCurrentProcess(), lpszSymbolPath, TRUE);
}

void CMemLeakDetect::symStackTrace(STACKFRAMEENTRY* pStacktrace )
{
	STACKFRAME     callStack;
	BOOL           bResult;
	CONTEXT        context;
	HANDLE		   hThread  = GetCurrentThread();

	// get the context
	memset( &context, NULL, sizeof(context) );
	context.ContextFlags = CONTEXT_FULL;
	if ( !GetThreadContext( hThread, &context ) )
	{
       AfxTrace("Call stack info(thread=0x%X) failed.\n", hThread );
	   return;
	}
	//initialize the call stack
	memset( &callStack, NULL, sizeof(callStack) );
	callStack.AddrPC.Offset    = context.Eip;
	callStack.AddrStack.Offset = context.Esp;
	callStack.AddrFrame.Offset = context.Ebp;
	callStack.AddrPC.Mode      = AddrModeFlat;
	callStack.AddrStack.Mode   = AddrModeFlat;
	callStack.AddrFrame.Mode   = AddrModeFlat;
	//
	for( DWORD index = 0; index < MLD_MAX_TRACEINFO; index++ ) 
	{
		bResult = StackWalk(IMAGE_FILE_MACHINE_I386,
							m_hProcess,
							hThread,
							&callStack,
							NULL, 
							NULL,
							SymFunctionTableAccess,
							SymGetModuleBase,
							NULL);

		//if ( index == 0 )
		 //  continue;

		if( !bResult || callStack.AddrFrame.Offset == 0 ) 
			break;
		//
		pStacktrace[0].addrPC	 = callStack.AddrPC;
		pStacktrace[0].addrFrame = callStack.AddrFrame;
		pStacktrace++;
	}
	//clear the last entry
	memset(pStacktrace, NULL, sizeof(STACKFRAMEENTRY));
}

//
// This code is still under investigation
// I have to test this code and make sure it is compatible
// with the other stack walker!
//
void CMemLeakDetect::symStackTrace2(STACKFRAMEENTRY* pStacktrace )
{
	ADDR			FramePtr				= NULL;
	ADDR			InstructionPtr			= NULL;
	ADDR			OriFramePtr				= NULL;
	ADDR			PrevFramePtr			= NULL;
	long			StackIndex				= NULL;

	// Get frame pointer
	_asm mov DWORD PTR [OriFramePtr], ebp

	FramePtr = OriFramePtr;

	//
	while (FramePtr)
	{
		InstructionPtr = ((ADDR *)FramePtr)[1];

		pStacktrace[StackIndex].addrPC.Offset	= InstructionPtr;
		pStacktrace[StackIndex].addrPC.Segment	= NULL;
		pStacktrace[StackIndex].addrPC.Mode		= AddrModeFlat;
		//
		StackIndex++;
		PrevFramePtr			= FramePtr;
		FramePtr				= ((ADDR *)FramePtr)[0];
	}
}
BOOL CMemLeakDetect::symFunctionInfoFromAddresses( ULONG fnAddress, ULONG stackAddress, LPTSTR lpszSymbol )
{
	DWORD             dwDisp	= 0;

	::ZeroMemory(m_pSymbol, m_dwsymBufSize );
	m_pSymbol->SizeOfStruct		= m_dwsymBufSize;
	m_pSymbol->MaxNameLength	= m_dwsymBufSize - sizeof(IMAGEHLP_SYMBOL);

    // Set the default to unknown
	_tcscpy( lpszSymbol, MLD_TRACEINFO_NOSYMBOL);

	// Get symbol info for IP
	if ( SymGetSymFromAddr( m_hProcess, (ULONG)fnAddress, &dwDisp, m_pSymbol ) )
	{
		_tcscpy(lpszSymbol, m_pSymbol->Name);
		return TRUE;
	}
	//create the symbol using the address because we have no symbol
	_stprintf(lpszSymbol, "0x%08X", fnAddress);
	return FALSE;
}

BOOL CMemLeakDetect::symSourceInfoFromAddress(UINT address, TCHAR* lpszSourceInfo)
{
	BOOL           ret = FALSE;
	IMAGEHLP_LINE  lineInfo;
	DWORD          dwDisp;
	TCHAR          lpModuleInfo[MLD_MAX_NAME_LENGTH] = MLD_TRACEINFO_EMPTY;

	_tcscpy( lpszSourceInfo, MLD_TRACEINFO_NOSYMBOL);

	memset( &lineInfo, NULL, sizeof( IMAGEHLP_LINE ) );
	lineInfo.SizeOfStruct = sizeof( IMAGEHLP_LINE );

	if ( SymGetLineFromAddr( m_hProcess, address, &dwDisp, &lineInfo ) )
	{
	   // Using the "sourcefile(linenumber)" format
		_stprintf( lpszSourceInfo, _T("%s(%d): 0x%08X"), lineInfo.FileName, lineInfo.LineNumber, address );
		ret = TRUE;
	}
	else
	{
        // Using the "modulename!address" format
	  	symModuleNameFromAddress( address, lpModuleInfo );

		if ( lpModuleInfo[0] == _T('?') || lpModuleInfo[0] == _T('\0'))
		{
			// Using the "address" format
			_stprintf(lpszSourceInfo, _T("0x%08X"), lpModuleInfo, address );
		}
		else
		{
			_stprintf(lpszSourceInfo, _T("%sdll! 0x%08X"), lpModuleInfo, address );
		}
		ret = FALSE;
	}
	//
	return ret;
}

BOOL CMemLeakDetect::symModuleNameFromAddress( UINT address, TCHAR* lpszModule )
{
	BOOL              ret = FALSE;
	IMAGEHLP_MODULE   moduleInfo;

	::ZeroMemory( &moduleInfo, sizeof(IMAGEHLP_MODULE) );
	moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

	if ( SymGetModuleInfo( m_hProcess, (DWORD)address, &moduleInfo ) )
	{
		_tcscpy(moduleInfo.ModuleName, lpszModule);
		ret = TRUE;
	}
	else
	{
		_tcscpy( lpszModule, MLD_TRACEINFO_NOSYMBOL);
	}
	
	return ret;
}
#endif