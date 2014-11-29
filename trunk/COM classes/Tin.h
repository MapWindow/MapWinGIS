/**************************************************************************************
 * File name: Tin.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: declaration of CTin
 *
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/ 
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */

#pragma once
# include "TinHeap.h"
# include "heap_node.h"
# include "point_table.h"
# include "table_row.h"
# include "table_row_node.h"
# include "tintypes.h"
# include "triangle_table.h"
# include "varH.h"
# include "vertex.h"
# include "vertex_table.h"
# include <math.h>
# include <stdlib.h>
# include <string>

// CTin
enum PerpSplitMethod 
{ 
	TinEdge, 
	SubsetEdge 
};

//structures
typedef struct {
   int p1,p2,p3;
   int b1,b2,b3;
} ITRIANGLE;

typedef struct {
   int p1,p2;
} IEDGE;

typedef struct {
   double x,y,z;
} XYZ;

class ATL_NO_VTABLE CTin : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTin, &CLSID_Tin>,
	public IDispatchImpl<ITin, &IID_ITin, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CTin();
	~CTin();

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TIN)

DECLARE_NOT_AGGREGATABLE(CTin)

BEGIN_COM_MAP(CTin)
	COM_INTERFACE_ENTRY(ITin)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


// ITin
public:
	STDMETHOD(TriangleNeighbors)(/*[in]*/ long TriIndex, /*[in, out]*/ long * triIndex1, /*[in, out]*/ long * triIndex2, /*[in, out]*/ long * triIndex3);
	STDMETHOD(get_IsNDTriangle)(/*[in]*/ long TriIndex, /*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_Filename)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(Min)(/*[out]*/ double * X, /*[out]*/ double * Y, /*[out]*/ double * Z);
	STDMETHOD(Max)(/*[out]*/ double * X, /*[out]*/ double * Y, /*[out]*/ double * Z);
	STDMETHOD(Vertex)(/*[in]*/ long VtxIndex, /*[out]*/ double * X, /*[out]*/ double * Y, /*[out]*/ double * Z);
	STDMETHOD(Triangle)(/*[in]*/ long TriIndex, /*[out]*/ long * vtx1Index, /*[out]*/ long * vtx2Index, /*[out]*/ long * vtx3Index);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_CdlgFilter)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_NumVertices)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_NumTriangles)(/*[out, retval]*/ long *pVal);
	STDMETHOD(Select)(/*[in, out]*/ long * TriangleHint, /*[in]*/ double X, /*[in]*/ double Y, /*[out]*/ double * Z, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(Close)(/*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(Save)(/*[in]*/ BSTR TinFilename, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(CreateNew)(/*[in]*/ IGrid * Grid, /*[in]*/ double Deviation, /*[in]*/ SplitMethod SplitTest, /*[in]*/ double STParam, /*[in]*/ long MeshDivisions, /*[in, optional, defaultvalue(1073741824)]*/ long MaximumTriangles, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(Open)(/*[in]*/ BSTR TinFile, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(CreateTinFromPoints)(SAFEARRAY * Points, VARIANT_BOOL* retval);

private:
	ICallback * _globalCallback;
	long _lastErrorCode;
	BSTR _key;
	BSTR _filename;
	
	triangleTable _triTable;
	vertexTable _vtxTable;
	vertex _min;
	vertex _max;

	TinHeap _devHeap;
	SplitMethod _splitMethod;
	double _splitParam;
	IGrid * _grid;
	IGridHeader * _gridHeader;
	char * _dTriangles;

private:
	inline long gridNcols();
	inline long gridNrows();
	inline void createTin( double deviation, long meshDivisions, long maxTriangles );
	inline void computeTriangleEquation( long triIndex, double & A, double & B, double & C, double & D );
	inline bool vtxInTriangle( vertex * triangle, vertex testVertex );
	inline bool doesCross( int SH, int NSH, vertex & corner_one, vertex & corner_two );
	inline void setSign( double val, int & SH );
	inline vertex gridCoord( long column, long row );
	inline double gridValue( long column, long row );
	inline double gridMin();
	inline double gridMax();
	void createMesh( long numDivisions, vertex v1, vertex v2, vertex v3, vertex v4 );
	inline void splitTriangle( long triIndex );
	inline void gridRaster( double x, double y, long & column, long & row );
	inline void computeMaxDev( long triIndex );
	inline bool splitPerp( long triIndex, long perpPointIndex, PerpSplitMethod perpSplitMethod );
	inline bool splitQuad( long triOneIndex, long triTwoIndex );
	inline void changeBorder( long triIndex, long oldValue, long newValue );
	/*inline double MIN( double one, double two );
	inline double MAX( double one, double two );*/
	bool willCreateBadTriangles( vertex * triangle, vertex testVertex );
	inline long indexAfterClockwise( long triIndex, long vtxIndex );
	inline void setBorders( long triIndex, long possBorder1, long possBorder2, long possBorder3,
							long possBorder4, long possBorder5, long possBorder6, long possBorder7 );
	inline long unsharedIndex( long triOne, long triTwo );
	inline void sharedIndexes( long triOne, long triTwo, long & index1, long & index2 );
	bool canSplitQuad( long p1_unshared, long p2_unshared, long p1_shared, long p2_shared );
	inline double inscribedCircleRad( vertex one, vertex two, vertex three );
	inline bool hasVertices( long triIndex, long vtxOne, long vtxTwo );

	void Prune(int &nv,XYZ v[]);
	void BuildTin(int nv, XYZ v[],int ntri,ITRIANGLE *tri,XYZ vMax,XYZ vMin);
	void FindAllBorders(int ntri,ITRIANGLE *tri);
	int FindBorder(int ntri,int curIndex,ITRIANGLE *tri,int v1,int v2);
	void FindMaxXYZAndMinXYZ(int nv,XYZ v[],XYZ &vMax,XYZ &vMin);
	int CreateTin(int nv,XYZ *pxyz);
	int Triangulate(int nv,XYZ *pxyz,ITRIANGLE *v,int *ntri);
	int CircumCircle(double xp,double yp,
		double x1,double y1,double x2,double y2,double x3,double y3,
		double *xc,double *yc,double *r);
};

OBJECT_ENTRY_AUTO(__uuidof(Tin), CTin)
