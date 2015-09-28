#pragma once
#include "CustomExpression.h"

namespace parser
{
	extern std::vector<CustomFunction*> functions;
	void InitializeFunctions();
	CustomFunction* GetFunction(CString name);
	void ReleaseFunctions();
};


