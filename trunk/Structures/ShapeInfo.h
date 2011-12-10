# ifndef SHAPE_INFO_H
# define SHAPE_INFO_H

struct ShapeInfo
{
	ShapeInfo()
	{	flags = 0;	
		ImageListIndex = -1; // Chris M 3/16/2006
        FontCharListIndex = -1; //ajp 10/01/2008
	}
	ShapeInfo( const ShapeInfo & si )
	{	pointType = si.pointType;
		trans = si.trans;
		fillClr = si.fillClr;
		lineClr = si.lineClr;
		pointClr = si.pointClr;
		lineWidth = si.lineWidth;
		pointSize = si.pointSize;
		lineStipple = si.lineStipple;
		fillStipple = si.fillStipple;
		stippleLineClr = si.stippleLineClr;
		flags = si.flags;
		ImageListIndex = si.ImageListIndex;
		FontCharListIndex = si.FontCharListIndex;
		transparentStipple = si.transparentStipple;
	}
	~ShapeInfo()
	{				
	}

	//Point type
	tkPointType pointType;
	
	//Point Image List Index. Chris M 3/16/2006.
	int ImageListIndex;

	//Point Font Character value ajp 10/01/2008.
	int FontCharListIndex;



	//Transparency
	float trans;
	//Colors
	OLE_COLOR fillClr;
	OLE_COLOR lineClr;
	OLE_COLOR pointClr;
	OLE_COLOR stippleLineClr;
	bool transparentStipple;
	//Size
	float lineWidth;
	float pointSize;		
	//Stipples
	tkLineStipple lineStipple;
	tkFillStipple fillStipple;	
	//Additional
	long flags;
	
	//Net ShapeLayer Info
	float lenAHead;
};

# endif