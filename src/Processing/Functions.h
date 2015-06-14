#pragma once
#include "CustomExpression.h"

namespace parser
{
	extern std::vector<Function*> functions;
	void InitializeFunctions();
	Function* GetFunction(CString name);
};


