#pragma once

class GlobalClassFactory
{
public:	
	IClassFactory* pointFactory;
	IClassFactory* shapeFactory;
	IClassFactory* extentsFactory;

	GlobalClassFactory::GlobalClassFactory()
	{
		pointFactory = shapeFactory = extentsFactory =  NULL;
		CoGetClassObject(CLSID_Point, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void**)&pointFactory); 
		CoGetClassObject(CLSID_Shape, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void**)&shapeFactory);
		CoGetClassObject(CLSID_Extents, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void**)&extentsFactory);
	}

	~GlobalClassFactory()
	{
		if (pointFactory)
			pointFactory->Release(); 
		if (shapeFactory)
			shapeFactory->Release();
		if (extentsFactory)
			extentsFactory->Release();
	}
};