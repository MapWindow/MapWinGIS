// RegistryKey.cpp: implementation of the RegistryKey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegistryKey.h"


const long BUFFER_SIZE = 500;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RegistryKey::RegistryKey()
{
	m_hKey = NULL;
	m_KeyName = NULL;
	m_ErrorCode =  REG_KEY_NOT_OPEN;
	m_NumSubKeys =  -1;
	m_NumValues =  -1;
}

RegistryKey::~RegistryKey()
{
	Close();
}

bool RegistryKey::OpenKey(HKEY BaseSection, char *KeyName)
{

	Close();

	if (KeyName == NULL)
	{
		m_ErrorCode = REG_INVALID_NULL_PARAM;
		return false;
	}

	m_ErrorCode = RegOpenKey(BaseSection, KeyName, &m_hKey);

	if (m_ErrorCode != REG_NO_ERROR)
		return false;


	m_ErrorCode = RegQueryInfoKey(m_hKey,NULL,NULL,NULL,&m_NumSubKeys,NULL,NULL,&m_NumValues,NULL,NULL,NULL,NULL);

	if (m_ErrorCode != REG_NO_ERROR)
		return false;

	InitKeyValueList();

	setKeyName(KeyName);

	return true;
}

char * RegistryKey::getValueName(unsigned long KeyIndex)
{
	char * buffer = NULL;

	if(isOpen() == false)
	{
		m_ErrorCode = REG_KEY_NOT_OPEN;
		return buffer;
	}

	if (KeyIndex >= m_NumValues)
	{
		m_ErrorCode = REG_INDEX_OUT_OF_BOUNDS;
		return buffer;
	}


	if (LoadValue(KeyIndex,false) == true)
		return m_KeyValues[KeyIndex]->name;
	else
		return NULL;
}

//char * RegistryKey::getErrorMsg()
//{
//	char * msg;
//
//	if (m_ErrorCode == 0)
//		msg = "No Error";
//	else if (m_ErrorCode < 0)
//	{
//		//RegistryKey Class Error
//		msg = new char [BUFFER_SIZE];
//		switch(m_ErrorCode)
//		{
//		case REG_KEY_NOT_OPEN:
//			msg = "Key not open";
//			break;
//		case REG_INVALID_NULL_PARAM:
//			msg = "Invalid Null Parameter";
//			break;
//		case REG_INDEX_OUT_OF_BOUNDS:
//			msg = "Index out of bounds";
//			break;
//		default:
//			msg = "Unknown error code";
//		}
//	}
//	else//system error
//	{
//		DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
//		DWORD LANG_ID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
//		DWORD NumBytes = FormatMessage(flags,NULL,m_ErrorCode, LANG_ID,(LPTSTR) &msg,0,NULL);
//
//	}
//
//	return msg;
//}

bool RegistryKey::isOpen()
{
	if (m_hKey == NULL)
		return false;
	else
		return true;
}

unsigned long RegistryKey::getNumValues()
{
	return m_NumValues;
}

unsigned long RegistryKey::getNumSubKeys()
{
	return m_NumSubKeys;
}

void RegistryKey::Close()
{
	if (isOpen())
	{
		RegCloseKey(m_hKey);
	}

	

	while (m_KeyValues.empty() == false)
	{
		KeyValue * kv = NULL;
		
		kv = m_KeyValues[0];
		m_KeyValues.erase(m_KeyValues.begin());
		if (kv != NULL)
			delete kv;//->~KeyValue();
	}

	m_KeyValues.clear();

	if (m_KeyName != NULL)
		delete [] m_KeyName;

	m_KeyName = NULL;
	m_ErrorCode = REG_KEY_NOT_OPEN;
	m_hKey = NULL;
	m_NumSubKeys = -1;
	m_NumValues = -1;
}

