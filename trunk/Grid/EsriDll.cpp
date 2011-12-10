#include "stdafx.h"
#include "EsriDll.h"
#include <deque>
#pragma warning(disable : 4996)

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//					Esri Functions
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

ESRI_PUTWINDOWROWFLOAT_PROC putwindowrowfloat = NULL;
ESRI_PUTWINDOWROW_PROC putwindowrow = NULL;
ESRI_CELLLYRCLOSE_PROC celllyrclose = NULL;
ESRI_GRIDCOPY_PROC gridcopy = NULL;
ESRI_CELLLAYEROPEN_PROC celllayeropen = NULL;
ESRI_BNDCELLREAD_PROC bndcellread = NULL;
ESRI_PRIVATEACCESSWINDOWSET_PROC privateaccesswindowset = NULL;
ESRI_GETMISSINGFLOAT_PROC getmissingfloat = NULL;
ESRI_PRIVATEWINDOWCOLS_PROC privatewindowcols = NULL;
ESRI_PRIVATEWINDOWROWS_PROC privatewindowrows = NULL;
ESRI_GETWINDOWROW_PROC getwindowrow = NULL;
ESRI_CELLLYREXISTS_PROC celllyrexists = NULL;
ESRI_GRIDDELETE_PROC griddelete = NULL;
ESRI_CELLLAYERCREATE_PROC celllayercreate = NULL;



bool esri_initialized = false;
bool avdll = false;
bool aidll = false;
int num_active_grids = 0;
HINSTANCE esri_dll = NULL;
CString current_working_directory = "";
CString esri_working_directory = "";


bool esri_directory()
{	if( _chdir( (LPCTSTR)esri_working_directory ) == 0 )
		return true;
	return false;
}

bool working_directory()
{	if( _chdir( (LPCTSTR)current_working_directory ) == 0 )
		return true;
	return false;
}

#pragma optimize("", off)
bool initialize_esri()
{	
	num_active_grids++;

	if( !esri_initialized )
	{	
		avdll = false;
		aidll = false;

		if( find_esri_dll(true) )
		{	
			int setup = 0;
			
			//AfxMessageBox("Found Esri DLL");

			if( esri_directory() )
			{	
				//AfxMessageBox("gridiosetup 1");
				ESRI_GRIDIOSETUP_PROC gridiosetup;
				if( avdll )
					gridiosetup = (ESRI_GRIDIOSETUP_PROC)GetProcAddress( esri_dll, "GridIOSetup" );
				else if( aidll )
					gridiosetup = (ESRI_GRIDIOSETUP_PROC)GetProcAddress( esri_dll, "GridIOInit" );
				//AfxMessageBox("gridiosetup 2");
				if( gridiosetup != NULL )
				{	//AfxMessageBox("gridiosetup 3");					
					setup = gridiosetup();
					//AfxMessageBox("gridiosetup 4");
					working_directory();
					//AfxMessageBox("gridiosetup 5");
				}
				//AfxMessageBox("gridiosetup 6");
			}
			//AfxMessageBox("gridiosetup 7");
			
			if( setup != 0 )
			{	
				putwindowrowfloat = (ESRI_PUTWINDOWROWFLOAT_PROC)GetProcAddress( esri_dll, "PutWindowRow" );				
				putwindowrow = (ESRI_PUTWINDOWROW_PROC)GetProcAddress( esri_dll, "PutWindowRow" );
				celllyrclose = (ESRI_CELLLYRCLOSE_PROC)GetProcAddress( esri_dll,"CellLyrClose");
				gridcopy = (ESRI_GRIDCOPY_PROC)GetProcAddress(esri_dll, "GridCopy");
				celllayeropen = (ESRI_CELLLAYEROPEN_PROC)GetProcAddress(esri_dll,"CellLayerOpen");
				bndcellread = (ESRI_BNDCELLREAD_PROC)GetProcAddress(esri_dll,"BndCellRead");			
				privateaccesswindowset = (ESRI_PRIVATEACCESSWINDOWSET_PROC)GetProcAddress(esri_dll,"PrivateAccessWindowSet");
				getmissingfloat = (ESRI_GETMISSINGFLOAT_PROC)GetProcAddress(esri_dll, "GetMissingFloat" );		
				privatewindowcols = (ESRI_PRIVATEWINDOWCOLS_PROC)GetProcAddress( esri_dll, "PrivateWindowCols");
				privatewindowrows = (ESRI_PRIVATEWINDOWROWS_PROC)GetProcAddress( esri_dll, "PrivateWindowRows");
				getwindowrow = (ESRI_GETWINDOWROW_PROC)GetProcAddress( esri_dll, "GetWindowRow" );
				celllyrexists = (ESRI_CELLLYREXISTS_PROC)GetProcAddress( esri_dll, "CellLyrExists" );
				griddelete = (ESRI_GRIDDELETE_PROC)GetProcAddress( esri_dll, "GridDelete" );
				celllayercreate = (ESRI_CELLLAYERCREATE_PROC)GetProcAddress(esri_dll, "CellLayerCreate" );

				esri_initialized = true;
			}
		}
	}

	return esri_initialized;
}
#pragma optimize("", on)

