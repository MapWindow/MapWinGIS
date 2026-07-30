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
	m_hKey = nullptr;
	m_KeyName = nullptr;
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

	if (KeyName == nullptr)
	{
		m_ErrorCode = REG_INVALID_NULL_PARAM;
		return false;
	}

	m_ErrorCode = RegOpenKey(BaseSection, KeyName, &m_hKey);

	if (m_ErrorCode != REG_NO_ERROR)
		return false;

	m_ErrorCode = RegQueryInfoKey(m_hKey, nullptr, nullptr, nullptr,&m_NumSubKeys, nullptr, nullptr,&m_NumValues, nullptr, nullptr, nullptr, nullptr);

	if (m_ErrorCode != REG_NO_ERROR)
		return false;

	InitKeyValueList();

	setKeyName(KeyName);

	return true;
}

char * RegistryKey::getValueName(unsigned long KeyIndex)
{
	char * buffer = nullptr;

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
		return nullptr;
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
	if (m_hKey == nullptr)
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
		KeyValue * kv = nullptr;
		
		kv = m_KeyValues[0];
		m_KeyValues.erase(m_KeyValues.begin());
		if (kv != nullptr)
			delete kv;//->~KeyValue();
	}

	m_KeyValues.clear();

	if (m_KeyName != nullptr)
		delete [] m_KeyName;

	m_KeyName = nullptr;
	m_ErrorCode = REG_KEY_NOT_OPEN;
	m_hKey = nullptr;
	m_NumSubKeys = -1;
	m_NumValues = -1;
}

RegistryKey * RegistryKey::getSubKey(unsigned long Index)
{
	RegistryKey * retval = new RegistryKey;

	if (!isOpen())
		return nullptr;

	if (Index >= m_NumSubKeys)
	{
		m_ErrorCode = REG_INDEX_OUT_OF_BOUNDS;
		return nullptr;
	}

	//get the name of the Requested key specified
	char * KeyName;

	KeyName = new char[BUFFER_SIZE];
	RegEnumKey(m_hKey,Index,KeyName,BUFFER_SIZE);
	
	if (retval->OpenKey(m_hKey,KeyName) == false)
		return nullptr;
	
	return retval;
}

void RegistryKey::setKeyName(char *name)
{
	if(m_KeyName != nullptr)
		delete [] m_KeyName;

	m_KeyName = new char [strlen(name) + 1];

	strcpy(m_KeyName,name);
}

BYTE * RegistryKey::getValueData(unsigned long ValueIndex)
{
	if(isOpen() == false)
	{
		m_ErrorCode = REG_KEY_NOT_OPEN;
		return nullptr;
	}

	if (ValueIndex >= m_NumValues)
	{
		m_ErrorCode = REG_INDEX_OUT_OF_BOUNDS;
		return nullptr;
	}


	if (LoadValue(ValueIndex,false) == true)
	{
		KeyValue * kv = nullptr;
		kv = m_KeyValues[ValueIndex];
		unsigned long bufferSize = kv->dataSize;
		LPBYTE buffer = new BYTE[bufferSize];

		unsigned long NameLength = static_cast<unsigned long>(strlen(kv->name) +1);

		m_ErrorCode = RegEnumValue(m_hKey,ValueIndex,kv->name,&NameLength, nullptr,&(kv->type),buffer,&bufferSize);


		if (m_ErrorCode != REG_NO_ERROR)
		{
			cout << "DataType: " << REG_SZ << endl;
			cout.flush();
			delete [] buffer;
			buffer = nullptr;
			return nullptr;
		}

		kv->dataSize = bufferSize;

		return buffer;
	}
	else
		return nullptr;
}

char * RegistryKey::getKeyName()
{
	char * retval = nullptr;
	if (m_KeyName != nullptr)
	{
		retval = new char[strlen(m_KeyName)+1];
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
		return 0;
}

void RegistryKey::InitKeyValueList()
{
	for (int i = 0; i < static_cast<int>(m_NumValues); i++)
		m_KeyValues.push_back(nullptr);
}

bool RegistryKey::LoadValue(unsigned long Index, bool ForceReload)
{
	char * NameBuffer;
	unsigned long NameLength = BUFFER_SIZE;

	if (m_KeyValues[Index] == nullptr || ForceReload == true)
	{
		
		if (m_KeyValues[Index] != nullptr)
			delete m_KeyValues[Index];

		m_KeyValues[Index] = nullptr;

		NameBuffer = new char [NameLength];
		
		KeyValue * kv = new KeyValue();
		
		//get the information, but don't get the data until requested
		m_ErrorCode = RegEnumValue(m_hKey,Index,NameBuffer,&NameLength, nullptr,&(kv->type), nullptr,&(kv->dataSize));

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
		kv = nullptr;
		kv = m_KeyValues[Index];
	}
	return true;
}