RegistryKey * RegistryKey::getSubKey(unsigned long Index)
{
	RegistryKey * retval = new RegistryKey;

	if (!isOpen())
		return NULL;

	if (Index >= m_NumSubKeys)
	{
		m_ErrorCode = REG_INDEX_OUT_OF_BOUNDS;
		return NULL;
	}

	//get the name of the Requested key specified
	char * KeyName;

	KeyName = new char[BUFFER_SIZE];
	RegEnumKey(m_hKey,Index,KeyName,BUFFER_SIZE);
	
	if (retval->OpenKey(m_hKey,KeyName) == false)
		return NULL;

	// Chris M 1/27/2006
	delete [] KeyName;
	
	return retval;

}

void RegistryKey::setKeyName(char *name)
{
	if(m_KeyName != NULL)
		delete [] m_KeyName;

	m_KeyName = new char [_tcslen(name) + 1];

	strcpy(m_KeyName,name);
}

BYTE * RegistryKey::getValueData(unsigned long ValueIndex)
{
	if(isOpen() == false)
	{
		m_ErrorCode = REG_KEY_NOT_OPEN;
		return NULL;
	}

	if (ValueIndex >= m_NumValues)
	{
		m_ErrorCode = REG_INDEX_OUT_OF_BOUNDS;
		return NULL;
	}


	if (LoadValue(ValueIndex,false) == true)
	{	
		KeyValue * kv = NULL;
		kv = m_KeyValues[ValueIndex];
		unsigned long bufferSize = kv->dataSize;
		LPBYTE buffer = new BYTE[bufferSize];

		unsigned long NameLength = _tcslen(kv->name) +1;

		m_ErrorCode = RegEnumValue(m_hKey,ValueIndex,kv->name,&NameLength,NULL,&(kv->type),buffer,&bufferSize);

		
		if (m_ErrorCode != REG_NO_ERROR)
		{
			cout << "DataType: " << REG_SZ << endl;
			cout.flush();
			delete [] buffer;
			buffer = NULL;
			return NULL;
		}

		kv->dataSize = bufferSize;

		return buffer;
	}
	else
		return NULL;
}

char * RegistryKey::getKeyName()
{
	char * retval = NULL;
	if (m_KeyName != NULL)
	{
		retval = new char[_tcslen(m_KeyName)+1];
		strcpy(retval,m_KeyName);
	}
	return retval;
}

DWORD RegistryKey::getValueType(unsigned long Index)
{
	if (isOpen() == false)
	{
		m_ErrorCode = REG_KEY_NOT_OPEN;
		return REG_NONE;
	}

	
	if (LoadValue(Index,false) == true)
		return m_KeyValues[Index]->type;
	else
		return NULL;
	
}

void RegistryKey::InitKeyValueList()
{
	for (int i = 0; i < (int)m_NumValues; i++)
		m_KeyValues.push_back(NULL);
}

bool RegistryKey::LoadValue(unsigned long Index, bool ForceReload)
{
	
	char * NameBuffer;
	unsigned long NameLength = BUFFER_SIZE;

	if (m_KeyValues[Index] == NULL || ForceReload == true)
	{
		
		if (m_KeyValues[Index] != NULL)
			delete m_KeyValues[Index];

		m_KeyValues[Index] = NULL;

		NameBuffer = new char [NameLength];
		
		KeyValue * kv = new KeyValue();
		
		//get the information, but don't get the data until requested
		m_ErrorCode = RegEnumValue(m_hKey,Index,NameBuffer,&NameLength, NULL,&(kv->type),NULL,&(kv->dataSize));
		
		
		if (m_ErrorCode != REG_NO_ERROR)
		{
			delete [] NameBuffer;
			delete kv;
			return false;
		}
		
		kv->name = new char[NameLength+1];
		
		strcpy(kv->name,NameBuffer);
		delete [] NameBuffer;
		
		m_KeyValues[Index] = kv;
		kv = NULL; // Do not delete -- it's assigned to a member variable
		kv = m_KeyValues[Index];
	}
	return true;
}
