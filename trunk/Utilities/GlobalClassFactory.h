#pragma once

class GlobalClassFactory
{
public:	
	IClassFactory* pointFactory;

	GlobalClassFactory::GlobalClassFactory()
	{
		pointFactory = NULL;
		CoGetClassObject(CLSID_Point, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void**)&pointFactory); 
	}

	~GlobalClassFactory()
	{
		if (pointFactory)
			pointFactory->Release(); 
	}
};