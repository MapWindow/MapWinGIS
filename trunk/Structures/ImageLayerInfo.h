#pragma once
#include "BaseLayerInfo.h"

struct ImageLayerInfo : public BaseLayerInfo
{	
	ImageLayerInfo()
	{	
		ColorScheme = NULL;
	}
	virtual ~ImageLayerInfo()
	{	
		//if( low_res_file != "" )
		//	_unlink(low_res_file);

		if( ColorScheme != NULL)
			ColorScheme->Release();
	}

	//GridColorScheme
	IDispatch * ColorScheme;
	CStringW GridFileName;//filename for a grid

};