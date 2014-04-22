// Registry.h: interface for the Registry class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __REGISTRY_H__
#define __REGISTRY_H__

#ifndef _WINDOWS_
	#include <windows.h>
#endif

#include <winreg.h>
#include <iostream>
using namespace std;

//Error Codes
const int REG_NO_ERROR = 0;
const int REG_KEY_NOT_OPEN = -1;
const int REG_INVALID_NULL_PARAM = -2;
const int REG_INDEX_OUT_OF_BOUNDS = -3;

struct KeyValue
{
	KeyValue()
	{
		name = NULL;
		type = REG_NONE;
		dataSize = 0;
	}

	~KeyValue()
	{
		if (name != NULL)
			delete [] name;
		name = NULL;
	}

	DWORD type;
	char * name;
	unsigned long dataSize;//in bytes
};


class RegistryKey
{
public:
	DWORD getValueType(unsigned long Index);
	char * getKeyName();
	BYTE * getValueData(unsigned long ValueIndex);
	RegistryKey * getSubKey(unsigned long Index);
	void Close();
	unsigned long getNumSubKeys();
	unsigned long getNumValues();
	bool isOpen();
	//char * getErrorMsg();
	char * getValueName(unsigned long KeyIndex);
	bool OpenKey(HKEY BaseSection, char * KeyName);
	
	RegistryKey();
	virtual ~RegistryKey();

private:
	inline bool LoadValue(unsigned long Index, bool ForceReload = false);
	void InitKeyValueList();
	void setKeyName(char * name);
	HKEY m_hKey;
	char * m_KeyName;
	unsigned long m_NumSubKeys;
	unsigned long m_NumValues;
	int m_ErrorCode;

	std::vector< KeyValue * > m_KeyValues;
};

#endif