#pragma optimize("", off)
void shutdown_esri()
{	

	num_active_grids--;

	if( num_active_grids <= 0 )
	{
		if( esri_initialized == true )
		{	ESRI_GRIDIOEXIT_PROC gridioexit = (ESRI_GRIDIOEXIT_PROC)GetProcAddress( esri_dll, "GridIOExit" );
			
			if( gridioexit != NULL )
				gridioexit();
		}

		if( esri_dll != NULL )
			FreeLibrary( esri_dll );
		esri_dll = NULL;

		putwindowrow = NULL;
		celllyrclose = NULL;
		gridcopy = NULL;
		celllayeropen = NULL;
		bndcellread = NULL;
		privateaccesswindowset = NULL;
		getmissingfloat = NULL;
		privatewindowcols = NULL;
		privatewindowrows = NULL;
		getwindowrow = NULL;
		celllyrexists = NULL;
		griddelete = NULL;
		celllayercreate = NULL;

		esri_initialized = false;
	}
	
}
#pragma optimize("", on)

bool can_use_esri_grids()
{	bool result = initialize_esri();
	shutdown_esri();
	return result;
}

#pragma optimize("", off)
char *CAllocate1(int number ,int size )
{
	return (char *)calloc(number, size);
}
#pragma optimize("", on )

#pragma optimize("", off )
void CFree1(char *ptr)
{
	free(ptr);
	ptr = NULL;
}
#pragma optimize("", on )

