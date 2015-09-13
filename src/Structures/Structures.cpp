#include "StdAfx.h"
#include "Structures.h"

// ****************************************************************
//		CallbackParams constructor
// ****************************************************************
CallbackParams::CallbackParams(ICallback* localCallback, const char * message) : sMsg(message)
{
	cBack = localCallback;
}

CallbackParams::CallbackParams(const char * message) : sMsg(message)
{
	cBack = m_globalSettings.callback;
}

