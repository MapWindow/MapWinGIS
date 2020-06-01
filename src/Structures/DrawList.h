# ifndef DRAW_LIST_H
# define DRAW_LIST_H

//Stan 30.09.2006
# include "label.h"

struct _DrawPoint
{	double x;
	double y;
	long size;
	OLE_COLOR color;
	byte alpha = 255;
};

struct _DrawLine
{	double x1;
	double y1;
	double x2;
	double y2;
	long width;
	OLE_COLOR color;
	byte alpha = 255;
};

struct _DrawCircle
{	double x;
	double y;
	double radius;
	int width;
	OLE_COLOR color;
	byte alpha = 255;
	bool fill;
};

struct _DrawPolygon
{	
	_DrawPolygon()
	{	xpnts=NULL;
		ypnts=NULL;
	}
	~_DrawPolygon()
	{	if( xpnts )
			delete [] xpnts;
		xpnts = NULL;
		if( ypnts )
			delete [] ypnts;
		ypnts = NULL;
	}
    
	double * xpnts;
	double * ypnts;
	long numPoints;
	OLE_COLOR color;
	byte alpha = 255;
	bool fill;
	int width;
};

class DrawList //: public LabelLayer
{	
public:
	DrawList()
	{	
		key = SysAllocString(L"");
		m_labels = NULL;
		CoCreateInstance(CLSID_Labels,NULL,CLSCTX_INPROC_SERVER,IID_ILabels,(void**)&m_labels);
	}
	~DrawList()
	{	::SysFreeString(key);
		register int i;
		int endcondition = m_dpoints.size();
		for(i=0;i<endcondition;i++)
			delete m_dpoints[i];

		endcondition = m_dlines.size();
		for(i=0;i<endcondition;i++)
			delete m_dlines[i];

		endcondition = m_dcircles.size();
		for(i=0;i<endcondition;i++)
			delete m_dcircles[i];

		endcondition = m_dpolygons.size();
		for(i=0;i<endcondition;i++)
			delete m_dpolygons[i];

		m_dpoints.clear();
		m_dlines.clear();
		m_dcircles.clear();
		m_dpolygons.clear();
		
		if (m_labels != NULL)
			m_labels->Release();
	}
	
	tkDrawReferenceList listType;
	BSTR key;

	std::vector<_DrawPoint *> m_dpoints;
	std::vector<_DrawLine *> m_dlines;
	std::vector<_DrawCircle *> m_dcircles;
	std::vector<_DrawPolygon *> m_dpolygons;
	
	// lsu: better to implement it in new COM class for drawings, but for now I'll just add it here
	ILabels* m_labels;
};

# endif