bool find_avgrid_dll(bool load_library)
{	
	bool found_esri_dlls = false;
	//Create a CRegistryKey from
	//and subsequently Opens it in the Registry
	//The Value is then queried and is stored in val

	char esri_key_to_find1[] = "software\\esri";
	std::deque< RegistryKey * > subKeyList;

	RegistryKey * MainRegKey = new RegistryKey();
	RegistryKey * CurSubKey = NULL;
	RegistryKey * tempSubKey = NULL;
	unsigned long NumSubKeys = 0;
	unsigned long NumValues = 0;


	if (MainRegKey->OpenKey(HKEY_LOCAL_MACHINE,esri_key_to_find1) == false)
		return false;

	//push the main key onto the list of keys to examine
	subKeyList.push_back(MainRegKey);

	while( subKeyList.size() > 0 && found_esri_dlls == false)
	{	
		CurSubKey = subKeyList[0];//get the next subkey to look at
		subKeyList.pop_front();//remove it from the list
		NumSubKeys = CurSubKey->getNumSubKeys();

		unsigned long i = 0;

		//add all of the current subkey's children to the list to check
		for (i = 0; i < NumSubKeys; i++)
		{

			tempSubKey = CurSubKey->getSubKey(i);
			if (tempSubKey != NULL)
				subKeyList.push_back(tempSubKey);
			tempSubKey = NULL;
		}

		NumValues = CurSubKey->getNumValues();
		//now look at the current key's set of values and see if it has a "path" value
		for (i = 0; i < NumValues; i++)
		{	
			CString value_name;
			if (CurSubKey->getValueType(i) == REG_SZ)//is the data type a string?
				value_name = CurSubKey->getValueName(i);
			else
				continue;//jump to the next value

			if( value_name.CompareNoCase("PATH") == 0 )
			{	

				CString value = (char*)CurSubKey->getValueData(i);

				CString dll_one = value + "\\avgridio.dll";
				CString dll_two = value + "\\avfeat.dll";
				CString dll_three = value + "\\avbase.dll";

				CFileFind cff;												
				if( !cff.FindFile( dll_one ) )
				{	//Keep Searching
					continue;				
				}
				if( !cff.FindFile( dll_two ) )
				{	//Keep Searching
					continue;				
				}
				if( !cff.FindFile( dll_three ) )
				{	//Keep Searching
					continue;				
				}

				if( load_library )
				{
					esri_dll = LoadLibraryEx(TEXT(dll_one), NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
					if( esri_dll == NULL )
					{	//Keep Searching
						continue;				
					}
				}

				//Esri has hardcoded their path names!!!  It is necessary to change
				//the current working directory to their directory to call functions
				esri_working_directory = value;
				char * cwd = new char[_MAX_PATH];
				_getcwd(cwd,_MAX_PATH);
				if( cwd == NULL ){
					//clean up Memory for any remaining Subkeys
					while (subKeyList.size() > 0)
					{
						delete subKeyList[0];
						subKeyList.pop_front();
					}
					return false;
				}

				//Guarantee that we can switch to their directory and back
				current_working_directory = (TCHAR*)cwd;
				if( esri_directory() )
				{	
					if( working_directory() )
					{
						//clean up Memory for any remaining Subkeys
						while (subKeyList.size() > 0)
						{
							delete subKeyList[0];
							subKeyList.pop_front();
						}
						return true;
					}
					else{
						//clean up Memory for any remaining Subkeys
						while (subKeyList.size() > 0)
						{
							delete subKeyList[0];
							subKeyList.pop_front();
						}
						return false;
					}
				}
				else
				{
					//clean up Memory for any remaining Subkeys
					while (subKeyList.size() > 0)
					{
						delete subKeyList[0];
						subKeyList.pop_front();
					}
					return false;
				}
			}
		}
		if (CurSubKey != NULL)
			delete CurSubKey;
	}

	//clean up Memory for any remaining Subkeys
	while (subKeyList.size() > 0)
	{
		delete subKeyList[0];
		subKeyList.pop_front();
	}

	return false;//if we got here, something went wrong
}
bool find_aigrid_dll(bool load_library)
{	
	bool found_esri_dlls = false;
	//Create a CRegistryKey from
	//and subsequently Opens it in the Registry
	//The Value is then queried and is stored in val

	//AfxMessageBox("Finding aigrid.dll");

	char esri_key_to_find1[] = "software\\esri";
	std::deque< RegistryKey * > subKeyList;

	RegistryKey * MainRegKey = new RegistryKey();
	RegistryKey * CurSubKey = NULL;
	RegistryKey * tempSubKey = NULL;
	unsigned long NumSubKeys = 0;
	unsigned long NumValues = 0;


	if (MainRegKey->OpenKey(HKEY_LOCAL_MACHINE,esri_key_to_find1) == false)
		return false;

	//push the main key onto the list of keys to examine
	subKeyList.push_back(MainRegKey);

	while( subKeyList.size() > 0 && found_esri_dlls == false )
	{	
		CurSubKey = subKeyList[0];//get the next subkey to look at
		subKeyList.pop_front();//remove it from the list
		NumSubKeys = CurSubKey->getNumSubKeys();

		unsigned long i = 0;

		//add all of the current subkey's children to the list to check
		for (i = 0; i < NumSubKeys; i++)
		{

			tempSubKey = CurSubKey->getSubKey(i);
			if (tempSubKey != NULL)
				subKeyList.push_back(tempSubKey);
			tempSubKey = NULL;
		}

		NumValues = CurSubKey->getNumValues();
		//now look at the current key's set of values and see if it has a "path" value
		for (i = 0; i < NumValues; i++)
		{	
			CString value_name;
			if (CurSubKey->getValueType(i) == REG_SZ)//is the data type a string?
				value_name = CurSubKey->getValueName(i);
			else
				continue;//jump to the next value

			if( value_name.CompareNoCase("INSTALLDIR") == 0 )
			{	
				CString value = (char*)CurSubKey->getValueData(i);

				//AfxMessageBox("Possible Location: " + value);

				CString dll_one = value + "\\bin\\aigridio.dll";
				CString dll_two = value + "\\bin\\aifeat.dll";
				CString dll_three = value + "\\bin\\aibase.dll";

				CFileFind cff;												
				if( !cff.FindFile( dll_one ) )
				{	//Keep Searching
					continue;				
				}
				if( !cff.FindFile( dll_two ) )
				{	//Keep Searching
					continue;				
				}
				if( !cff.FindFile( dll_three ) )
				{	//Keep Searching
					continue;				
				}

				//AfxMessageBox("Loading aigrid.dll");

				if( load_library )
				{
					esri_dll = LoadLibraryEx(TEXT(dll_one), NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
					if( esri_dll == NULL )
					{	//Keep Searching
						continue;				
					}
				}

				//AfxMessageBox("DLL Loaded!");

				//Esri has hardcoded their path names!!!  It is necessary to change
				//the current working directory to their directory to call functions
				//AfxMessageBox("1");
				esri_working_directory = value;
				//AfxMessageBox("2");
				char * cwd = new char[_MAX_PATH];
				//AfxMessageBox("3");
				_getcwd(cwd,_MAX_PATH);
				//AfxMessageBox("4");
				if( cwd == NULL ){
					//clean up Memory for any remaining Subkeys
					while (subKeyList.size() > 0)
					{
						delete subKeyList[0];
						subKeyList.pop_front();
					}
					return false;
				}
				//AfxMessageBox("5");

				//Guarantee that we can switch to their directory and back
				current_working_directory = (TCHAR*)cwd;
				//AfxMessageBox("6");
				if( esri_directory() )
				{	//AfxMessageBox("7");
					if( working_directory() )
					{	//AfxMessageBox("8");
						//clean up Memory for any remaining Subkeys
						while (subKeyList.size() > 0)
						{
							delete subKeyList[0];
							subKeyList.pop_front();
						}
						return true;
					}
					else
					{	
						FreeLibrary( esri_dll );
						esri_dll = NULL;
						//clean up Memory for any remaining Subkeys
						while (subKeyList.size() > 0)
						{
							delete subKeyList[0];
							subKeyList.pop_front();
						}
						return false;
					}
				}
				else
				{	
					FreeLibrary( esri_dll );
					esri_dll = NULL;
					//clean up Memory for any remaining Subkeys
					while (subKeyList.size() > 0)
					{
						delete subKeyList[0];
						subKeyList.pop_front();
					}
					return false;
				}
			}
		}
	}

	//clean up Memory for any remaining Subkeys
	while (subKeyList.size() > 0)
	{
		delete subKeyList[0];
		subKeyList.pop_front();
	}

	return false;//if we got here, something went wrong
}

bool find_esri_dll( bool load_library )
{	
	/*if( find_aigrid_dll( load_library ) )
	{	aidll = true;
		return true;
	}
	else*/
	if( find_avgrid_dll( load_library ) )
	{	avdll = true;
		return true;
	}
	return false;
}

#pragma optimize("", off)

DATA_TYPE esri_data_type( CString filename )
{	
	double * csize = new double(0);
	int * type = new int(-1);
	DATA_TYPE type_code = INVALID_DATA_TYPE;
	
	if( celllayeropen != NULL )
	{	
		char * e_filename = new char[_MAX_PATH+1];		
		if( GetShortPathName(filename,e_filename,_MAX_PATH) == 0 )
			strcpy( e_filename, filename );		
		if( e_filename[filename.GetLength() - 1] != '\\' && e_filename[filename.GetLength() - 1] != '/' )
			strcat( e_filename, "\\" );	

		int layer = celllayeropen(e_filename,READONLY,ROWIO,type,csize);
		if( layer >= 0 )
		{	if( (*type) == CELLINT )
				type_code = LONG_TYPE;
			else if( (*type) == CELLFLOAT )
				type_code = FLOAT_TYPE;
			
			if( celllyrclose != NULL )
				celllyrclose(layer);
		}
		delete [] e_filename;
	}
	
	return type_code;
}

#pragma optimize("", on)

#pragma optimize("", off)
void delete_esri_grid( CString filename )
{	
	if( griddelete != NULL )
	{	
		char * fname = new char[ filename.GetLength() + 1 ];
		strcpy( fname, filename );
		griddelete( fname );	
		delete [] fname;
		fname = NULL;
	}
}
#pragma optimize("", on)
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//				End of Esri Functions
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
