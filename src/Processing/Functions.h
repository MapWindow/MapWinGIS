#pragma once
#include "Expression.h"

namespace parser
{
	extern std::vector<CFunction*> functions;
	void InitializeFunctions();
	CFunction* GetFunction(CString name);
};


