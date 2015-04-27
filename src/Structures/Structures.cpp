#include "StdAfx.h"
#include "Structures.h"

// ****************************************************************
//		CallbackParams constructor
// ****************************************************************
CallbackParams::CallbackParams(ICallback* localCallback, const char * message) : sMsg(message)
{
	ICallback* callback = m_globalSettings.callback ? m_globalSettings.callback : localCallback;
	cBack = callback;
}

CallbackParams::CallbackParams(const char * message) : sMsg(message)
{
	cBack = m_globalSettings.callback;
}

