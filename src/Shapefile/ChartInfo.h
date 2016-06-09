#pragma once

struct CChartInfo
{	
public:
	CChartInfo()
	{
		visible	= true;
		x = y = 0.0;
		frame = NULL;
		isDrawn = VARIANT_FALSE;
	}
	~CChartInfo()
	{
		// don't forget to set pointer to NULL after the use of frame
		// or it will be the problems here
		if (frame) 
		{
			delete frame;
			frame = NULL;
		}
	}
	
	VARIANT_BOOL isDrawn;
	CRect* frame;		// and are deleted on the next redraw
	bool visible;
	double x;
	double y;
};

