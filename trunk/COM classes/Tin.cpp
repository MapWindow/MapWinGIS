/**************************************************************************************
 * File name: Tin.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of CTin
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

#include "stdafx.h"
#include "Tin.h"
#include "Vector.h"
#include "stdio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CTin
CTin::CTin()
{	_globalCallback = NULL;
	_lastErrorCode = tkNO_ERROR;
	USES_CONVERSION;
	_key = SysAllocString(L"");
	_filename = SysAllocString(L"");
	_dTriangles = NULL;
}

CTin::~CTin()
{	VARIANT_BOOL retval;
	Close(&retval);
	_globalCallback = NULL;
	::SysFreeString(_key);
	::SysFreeString(_filename);
}

STDMETHODIMP CTin::Open(BSTR TinFile, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	ICallback* callback = cBack ? cBack : _globalCallback;

	Close(retval);
	CString tinFile = OLE2CA(TinFile);

	_triTable.clear();
	_vtxTable.clear();

	FILE * in = fopen( tinFile, "rb" );

	if( !in )
	{	*retval = FALSE;		
	}
	else
	{	_filename = A2BSTR(tinFile);
	
		//Bounding Box
		double mvalx, mvaly, mvalz;
		fread( &mvalx, sizeof(double), 1, in );
		fread( &mvaly, sizeof(double), 1, in );
		fread( &mvalz, sizeof(double), 1, in );
		_min = vertex( mvalx, mvaly, mvalz );
		fread( &mvalx, sizeof(double), 1, in );
		fread( &mvaly, sizeof(double), 1, in );
		fread( &mvalz, sizeof(double), 1, in );
		_max = vertex( mvalx, mvaly, mvalz );
		
		//Number of Triangles
		long numTriangles;
		fread( &numTriangles, sizeof(long), 1, in );

		//Number of Vertices
		long numVertices;
		fread( &numVertices, sizeof(long), 1, in );

		long vtx1, vtx2, vtx3;
		long b1, b2, b3;
		
		double total = numTriangles + numVertices;
		long percent = 0;
		int newpercent = 0;

		//Triangles
		long t = 0;
		for(t = 0; t < numTriangles; t++ )
		{		
			fread( &vtx1, sizeof(long), 1, in );
			fread( &vtx2, sizeof(long), 1, in );
			fread( &vtx3, sizeof(long), 1, in );
			fread( &b1, sizeof(long), 1, in );
			fread( &b2, sizeof(long), 1, in );
			fread( &b3, sizeof(long), 1, in );

			tinTableRow r( vtx1, vtx2, vtx3, b1, b2, b3 );

			_triTable.addRow( r );

			CallbackHelper::Progress(callback, t, total, "Reading Tin", _key, percent);
		}
		double x, y, z;
		vertex ver;
		long v;
		for( v = 0; v < numVertices; v++ )
		{	fread( &x, sizeof(double), 1, in );
			fread( &y, sizeof(double), 1, in );
			fread( &z, sizeof(double), 1, in );
			ver = vertex( x, y, z );
			_vtxTable.add( ver );
			
			CallbackHelper::Progress(callback, t + v, total, "Reading Tin", _key, percent);
		}

		if( numTriangles > 0 )
		{	_dTriangles = new char[numTriangles];
			if( feof(in) )
				memset(_dTriangles,0,sizeof(char)*numTriangles);
			else
				fread(_dTriangles,sizeof(char),numTriangles, in);
		}

		fclose( in );
		*retval = TRUE;
	}

	return S_OK;
}

STDMETHODIMP CTin::CreateNew(IGrid *Grid, double Deviation, SplitMethod SplitTest, double STParam, long MeshDivisions, long MaximumTriangles, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( Grid == NULL )
	{	*retval = FALSE;
	}
	else
	{	Close(retval);
		_grid = Grid;
		_grid->get_Header(&_gridHeader);
		_devHeap = TinHeap( Deviation );
		_triTable.clear();
		_vtxTable.clear();

		_splitMethod = SplitTest;
		_splitParam = STParam;

		ICallback * tmpcallback = _globalCallback;
		if( cBack != NULL )
			_globalCallback = cBack;

		createTin( Deviation, MeshDivisions, MaximumTriangles );

		//Mark ndv triangles
		VARIANT vndv;
		VariantInit(&vndv); //added by Rob Cairns 4-Jan-06
		double ndv;
		_gridHeader->get_NodataValue(&vndv);
		dVal(vndv,ndv);
		if( _triTable.size() > 0 )
			_dTriangles=new char[_triTable.size()];

		for( long i = 0; i < _triTable.size(); i++ )
		{	long index_one = *((long*)_triTable.getValue( VTX_ONE, i ) );
			long index_two = *((long*)_triTable.getValue( VTX_TWO, i ) );
			long index_three = *((long*)_triTable.getValue( VTX_THREE, i ) );
		
			//Swap Polygon Order
			_triTable.setValue( VTX_THREE, i, (void*)&index_one);
			_triTable.setValue( VTX_TWO, i, (void*)&index_two);
			_triTable.setValue( VTX_ONE, i, (void*)&index_three);			

			//Mark ndv Triangles
			vertex v1 = _vtxTable.getVertex( index_one );			
			vertex v2 = _vtxTable.getVertex( index_two );
			vertex v3 = _vtxTable.getVertex( index_three );
			if( v1.getZ() == ndv || v2.getZ() == ndv || v3.getZ() == ndv )
				_dTriangles[i]=1;
			else
				_dTriangles[i]=0;
		}
		_gridHeader->Release();
		_gridHeader = NULL;
		_grid = NULL;
		*retval = TRUE;
		VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
		_globalCallback = tmpcallback;
	}
	return S_OK;
}

STDMETHODIMP CTin::Save(BSTR TinFilename, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	ICallback* callback = cBack ? cBack : _globalCallback;

	FILE * out = fopen( OLE2CA(TinFilename), "wb");
	if( !out )
	{	*retval = FALSE;
	}
	else
	{	_filename = OLE2BSTR(TinFilename);
		//Bounding Box	
		double mval = _min.getX();
		fwrite(&mval, sizeof(double),1, out );
		mval = _min.getY();
		fwrite(&mval, sizeof(double),1, out );
		mval = _min.getZ();
		fwrite(&mval, sizeof(double),1, out );
		mval = _max.getX();
		fwrite(&mval, sizeof(double),1, out );
		mval = _max.getY();
		fwrite(&mval, sizeof(double),1, out );
		mval = _max.getZ();
		fwrite(&mval, sizeof(double),1, out );

		//Number of Triangles
		long numTriangles = _triTable.size();
		fwrite(&numTriangles, sizeof(long), 1, out );
		long numVertices = _vtxTable.size();
		fwrite(&numVertices, sizeof(long), 1, out );
		
		double total = numVertices + numTriangles;
		long percent = 0;
		int newpercent = 0;

		long i = 0;
		//Triangles
		for(i = 0; i < numTriangles; i++ )
		{	
			long index_one = *((long*)_triTable.getValue( VTX_ONE, i ) );
			long index_two = *((long*)_triTable.getValue( VTX_TWO, i ) );
			long index_three = *((long*)_triTable.getValue( VTX_THREE, i ) );
		
			long border_one = *((long*)_triTable.getValue( BDR_ONE, i ) );
			long border_two = *((long*)_triTable.getValue( BDR_TWO, i ) );
			long border_three = *((long*)_triTable.getValue( BDR_THREE, i ) );
			
			fwrite(&index_one, sizeof(long), 1, out );	
			fwrite(&index_two, sizeof(long), 1, out );
			fwrite(&index_three, sizeof(long), 1, out );					
			
			fwrite(&border_one, sizeof(long), 1, out );
			fwrite(&border_two, sizeof(long), 1, out );
			fwrite(&border_three, sizeof(long), 1, out );	
			
			CallbackHelper::Progress(callback, i, total, "Saving Tin", _key, percent);
		}

		long j = 0;
		for(j = 0; j < numVertices; j++ )
		{
			vertex v1 = _vtxTable.getVertex( j );			
		
			double v1_x = v1.getX();
			double v1_y = v1.getY();
			double v1_z = v1.getZ();
			fwrite(&v1_x, sizeof(double), 1, out );
			fwrite(&v1_y, sizeof(double), 1, out );
			fwrite(&v1_z, sizeof(double), 1, out );

			/*
			vector vNorm = v1.getNormal();
			v1_x = vNorm.geti();
			v1_y = vNorm.getj();
			v1_z = vNorm.getk();
			fwrite(&v1_x, sizeof(double), 1, out );
			fwrite(&v1_y, sizeof(double), 1, out );
			fwrite(&v1_z, sizeof(double), 1, out );
			*/

			CallbackHelper::Progress(callback, i + j, total, "Saving Tin", _key, percent);
		}

		if( numTriangles > 0 )
			fwrite(_dTriangles,sizeof(char),numTriangles,out);
		
		fclose( out );
		*retval = TRUE;
	}

	return S_OK;
}

STDMETHODIMP CTin::Close(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	::SysFreeString(_filename);
	_filename = A2BSTR("");

	if( _dTriangles != NULL )
		delete [] _dTriangles;
	_dTriangles = NULL;
	_triTable.clear();
	_vtxTable.clear();	
	*retval = TRUE;

	return S_OK;
}

STDMETHODIMP CTin::Select(long * TriangleHint, double X, double Y, double *Z, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//Make an initial test to see if the point is even in the TIN bounds
	if( X < _min.getX() || X > _max.getX() )
		*retval = FALSE;
	else if( Y < _min.getY() || Y > _max.getY() )
		*retval = FALSE;
	else
	{	
		std::deque<long> breadthTraverse;
		breadthTraverse.push_back( *TriangleHint );

		bool * checkedTriangle = new bool[ _triTable.size() + 1];
		memset( checkedTriangle, 0, _triTable.size() + 1 );

		vertex triangle[3];
		long findHeightBorder1;
		long findHeightBorder2;
		long findHeightBorder3;
		long findHeightTriIndex;
		vertex orthoVertex( X, Y, 0.0 );

		bool found = false;
		long triIndex = Undefined;	

		while( !found )
		{		if( breadthTraverse.size() <= 0 )
				{	triIndex = Undefined;
					delete [] checkedTriangle;
					*retval = FALSE;
					return S_OK;
				}

				findHeightTriIndex = breadthTraverse[0];
				if( findHeightTriIndex != -1 && !checkedTriangle[findHeightTriIndex])
				{
					checkedTriangle[ findHeightTriIndex ] = true;

					triangle[0] = _vtxTable.getVertex( *((long*)_triTable.getValue( VTX_ONE, findHeightTriIndex ) ) );
					triangle[1] = _vtxTable.getVertex( *((long*)_triTable.getValue( VTX_TWO, findHeightTriIndex ) ) );;
					triangle[2] = _vtxTable.getVertex( *((long*)_triTable.getValue( VTX_THREE, findHeightTriIndex ) ) );;

					if( vtxInTriangle( triangle, orthoVertex ) )
					{	//Vertex is in triangle
						//	Compute the vertex from the plane equation
						//	in the form D = Ax + By + Cz
						double a, b, c, d;
						computeTriangleEquation( findHeightTriIndex, a, b, c, d );
						double z = ( d - a*orthoVertex.getX() - b*orthoVertex.getY() )/c;
						orthoVertex.setZ( z );
						found = true;
						triIndex = findHeightTriIndex;
					}
					else if( fabs( triangle[0].getX() - orthoVertex.getX() ) < CompareEpsilom &&
							 fabs( triangle[0].getY() - orthoVertex.getY() ) < CompareEpsilom )
					{	found = true;
						orthoVertex = triangle[0];
						triIndex = findHeightTriIndex;
					}
					else if( fabs( triangle[1].getX() - orthoVertex.getX() ) < CompareEpsilom &&
							 fabs( triangle[1].getY() - orthoVertex.getY() ) < CompareEpsilom )
					{	found = true;
						orthoVertex = triangle[1];
						triIndex = findHeightTriIndex;
					}
					else if( fabs( triangle[2].getX() - orthoVertex.getX() ) < CompareEpsilom &&
							 fabs( triangle[2].getY() - orthoVertex.getY() ) < CompareEpsilom )
					{	found = true;
						orthoVertex = triangle[2];
						triIndex = findHeightTriIndex;
					}
					else
					{	//Breadth search tin
						breadthTraverse.pop_front();
						findHeightBorder1 = *((long*)_triTable.getValue( BDR_ONE, findHeightTriIndex ) );
						findHeightBorder2 = *((long*)_triTable.getValue( BDR_TWO, findHeightTriIndex ) );
						findHeightBorder3 = *((long*)_triTable.getValue( BDR_THREE, findHeightTriIndex ) );

						breadthTraverse.push_back( findHeightBorder1 );
						breadthTraverse.push_back( findHeightBorder2 );
						breadthTraverse.push_back( findHeightBorder3 );						
					}
				}
				else
					breadthTraverse.pop_front();							
		}
		delete [] checkedTriangle;
		*retval = TRUE;
		*TriangleHint = triIndex;
		*Z = orthoVertex.getZ();
	}
	return S_OK;
}

STDMETHODIMP CTin::get_NumTriangles(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _triTable.size();

	return S_OK;
}

STDMETHODIMP CTin::get_NumVertices(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _vtxTable.size();

	return S_OK;
}

STDMETHODIMP CTin::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;

	return S_OK;
}

STDMETHODIMP CTin::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

STDMETHODIMP CTin::get_CdlgFilter(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = A2BSTR("USU TIN Binary (*.tin)|*.tin");

	return S_OK;
}

STDMETHODIMP CTin::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _globalCallback;
	if( _globalCallback != NULL )
		_globalCallback->AddRef();

	return S_OK;
}

STDMETHODIMP CTin::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

STDMETHODIMP CTin::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);

	return S_OK;
}

STDMETHODIMP CTin::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);

	return S_OK;
}

STDMETHODIMP CTin::Triangle(long TriIndex, long *vtx1Index, long *vtx2Index, long *vtx3Index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( TriIndex >= 0 && TriIndex < _triTable.size() )
	{	*vtx1Index = *((long*)_triTable.getValue( VTX_ONE, TriIndex ) );
		*vtx2Index = *((long*)_triTable.getValue( VTX_TWO, TriIndex ) );
		*vtx3Index = *((long*)_triTable.getValue( VTX_THREE, TriIndex ) );
	}
	else
	{	*vtx1Index = -1;
		*vtx2Index = -1;
		*vtx3Index = -1;
	}

	return S_OK;
}

STDMETHODIMP CTin::TriangleNeighbors(long TriIndex, long *triIndex1, long *triIndex2, long *triIndex3)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( TriIndex >= 0 && TriIndex < _triTable.size() )
	{	*triIndex1 = *((long*)_triTable.getValue( BDR_ONE, TriIndex ) );
		*triIndex2 = *((long*)_triTable.getValue( BDR_TWO, TriIndex ) );
		*triIndex3 = *((long*)_triTable.getValue( BDR_THREE, TriIndex ) );
	}
	else
	{	*triIndex1 = -1;
		*triIndex2 = -1;
		*triIndex3 = -1;
	}

	return S_OK;
}


STDMETHODIMP CTin::Vertex(long VtxIndex, double *X, double *Y, double *Z)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( VtxIndex >= 0 && VtxIndex < _vtxTable.size() )
	{	vertex v = _vtxTable.getVertex( VtxIndex );
		*X = v.getX();
		*Y = v.getY();
		*Z = v.getZ();
	}
	else
	{	*X = 0;
		*Y = 0;
		*Z = 0;
	}

	return S_OK;
}

STDMETHODIMP CTin::Max(double *X, double *Y, double *Z)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*X = _max.getX();
	*Y = _max.getY();
	*Z = _max.getZ();

	return S_OK;
}

STDMETHODIMP CTin::Min(double *X, double *Y, double *Z)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*X = _min.getX();
	*Y = _min.getY();
	*Z = _min.getZ();

	return S_OK;
}

STDMETHODIMP CTin::get_Filename(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(_filename);

	return S_OK;
}

inline long CTin::gridNcols()
{	long cols = 0;
	_gridHeader->get_NumberCols(&cols);
	return cols;
}
inline long CTin::gridNrows()
{	long rows = 0;
	_gridHeader->get_NumberRows(&rows);
	return rows;
}

inline void CTin::createTin( double deviation, long meshDivisions, long maxTriangles )
{
	//Make an initial mesh
	int ncols = gridNcols();
	int nrows = gridNrows();
	vertex v1 = gridCoord( 0, 0 );
	vertex v2 = gridCoord( ncols-1, 0 );
	vertex v3 = gridCoord( ncols-1, nrows-1 );
	vertex v4 = gridCoord( 0, nrows-1 );	

	_min = gridCoord( 0, nrows-1 );
	_min.setZ( gridMin() );
	_max = gridCoord( ncols-1, 0 );
	_max.setZ( gridMax() );

	createMesh( meshDivisions, v1, v2, v3, v4 );

	//Split the triangles until a deviation or numTriangles
	heapNode triNode = _devHeap.top();

	int percent = 0, newpercent1, newpercent2 = 0;
	double total = triNode.value;		
	
	while( triNode.value > deviation && _triTable.size() < maxTriangles )
	{
		//The values are only pushed onto the heap, and not deleted
		//	Check the stored deviation vertex against the triangles deviation vertex
		vertex triVertex = *((vertex*)_triTable.getValue( DEV_VERTEX, triNode.index ) );
		if( triNode.devVertex == triVertex )
		{			
			_devHeap.pop();
			splitTriangle( triNode.index );	
			newpercent1 = 100 - (int)(((triNode.value - deviation)/total)*100);
			if( maxTriangles == MaxTriangles )
			{	if( newpercent1 > percent )
				{	
					percent = newpercent1;
					CallbackHelper::Progress(_globalCallback, percent, "Splitting Triangles", _key);
				}				
			}
			else
			{	newpercent2 = (int)(((double)_triTable.size()/maxTriangles)*100);
				if( newpercent2 > newpercent1 )
					newpercent1 = newpercent2;

				if( newpercent1 > percent )
				{	
					percent = newpercent1;
					CallbackHelper::Progress(_globalCallback, percent, "Splitting Triangles", _key);
				}
			}	
			triNode = _devHeap.top();
		}
		else
		{	_devHeap.pop();
			triNode = _devHeap.top();
		}
	}
}

inline bool CTin::vtxInTriangle( vertex * triangle, vertex testVertex )
{
	int NC = 0;		//number_crossings

	//X = U;
	//Y = V;
	//Always drop Z Coordinate and Project on XY Plane

	//Translate points to origin centered on test_point
	vertex corner_one = triangle[0] - testVertex;
	vertex corner_two = triangle[1] - testVertex;
	vertex corner_three = triangle[2] - testVertex;
	testVertex = testVertex - testVertex;

	int SH = 0;
	int NSH = 0;

	setSign( corner_one.getY(), SH );
	setSign( corner_two.getY(), NSH );
	
	if( doesCross( SH, NSH, corner_one, corner_two ) )
		NC++;	
	SH = NSH;
	setSign( corner_three.getY(), NSH );
	if( doesCross( SH, NSH, corner_two, corner_three ) )
		NC++;
	SH = NSH;
	setSign( corner_one.getY(), NSH );
	if( doesCross( SH, NSH, corner_three, corner_one ) )
		NC++;

	//ODD Crossings = point_in_polygon
	if( NC % 2 != 0 )
		return true;
	else
		return false;
}

inline bool CTin::doesCross( int SH, int NSH, vertex & corner_one, vertex & corner_two )
{	if( SH != NSH )
	{	if( corner_one.getX() > 0 && corner_two.getX() > 0 )
			return true;
		else if( corner_one.getX() > 0 || corner_two.getX() > 0 )
		{	//b = v - u*m
			double m = ( corner_two.getX() - corner_one.getX() )/					
					   ( corner_two.getY() - corner_one.getY() );

			if( ( corner_one.getX() - corner_one.getY()*m + CompareEpsilom ) > 0 )
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

inline void CTin::setSign( double val, int & SH )
{	if( val < 0 )
		SH = -1;
	else
		SH = 1;
}

inline void CTin::computeTriangleEquation( long triIndex, double & A, double & B, double & C, double & D )
{	
	//plane equation in the form D = Ax + By + Cz
	vertex v1 = _vtxTable.getVertex( *((long*)_triTable.getValue( VTX_ONE, triIndex ) ) );
	vertex v2 = _vtxTable.getVertex( *((long*)_triTable.getValue( VTX_TWO, triIndex ) ) );
	vertex v3 = _vtxTable.getVertex( *((long*)_triTable.getValue( VTX_THREE, triIndex ) ) );
   
	cppVector one( v2.getX() - v1.getX(),
				v2.getY() - v1.getY(),
				v2.getZ() - v1.getZ() );
	cppVector two( v3.getX() - v1.getX(),
				v3.getY() - v1.getY(),
				v3.getZ() - v1.getZ() );

	cppVector normal = two.crossProduct( one );
	D = normal.geti()*v1.getX()
				+ normal.getj()*v1.getY()
				+ normal.getk()*v1.getZ();
	A = normal.geti();
	B = normal.getj();
	C = normal.getk();		
}

inline vertex CTin::gridCoord( long column, long row )
{	double x = 0, y = 0;
	_grid->CellToProj(column,row,&x,&y);
	return vertex( x, y, gridValue( column, row ) );
}

inline double CTin::gridValue( long column, long row )
{	
	VARIANT val;
	VariantInit(&val); //added by Rob Cairns 4-Jan-06
	_grid->get_Value(column,row,&val);
	double dval;
	dVal(val,dval);
	VariantClear(&val); //added by Rob Cairns 4-Jan-06
	return dval;
}

inline double CTin::gridMin()
{	VARIANT min;
	VariantInit(&min); //added by Rob Cairns 4-Jan-06
	_grid->get_Minimum(&min);
	double dmin;
	dVal(min,dmin);
	VariantClear(&min); //added by Rob Cairns 4-Jan-06
	return dmin;
}

inline double CTin::gridMax()
{	VARIANT max;
	VariantInit(&max); //added by Rob Cairns 4-Jan-06
	_grid->get_Minimum(&max);
	double dmax;
	dVal(max,dmax);
	VariantClear(&max); //added by Rob Cairns 4-Jan-06
	return dmax;
}

void CTin::createMesh( long numDivisions, vertex v1, vertex v2, vertex v3, vertex v4 )
{
	if( numDivisions < 0 )
		numDivisions = 0;

	double total = (numDivisions+1)*(numDivisions+1);
	int percent = 0, newPercent = 0, percentCnt = 0;

	//	Vertex Layout
	//
	//	 v3		v4
	//
	//	 v1		v2
	//

	double xd = v1.xyDistance( v2 )/( numDivisions + 1 );
	double yd = v4.xyDistance( v1 )/( numDivisions + 1 );

	int y = 0, x = 0;
	double xV = 0.0, yV = 0.0;	
	long column, row;
	for( y = 0; y <= numDivisions + 1; y++ )
	{	for( x = 0; x <= numDivisions + 1; x++ )
		{	
			xV = v1.getX() + x*xd;
			yV = v4.getY() + y*yd;			
			gridRaster( xV, yV, column, row );
			_vtxTable.add( vertex( xV, yV, gridValue( column, row) ) );
		}
	}

	long v1i = 0, v2i = 0, v3i = 0, v4i = 0;
	long n1 = 0, n2 = 0, n3 = 0;
	long row1 = 0, row2 = 0;
	long numTrianglesInRow = (numDivisions+1)*2;
	long numTriangles = numTrianglesInRow * (numDivisions+1);
	long numVerticesInRow = numDivisions+2;
	long lowTriIndex = 0, hiTriIndex = 0;

	for( y = 0; y < numDivisions + 1; y++ )
	{	for( x = 0; x < numDivisions + 1; x++ )
		{	
			row1 = y*numVerticesInRow;
			row2 = (y+1)*numVerticesInRow;

			v1i = row1 + x;
			v2i = row1 + x + 1;
			v3i = row2 + x;
			v4i = row2 + x + 1;

			lowTriIndex = y*numTrianglesInRow;
			hiTriIndex = 2*(numDivisions+1) + y*numTrianglesInRow;

			//Even Row
			if( y%2 == 0 )
			{
				//Even Column
					//v3 to v2
				if( x%2 == 0 )
				{
					n1 = 2*x + y*numTrianglesInRow - 1;
					n2 = 2*x + y*numTrianglesInRow + 1;
					n3 = 2*x + (y-1)*numTrianglesInRow;
					if( n1 < lowTriIndex )	n1 = Undefined;
					if( n3 < 0 )			n3 = Undefined;
					
					//Triangle One
					tinTableRow t1( v1i, v3i, v2i, n1, n2, n3 );
					long rowt1 = _triTable.addRow( t1 );
					computeMaxDev( rowt1 );
					
					n1 = 2*x + y*numTrianglesInRow;
					n2 = 2*x + (y+1)*numTrianglesInRow + 1;
					n3 = 2*(x+1) + y*numTrianglesInRow;
					if( n2 >= numTriangles )	n2 = Undefined;
					if( n3 >= hiTriIndex )		n3 = Undefined;
					
					//Triangle Two
					tinTableRow t2( v2i, v3i, v4i, n1, n2, n3 );
					long rowt2 = _triTable.addRow( t2 );
					computeMaxDev( rowt2 );
				}	
				//Odd Column
					//v1 to v4
				else
				{	n1 = 2*x + y*numTrianglesInRow - 1;
					n2 = 2*x + (y+1)*numTrianglesInRow;
					n3 = 2*x + y*numTrianglesInRow + 1;
					
					if( n2 >= numTriangles )	n2 = Undefined;
				
					//Triangle One
					tinTableRow t1( v1i, v3i, v4i, n1, n2, n3 );
					long rowt1 = _triTable.addRow( t1 );
					computeMaxDev( rowt1 );
		
					n1 = 2*x + y*numTrianglesInRow;
					n2 = 2*(x+1) + y*numTrianglesInRow;
					n3 = 2*x + (y-1)*numTrianglesInRow + 1;
					
					if( n2 >= hiTriIndex )	n2 = Undefined;
					if( n3 < 0 )			n3 = Undefined;
									
					//Triangle Two
					tinTableRow t2( v1i, v4i, v2i, n1, n2, n3 );
					long rowt2 = _triTable.addRow( t2 );
					computeMaxDev( rowt2 );
				}						
			}
			//Odd Row
			else
			{
				//Even Column
					//v1 to v4
				if( x%2 == 0 )
				{
					n1 = 2*x + y*numTrianglesInRow - 1;
					n2 = 2*x + (y+1)*numTrianglesInRow;
					n3 = 2*x + y*numTrianglesInRow + 1;
					
					if( n1 < lowTriIndex )		n1 = Undefined;
					if( n2 >= numTriangles )	n2 = Undefined;
				
					//Triangle One
					tinTableRow t1( v1i, v3i, v4i, n1, n2, n3 );
					long rowt1 = _triTable.addRow( t1 );
					computeMaxDev( rowt1 );
				
					n1 = 2*x + y*numTrianglesInRow;
					n2 = 2*(x+1) + y*numTrianglesInRow;
					n3 = 2*x + (y-1)*numTrianglesInRow + 1;
					
					if( n2 >= hiTriIndex )	n2 = Undefined;
					
					//Triangle Two
					tinTableRow t2( v1i, v4i, v2i, n1, n2, n3 );
					long rowt2 = _triTable.addRow( t2 );
					computeMaxDev( rowt2 );
				
				}	
				//Odd Column
					//v3 to v2	
				else
				{	n1 = 2*x + y*numTrianglesInRow - 1;
					n2 = 2*x + y*numTrianglesInRow + 1;
					n3 = 2*x + (y-1)*numTrianglesInRow;
					
					//Triangle One
					tinTableRow t1( v1i, v3i, v2i, n1, n2, n3 );
					long rowt1 = _triTable.addRow( t1 );
					computeMaxDev( rowt1 );
			
					n1 = 2*x + y*numTrianglesInRow;
					n2 = 2*x + (y+1)*numTrianglesInRow + 1;
					n3 = 2*x + y*numTrianglesInRow + 2;					
					if( n2 >= numTriangles )	n2 = Undefined;
					if( n3 >= hiTriIndex )		n3 = Undefined;
				
					//Triangle Two
					tinTableRow t2( v2i, v3i, v4i, n1, n2, n3 );
					long rowt2 = _triTable.addRow( t2 );
					computeMaxDev( rowt2 );
				}	
			}

			newPercent = (int)(((++percentCnt)/total)*100);
			if( newPercent > percent )
			{	percent = newPercent;
				if( _globalCallback != NULL )
				{	//callback( percent, "Creating Tin Mesh" );
				}
			}							
		}
	}
}

inline void CTin::splitTriangle( long triIndex )
{
	long indexOne = *((long*)_triTable.getValue( VTX_ONE, triIndex ) );
	long indexTwo = *((long*)_triTable.getValue( VTX_TWO, triIndex ) );
	long indexThree = *((long*)_triTable.getValue( VTX_THREE, triIndex ) );
	vertex maxDevVertex = *((vertex*)_triTable.getValue( DEV_VERTEX, triIndex ) );
	long newVtxIndex = _vtxTable.add( maxDevVertex );

	long borderOne = *((long*)_triTable.getValue( BDR_ONE, triIndex ) );
	long borderTwo = *((long*)_triTable.getValue( BDR_TWO, triIndex ) );
	long borderThree = *((long*)_triTable.getValue( BDR_THREE, triIndex ) );

	long numTriangles = _triTable.size();
	//Triangle One
	tinTableRow triOne( indexOne, indexTwo, newVtxIndex,
						borderOne,
						numTriangles,		//Triangle Two
						numTriangles + 1 );	//Triangle Three

	//Triangle two
	tinTableRow triTwo( indexTwo, indexThree, newVtxIndex,
						borderTwo,
						numTriangles + 1,	//Triangle Three
						triIndex );			//Triangle One

	//Triangle three
	tinTableRow triThree( indexThree, indexOne, newVtxIndex,
						  borderThree,
						  triIndex,			//Triangle One
						  numTriangles );	//Triangle Two

	//Add the new triangles to the table
	_triTable.setRow( triOne, triIndex );
	long triTwoIndex = _triTable.addRow( triTwo );
	long triThreeIndex = _triTable.addRow( triThree );

	//1.	Split to the edge perpendicularly or
	//2.	Split the triangles across the old diagonal

	// IF 1. Fails THEN
	//		compute the max deviation
	// IF 2. Fails THEN
	//		change the old border of triangle one
	//		compute the max deviation
			
	if( borderOne == Undefined )
	{	if( !splitPerp( triIndex, newVtxIndex, TinEdge ) )
			computeMaxDev( triIndex );				
	}
	else if( !splitQuad( triIndex, borderOne ) )
		computeMaxDev( triIndex );	

	if( borderTwo == Undefined )
	{	if( !splitPerp( triTwoIndex, newVtxIndex, TinEdge ) )
			computeMaxDev( triTwoIndex );		
	}
	else if( !splitQuad( triTwoIndex, borderTwo ) )
	{	changeBorder( borderTwo, triIndex, triTwoIndex );
		computeMaxDev( triTwoIndex );
	}

	if( borderThree == Undefined )
	{	if( !splitPerp( triThreeIndex, newVtxIndex, TinEdge ) )
			computeMaxDev( triThreeIndex );			
	}
	else if( !splitQuad( triThreeIndex, borderThree ) )
	{	changeBorder( borderThree, triIndex, triThreeIndex );
		computeMaxDev( triThreeIndex );
	}
}

inline void CTin::gridRaster( double x, double y, long & column, long & row )
{	_grid->ProjToCell( x, y, &column, &row);
}

inline void CTin::computeMaxDev( long triIndex )
{
	double A, B, C, D;
	computeTriangleEquation( triIndex, A, B, C, D );
	vertex v1 = _vtxTable.getVertex( *((long*)_triTable.getValue( VTX_ONE, triIndex )) );
	vertex v2 = _vtxTable.getVertex( *((long*)_triTable.getValue( VTX_TWO, triIndex )) );
	vertex v3 = _vtxTable.getVertex( *((long*)_triTable.getValue( VTX_THREE, triIndex )) );

	long r1c, r1r;
	gridRaster( v1.getX(), v1.getY(), r1c, r1r );
	long r2c, r2r;
	gridRaster( v2.getX(), v2.getY(), r2c, r2r );
	long r3c, r3r;
	gridRaster( v3.getX(), v3.getY(), r3c, r3r );

	long lli = (long)MIN( r1c, MIN( r2c, r3c ) );
	long llj = (long)MIN( r1r, MIN( r2r, r3r ) );
	long uri = (long)MAX( r1c, MAX( r2c, r3c ) );
	long urj = (long)MAX( r1r, MAX( r2r, r3r ) );
	long triangle_width = uri - lli;
	long triangle_height = urj - llj;

	double maxDev = 0.0;
	vertex devVertex(0,0,0);

	if( C != 0 && triangle_width > 0 && triangle_height > 0)
	{
		vertex triangle[3];
		triangle[0] = v1;
		triangle[1] = v2;
		triangle[2] = v3;		

		//Find the max_deviation from the polygon's plane
		for( int y = llj; y < urj; y++ )
		{	for( int x = lli; x < uri; x++ )
			{		
				vertex vert = gridCoord( x, y );
				if( vtxInTriangle( triangle, vert )	)					
				{	
					double planeZ = ( - A*vert.getX()
									   - B*vert.getY()
									   + D )/C;
					double dz = fabs( vert.getZ() - planeZ );
					if( dz > maxDev )
					{	
						//Check to make sure it won't split to a bad triangle
						if( !willCreateBadTriangles( triangle, gridCoord( x, y ) ) )
						{
							devVertex = vert;
							maxDev = dz;
						}					
					}					
				}				
			}
		}					
	}	
	_triTable.setValue( MAX_DEV, triIndex, &maxDev );
	_triTable.setValue( DEV_VERTEX, triIndex, &devVertex );
	_devHeap.insert( triIndex, maxDev, devVertex );			
}

inline bool CTin::splitPerp( long triIndex, long perpPointIndex, PerpSplitMethod perpSplitMethod )
{
	if( perpSplitMethod == TinEdge )
	{
		//Find the points on the line
		long indexOne = *((long*)_triTable.getValue( VTX_ONE, triIndex ) );
		long indexTwo = *((long*)_triTable.getValue( VTX_TWO, triIndex ) );
		long indexThree = *((long*)_triTable.getValue( VTX_THREE, triIndex ) );
		
		//Find the Vertices that define the line
		long lineIndexOne, lineIndexTwo;
		if( indexOne != perpPointIndex )
			lineIndexOne = indexOne;
		else
			lineIndexOne = indexTwo;
		if( indexThree != perpPointIndex )
			lineIndexTwo = indexThree;
		else
			lineIndexTwo = indexTwo;

		vertex lineVtx1 = _vtxTable.getVertex( lineIndexOne );
		vertex lineVtx2 = _vtxTable.getVertex( lineIndexTwo );
		vertex perpVtx = _vtxTable.getVertex( perpPointIndex );

		long intersectionIndex;
		vertex intersectionVtx;
		double lineLength = lineVtx1.xyDistance( lineVtx2 );
		lineLength *= lineLength;
		double perpLineLength = 0;

		//y = m*x + b;
		double dx = lineVtx2.getX() - lineVtx1.getX();
		double dy = lineVtx2.getY() - lineVtx1.getY();
		if( dx != 0 )
		{	
			//y = m*x + b1;				//line
			//y = (-1/m)*x + b2;		//perp line
			//x = (b2 - b1)/(m - 1/m);	//intersection point

			double m = (double)dy/dx;
			if( m != 0.0 )
			{
				long b1 = (long)(lineVtx1.getY() - ( m * lineVtx1.getX() ));
				double one_over_m = -1/m;
				long b2 = (long)(perpVtx.getY() - ( one_over_m * perpVtx.getX() ));
				
				//intersection point
				long x = (long)(( b2 - b1 ) / ( m - one_over_m ));
				long y = (long)(one_over_m * x + b2);

				intersectionVtx = vertex( x, y, 0 );
				
				//Test to see if the intersection point lies in the triangle
				if( intersectionVtx.getX() >= lineVtx2.getX() &&
					intersectionVtx.getX() >= lineVtx1.getX() )
						return false;
				else if( intersectionVtx.getX() <= lineVtx2.getX() &&
					intersectionVtx.getX() <= lineVtx1.getX() )
						return false;

				perpLineLength = perpVtx.xyDistance( intersectionVtx );
				perpLineLength *= perpLineLength;
			}
			else
			{	intersectionVtx = vertex( perpVtx.getX(), lineVtx2.getY(), 0 );

				//Test to see if the intersection point lies in the triangle
				if( intersectionVtx.getX() >= lineVtx2.getX() &&
					intersectionVtx.getX() >= lineVtx1.getX() )
						return false;
				else if( intersectionVtx.getX() <= lineVtx2.getX() &&
					intersectionVtx.getX() <= lineVtx1.getX() )
						return false;

				perpLineLength = pow( lineVtx2.getY() - perpVtx.getY(), 2 );
				
			}
		}
		else
		{
			intersectionVtx = vertex( lineVtx2.getX(), perpVtx.getY(), 0 );

			//Test to see if the intersection point lies in the triangle
			if( intersectionVtx.getY() >= lineVtx2.getY() &&
				intersectionVtx.getY() >= lineVtx1.getY() )
					return false;
			else if( intersectionVtx.getY() <= lineVtx2.getY() &&
				intersectionVtx.getY() <= lineVtx1.getY() )
					return false;

			perpLineLength = pow( lineVtx2.getX() - perpVtx.getX(), 2 );
			
		}
		
		if( perpLineLength < lineLength )
		{
			long rc, rr;
			gridRaster( intersectionVtx.getX(), intersectionVtx.getY(), rc, rr );
			intersectionVtx.setZ( gridValue( rc, rr ) );
			intersectionIndex = _vtxTable.add( intersectionVtx );

			//Triangle One
			tinTableRow triOne;
			triOne.vertexOne = perpPointIndex;
			if( lineIndexOne == indexAfterClockwise( triIndex, perpPointIndex ) )
				triOne.vertexTwo = lineIndexOne;
			else
				triOne.vertexTwo = lineIndexTwo;
			triOne.vertexThree = intersectionIndex;
			
			//Triangle Two
			tinTableRow triTwo;
			triTwo.vertexOne = intersectionIndex;
			if( triOne.vertexTwo == lineIndexOne )
				triTwo.vertexTwo = lineIndexTwo;
			else
				triTwo.vertexTwo = lineIndexOne;
			triTwo.vertexThree = perpPointIndex;
			long triTwoIndex = _triTable.addRow( triTwo );

			//Update Triangle Index in the table
			_triTable.setValue( VTX_ONE, triIndex, &(triOne.vertexOne) );
			_triTable.setValue( VTX_TWO, triIndex, &(triOne.vertexTwo) );
			_triTable.setValue( VTX_THREE, triIndex, &(triOne.vertexThree) );
			
			long possBorder1 = *((long*)_triTable.getValue( BDR_ONE, triIndex ) );
			long possBorder2 = *((long*)_triTable.getValue( BDR_TWO, triIndex ) );
			long possBorder3 = *((long*)_triTable.getValue( BDR_THREE, triIndex ) );
			long possBorder4 = triIndex;
			long possBorder5 = triTwoIndex;
			long possBorder6 = Undefined;
					
			//Find the Borders for Triangle One
			setBorders( triIndex, possBorder1, possBorder2, possBorder3,
						 possBorder4, possBorder5, possBorder6, triTwoIndex );
			long newBorder1 = *((long*)_triTable.getValue( BDR_ONE, triIndex ) );
			if( newBorder1 != triTwoIndex )
				if( newBorder1 != -1 )
					changeBorder( newBorder1, triTwoIndex, triIndex );
			long newBorder2 = *((long*)_triTable.getValue( BDR_TWO, triIndex ) );
			if( newBorder2 != triTwoIndex )
				if( newBorder2 != -1 )
					changeBorder( newBorder2, triTwoIndex, triIndex );
			long newBorder3 = *((long*)_triTable.getValue( BDR_THREE, triIndex ) );
			if( newBorder3 != triTwoIndex )
				if( newBorder3 != -1 )
					changeBorder( newBorder3, triTwoIndex, triIndex );
			//Find the Borders for Triangle Two
			setBorders( triTwoIndex, possBorder1, possBorder2, possBorder3,
						 possBorder4, possBorder5, possBorder6, triIndex );
			newBorder1 = *((long*)_triTable.getValue( BDR_ONE, triTwoIndex ) );
			if( newBorder1 != triIndex )
				if( newBorder1 != -1 )
					changeBorder( newBorder1, triIndex, triTwoIndex );
			newBorder2 = *((long*)_triTable.getValue( BDR_TWO, triTwoIndex ) );
			if( newBorder2 != triIndex )
				if( newBorder2 != -1 )
					changeBorder( newBorder2, triIndex, triTwoIndex );
			newBorder3 = *((long*)_triTable.getValue( BDR_THREE, triTwoIndex ) );
			if( newBorder3 != triIndex )
				if( newBorder3 != -1 )
					changeBorder( newBorder3, triIndex, triTwoIndex );
			
			//Compute the max_devs of the new triangles
			computeMaxDev( triIndex );
			computeMaxDev( triTwoIndex );
			return true;	
		}	
	}

	return false;
}

inline bool CTin::splitQuad( long triOneIndex, long triTwoIndex )
{	
	//IF canSplit THEN
	//	triOne = new triangle;
	//	triTwo = old triangle;
	//	1. Set new rows in the triTable
	//	2. Compute maxDev for new triangles
	//	3. Insert new triangles on the heap
	
	long t1IndexUnshared = unsharedIndex( triOneIndex, triTwoIndex );
	long t2IndexUnshared = unsharedIndex( triTwoIndex, triOneIndex );
	long index1Shared = Undefined;
	long index2Shared = Undefined;
	sharedIndexes( triOneIndex, triTwoIndex, index1Shared, index2Shared );

	if( canSplitQuad( t1IndexUnshared, t2IndexUnshared, index1Shared, index2Shared ) )	
	{	
		//Create ClockWise Polygons

		//Triangle One
		tinTableRow triOne;
		triOne.vertexOne = t1IndexUnshared;
		if( index1Shared == indexAfterClockwise( triOneIndex, t1IndexUnshared ) )
			triOne.vertexTwo = index1Shared;
		else
			triOne.vertexTwo = index2Shared;
		triOne.vertexThree = t2IndexUnshared;
		
		//Triangle Two
		tinTableRow triTwo;
		triTwo.vertexOne = t2IndexUnshared;
		if( index1Shared == indexAfterClockwise( triTwoIndex, t2IndexUnshared ) )
			triTwo.vertexTwo = index1Shared;
		else
			triTwo.vertexTwo = index2Shared;
		triTwo.vertexThree = t1IndexUnshared;

		long possBorder1 = *((long*)_triTable.getValue( BDR_ONE, triOneIndex ) );
		long possBorder2 = *((long*)_triTable.getValue( BDR_TWO, triOneIndex ) );
		long possBorder3 = *((long*)_triTable.getValue( BDR_THREE, triOneIndex ) );
		long possBorder4 = *((long*)_triTable.getValue( BDR_ONE, triTwoIndex ) );
		long possBorder5 = *((long*)_triTable.getValue( BDR_TWO, triTwoIndex ) );
		long possBorder6 = *((long*)_triTable.getValue( BDR_THREE, triTwoIndex ) );

		
		//Update Triangles in the table
		_triTable.setValue( VTX_ONE, triOneIndex, &(triOne.vertexOne ) );
		_triTable.setValue( VTX_TWO, triOneIndex, &(triOne.vertexTwo ) );
		_triTable.setValue( VTX_THREE, triOneIndex, &(triOne.vertexThree ) );
		_triTable.setValue( VTX_ONE, triTwoIndex, &(triTwo.vertexOne ) );
		_triTable.setValue( VTX_TWO, triTwoIndex, &(triTwo.vertexTwo ) );
		_triTable.setValue( VTX_THREE, triTwoIndex, &(triTwo.vertexThree ) );

		//Find the Borders for Triangle One
		setBorders( triOneIndex, possBorder1, possBorder2, possBorder3,
					possBorder4, possBorder5, possBorder6, triTwoIndex );
		long newBorder1 = *((long*)_triTable.getValue( BDR_ONE, triOneIndex ) );
		if( newBorder1 != triTwoIndex )
			if( newBorder1 != -1 )
				changeBorder( newBorder1, triTwoIndex, triOneIndex );
		long newBorder2 = *((long*)_triTable.getValue( BDR_TWO, triOneIndex ) );
		if( newBorder2 != triTwoIndex )
			if( newBorder2 != -1 )
				changeBorder( newBorder2, triTwoIndex, triOneIndex );
		long newBorder3 = *((long*)_triTable.getValue( BDR_THREE, triOneIndex ) );
		if( newBorder3 != triTwoIndex )
			if( newBorder3 != -1 )
				changeBorder( newBorder3, triTwoIndex, triOneIndex );

		//Find the Borders for Triangle Two
		setBorders( triTwoIndex, possBorder1, possBorder2, possBorder3,
					possBorder4, possBorder5, possBorder6, triOneIndex );
		newBorder1 = *((long*)_triTable.getValue( BDR_ONE, triTwoIndex ) );
		if( newBorder1 != triOneIndex )
			if( newBorder1 != -1 )
				changeBorder( newBorder1, triOneIndex, triTwoIndex );
		newBorder2 = *((long*)_triTable.getValue( BDR_TWO, triTwoIndex ) );
		if( newBorder2 != triOneIndex )
			if( newBorder2 != -1 )
				changeBorder( newBorder2, triOneIndex, triTwoIndex );
		newBorder3 = *((long*)_triTable.getValue( BDR_THREE, triTwoIndex ) );
		if( newBorder3 != triOneIndex )
			if( newBorder3 != -1 )
				changeBorder( newBorder3, triOneIndex, triTwoIndex );
		
		//Compute the max_devs of the new triangles
		computeMaxDev( triOneIndex );
		computeMaxDev( triTwoIndex );
		return true;
	}
	else
		return false;
}

inline void CTin::changeBorder( long triIndex, long oldValue, long newValue )
{	
	if( *((long*)_triTable.getValue( BDR_ONE, triIndex ) ) == oldValue )
		_triTable.setValue( BDR_ONE, triIndex, &newValue );

	else if( *((long*)_triTable.getValue( BDR_TWO, triIndex ) ) == oldValue )
		_triTable.setValue( BDR_TWO, triIndex, &newValue );

	else if( *((long*)_triTable.getValue( BDR_THREE, triIndex ) ) == oldValue )
		_triTable.setValue( BDR_THREE, triIndex, &newValue );
}

//inline double CTin::MIN( double one, double two )
//{	if( one < two )
//		return one;
//	return two;
//}
//
//inline double CTin::MAX( double one, double two )
//{	if( one > two )
//		return one;
//	return two;
//}

bool CTin::willCreateBadTriangles( vertex * triangle, vertex testVertex )
{	
	vertex v1 = triangle[0];
	vertex v2 = triangle[1];
	vertex v3 = triangle[2];
	vertex v4 = testVertex;

	if( _splitMethod == AngleDeviation )
	{
		double a = v1.xyDistance( v2 );
		double b = v2.xyDistance( v4 );
		double c = v4.xyDistance( v1 );
		if( a <= 0.0 || b <= 0.0 || c <= 0.0 )
			return true;
		double cosa1 = - ( a*a - b*b - c*c )/(2*b*c);
		double cosa2 = - ( b*b - a*a - c*c )/(2*a*c);
		double cosa3 = - ( c*c - a*a - b*b )/(2*a*b);

		a = v2.xyDistance( v3 );
		b = v3.xyDistance( v4 );
		c = v4.xyDistance( v2 );
		if( a <= 0.0 || b <= 0.0 || c <= 0.0 )
			return true;	
		double cosa4 = - ( a*a - b*b - c*c )/(2*b*c);
		double cosa5 = - ( b*b - a*a - c*c )/(2*a*c);
		double cosa6 = - ( c*c - a*a - b*b )/(2*a*b);

		a = v3.xyDistance( v1 );
		b = v1.xyDistance( v4 );
		c = v4.xyDistance( v3 );
		if( a <= 0.0 || b <= 0.0 || c <= 0.0 )
			return true;
		double cosa7 = - ( a*a - b*b - c*c )/(2*b*c);
		double cosa8 = - ( b*b - a*a - c*c )/(2*a*c);
		double cosa9 = - ( c*c - a*a - b*b )/(2*a*b);
		
		double cosMinAngle = cos( _splitParam*dTOr );

		if( cosa1 > cosMinAngle ||
			cosa2 > cosMinAngle ||
			cosa3 > cosMinAngle ||
			cosa4 > cosMinAngle ||
			cosa5 > cosMinAngle ||
			cosa6 > cosMinAngle ||
			cosa7 > cosMinAngle ||
			cosa8 > cosMinAngle ||
			cosa9 > cosMinAngle )
			return true;
		else
			return false;
	}
	else if( _splitMethod == InscribedRadius )
	{
		double radiusOne = inscribedCircleRad( v1, v2, v4 );
		double radiusTwo = inscribedCircleRad( v2, v3, v4 );
		double radiusThree = inscribedCircleRad( v3, v1, v4 );

		if( radiusOne < _splitParam ||
			radiusTwo < _splitParam ||
			radiusThree < _splitParam )
			return true;
		else
			return false;
	}

	return true;
}

inline long CTin::indexAfterClockwise( long triIndex, long vtxIndex )
{	long indexOne = *((long*)_triTable.getValue( VTX_ONE, triIndex ) );
	long indexTwo = *((long*)_triTable.getValue( VTX_TWO, triIndex ) );
	long indexThree = *((long*)_triTable.getValue( VTX_THREE, triIndex ) );	

	if( vtxIndex == indexOne )
		return indexTwo;
	else if( vtxIndex == indexTwo )
		return indexThree;
	else if( vtxIndex == indexThree )
		return indexOne;

	return Undefined;
}

inline void CTin::setBorders( long triIndex, long possBorder1, long possBorder2, long possBorder3,
							 long possBorder4, long possBorder5, long possBorder6, long possBorder7 )
{	
	
	if( possBorder1 == triIndex )
		possBorder1 = Undefined;
	if( possBorder2 == triIndex )
		possBorder2 = Undefined;
	if( possBorder3 == triIndex )
		possBorder3 = Undefined;
	if( possBorder4 == triIndex )
		possBorder4 = Undefined;
	if( possBorder5 == triIndex )
		possBorder5 = Undefined;
	if( possBorder6 == triIndex )
		possBorder6 = Undefined;
	if( possBorder7 == triIndex )
		possBorder7 = Undefined;

	long indexOne = *((long*)_triTable.getValue( VTX_ONE, triIndex ) );
	long indexTwo = *((long*)_triTable.getValue( VTX_TWO, triIndex ) );
	long indexThree = *((long*)_triTable.getValue( VTX_THREE, triIndex ) );
	long undefined = Undefined;

	//BORDER 1
	if( hasVertices( possBorder1, indexOne, indexTwo ) )
		_triTable.setValue( BDR_ONE, triIndex, &possBorder1 );
	else if( hasVertices( possBorder2, indexOne, indexTwo ) )
		_triTable.setValue( BDR_ONE, triIndex, &possBorder2 );
	else if( hasVertices( possBorder3, indexOne, indexTwo ) )
		_triTable.setValue( BDR_ONE, triIndex, &possBorder3 );
	else if( hasVertices( possBorder4, indexOne, indexTwo ) )
		_triTable.setValue( BDR_ONE, triIndex, &possBorder4 );
	else if( hasVertices( possBorder5, indexOne, indexTwo ) )
		_triTable.setValue( BDR_ONE, triIndex, &possBorder5 );
	else if( hasVertices( possBorder6, indexOne, indexTwo ) )
		_triTable.setValue( BDR_ONE, triIndex, &possBorder6 );
	else if( hasVertices( possBorder7, indexOne, indexTwo ) )
		_triTable.setValue( BDR_ONE, triIndex, &possBorder7 );
	else
		_triTable.setValue( BDR_ONE, triIndex, &undefined );

	//BORDER 2
	if( hasVertices( possBorder1, indexTwo, indexThree ) )
		_triTable.setValue( BDR_TWO, triIndex, &possBorder1 );
	else if( hasVertices( possBorder2, indexTwo, indexThree ) )
		_triTable.setValue( BDR_TWO, triIndex, &possBorder2 );
	else if( hasVertices( possBorder3, indexTwo, indexThree ) )
		_triTable.setValue( BDR_TWO, triIndex, &possBorder3 );
	else if( hasVertices( possBorder4, indexTwo, indexThree ) )
		_triTable.setValue( BDR_TWO, triIndex, &possBorder4 );
	else if( hasVertices( possBorder5, indexTwo, indexThree ) )
		_triTable.setValue( BDR_TWO, triIndex, &possBorder5 );
	else if( hasVertices( possBorder6, indexTwo, indexThree ) )
		_triTable.setValue( BDR_TWO, triIndex, &possBorder6 );
	else if( hasVertices( possBorder7, indexTwo, indexThree ) )
		_triTable.setValue( BDR_TWO, triIndex, &possBorder7 );
	else
		_triTable.setValue( BDR_TWO, triIndex, &undefined );
	
	//BORDER 3
	if( hasVertices( possBorder1, indexThree, indexOne ) )
		_triTable.setValue( BDR_THREE, triIndex, &possBorder1 );
	else if( hasVertices( possBorder2, indexThree, indexOne ) )
		_triTable.setValue( BDR_THREE, triIndex, &possBorder2 );
	else if( hasVertices( possBorder3, indexThree, indexOne ) )
		_triTable.setValue( BDR_THREE, triIndex, &possBorder3 );
	else if( hasVertices( possBorder4, indexThree, indexOne ) )
		_triTable.setValue( BDR_THREE, triIndex, &possBorder4 );
	else if( hasVertices( possBorder5, indexThree, indexOne ) )
		_triTable.setValue( BDR_THREE, triIndex, &possBorder5 );
	else if( hasVertices( possBorder6, indexThree, indexOne ) )
		_triTable.setValue( BDR_THREE, triIndex, &possBorder6 );
	else if( hasVertices( possBorder7, indexThree, indexOne ) )
		_triTable.setValue( BDR_THREE, triIndex, &possBorder7 );
	else
		_triTable.setValue( BDR_THREE, triIndex, &undefined );
}

inline long CTin::unsharedIndex( long triOne, long triTwo )
{	long t1_index_one = *((long*)_triTable.getValue( VTX_ONE, triOne ) );
	long t1_index_two = *((long*)_triTable.getValue( VTX_TWO, triOne ) );
	long t1_index_three = *((long*)_triTable.getValue( VTX_THREE, triOne ) );

	long t2_index_one = *((long*)_triTable.getValue( VTX_ONE, triTwo ) );
	long t2_index_two = *((long*)_triTable.getValue( VTX_TWO, triTwo ) );
	long t2_index_three = *((long*)_triTable.getValue( VTX_THREE, triTwo ) );

	if( t1_index_one != t2_index_one &&
		t1_index_one != t2_index_two &&
		t1_index_one != t2_index_three )
			return t1_index_one;
	else if( t1_index_two != t2_index_one &&
			 t1_index_two != t2_index_two &&
			 t1_index_two != t2_index_three )
				 return t1_index_two;
	else if( t1_index_three != t2_index_one &&
			 t1_index_three != t2_index_two &&
			 t1_index_three != t2_index_three )
				 return t1_index_three;
	else
		return Undefined;
}

inline void CTin::sharedIndexes( long triOne, long triTwo, long & index1, long & index2 )
{	index1 = Undefined;
	index2 = Undefined;

	long t1_index_one = *((long*)_triTable.getValue( VTX_ONE, triOne ) );
	long t1_index_two = *((long*)_triTable.getValue( VTX_TWO, triOne ) );
	long t1_index_three = *((long*)_triTable.getValue( VTX_THREE, triOne ) );
	
	long unshared = unsharedIndex( triOne, triTwo );
	if( t1_index_one == unshared )
	{	index1 = t1_index_two;
		index2 = t1_index_three;
	}
	else if( t1_index_two == unshared )
	{	index1 = t1_index_three;
		index2 = t1_index_one;
	}
	else if( t1_index_three == unshared )
	{	index1 = t1_index_one;
		index2 = t1_index_two;
	}	
}

bool CTin::canSplitQuad( long p1_unshared, long p2_unshared, long p1_shared, long p2_shared )
{	
	vertex v1 = _vtxTable.getVertex( p1_unshared );
	vertex v2 = _vtxTable.getVertex( p2_unshared );
	vertex v3 = _vtxTable.getVertex( p1_shared );
	vertex v4 = _vtxTable.getVertex( p2_shared );

	if( _splitMethod == AngleDeviation )
	{
		//Quad Triangles
		double a = v1.xyDistance( v2 );
		double b = v2.xyDistance( v3 );
		double c = v3.xyDistance( v1 );
		if( a <= 0.0 || b <= 0.0 || c <= 0.0 )
			return false;
		double cosa1 = - ( a*a - b*b - c*c )/(2*b*c);
		double cosa2 = - ( b*b - a*a - c*c )/(2*a*c);
		double cosa3 = - ( c*c - a*a - b*b )/(2*a*b);

		a = v1.xyDistance( v2 );
		b = v2.xyDistance( v4 );
		c = v4.xyDistance( v1 );
		if( a <= 0.0 || b <= 0.0 || c <= 0.0 )
			return false;
		double cosa4 = - ( a*a - b*b - c*c )/(2*b*c);
		double cosa5 = - ( b*b - a*a - c*c )/(2*a*c);
		double cosa6 = - ( c*c - a*a - b*b )/(2*a*b);

		//Check the Quad Triangles to see if they're good enough
		double cosMinAngle = cos( _splitParam*dTOr );

		if( cosa1 > cosMinAngle ||
			cosa2 > cosMinAngle ||
			cosa3 > cosMinAngle ||
			cosa4 > cosMinAngle ||
			cosa5 > cosMinAngle ||
			cosa6 > cosMinAngle )	
			return false;


		//Current Triangles
		a = v1.xyDistance( v3 );
		b = v3.xyDistance( v4 );
		c = v4.xyDistance( v1 );	
		double cosa7 = - ( a*a - b*b - c*c )/(2*b*c);
		double cosa8 = - ( b*b - a*a - c*c )/(2*a*c);
		double cosa9 = - ( c*c - a*a - b*b )/(2*a*b);

		a = v2.xyDistance( v3 );
		b = v3.xyDistance( v4 );
		c = v4.xyDistance( v2 );	
		double cosa10 = - ( a*a - b*b - c*c )/(2*b*c);
		double cosa11 = - ( b*b - a*a - c*c )/(2*a*c);
		double cosa12 = - ( c*c - a*a - b*b )/(2*a*b);

		double quadAngleDev = fabs( .5 - cosa1 ) +
								 fabs( .5 - cosa2 ) +
								 fabs( .5 - cosa3 ) +
								 fabs( .5 - cosa4 ) +
								 fabs( .5 - cosa5 ) +
								 fabs( .5 - cosa6 );
		double currentAngleDev = fabs( .5 - cosa7 ) +
								 fabs( .5 - cosa8 ) +
								 fabs( .5 - cosa9 ) +
								 fabs( .5 - cosa10 ) +
								 fabs( .5 - cosa11 ) +
								 fabs( .5 - cosa12 );

		//If the split deviation from a 60 deg angle is less
		//	the current then split
		if( quadAngleDev < currentAngleDev )
		{
			//Vertex in Triangle Test
				//Testing for concave polygon on Split
			vertex triangle[3];
			triangle[0] = v1;  triangle[1] = v2;  triangle[2] = v3;
			if( vtxInTriangle( triangle, v4 ) )
				return false;
			triangle[0] = v2;  triangle[1] = v3;  triangle[2] = v4;
			if( vtxInTriangle( triangle, v1 ) )
				return false;
			triangle[0] = v3;  triangle[1] = v4;  triangle[2] = v1;
			if( vtxInTriangle( triangle, v2 ) )
				return false;
			triangle[0] = v4;  triangle[1] = v1;  triangle[2] = v2;
			if( vtxInTriangle( triangle, v3 ) )
				return false;

			return true;
		}
		else
			return false;
	}
	else if( _splitMethod == InscribedRadius )
	{
		//Find the inscribed Radius of the current triangles
		double currRadiusOne = inscribedCircleRad( v1, v4, v3 );
		double currRadiusTwo = inscribedCircleRad( v2, v3, v4 );

		//Find the inscribed Radius of the proposed triangles
		double newRadiusOne = inscribedCircleRad( v1, v4, v2 );
		double newRadiusTwo = inscribedCircleRad( v1, v2, v3 );

		//Check the proposed triangles for initial qualifications
		if( newRadiusOne < _splitParam ||
			newRadiusTwo < _splitParam )
			return false;
		else
		{	
			//Choose the Maximum inscribed radius of the Minimums
			//	of each set
			double minCur = MIN( currRadiusOne, currRadiusTwo );
			double minNew = MIN( newRadiusOne, newRadiusTwo );

			if( minNew <= minCur )
				return false;
			else
			{
				//Vertex in Triangle Test
					//Testing for concave polygon on Split
				vertex triangle[3];
				triangle[0] = v1;  triangle[1] = v2;  triangle[2] = v3;
				if( vtxInTriangle( triangle, v4 ) )
					return false;
				triangle[0] = v2;  triangle[1] = v3;  triangle[2] = v4;
				if( vtxInTriangle( triangle, v1 ) )
					return false;
				triangle[0] = v3;  triangle[1] = v4;  triangle[2] = v1;
				if( vtxInTriangle( triangle, v2 ) )
					return false;
				triangle[0] = v4;  triangle[1] = v1;  triangle[2] = v2;
				if( vtxInTriangle( triangle, v3 ) )
					return false;
			
				return true;
			}
		}
	}
	return false;
}

inline double CTin::inscribedCircleRad( vertex one, vertex two, vertex three )
{
	double a = one.xyDistance( two );
	double b = two.xyDistance( three );
	double c = three.xyDistance( one );
	double s = ( a + b + c ) * .5;
	
	if( s == 0 )
		return 0;
	else
	{	double top = ( s - a )*( s - b )*( s - c );
		double tmp = top/s;
		CheckZero( tmp );	
		return sqrt( tmp );		
	}
}

inline bool CTin::hasVertices( long triIndex, long vtxOne, long vtxTwo )
{	
	if( triIndex == -1 )
		return false;

	long indexOne = *((long*)_triTable.getValue( VTX_ONE, triIndex ) );
	long indexTwo = *((long*)_triTable.getValue( VTX_TWO, triIndex ) );
	long indexThree = *((long*)_triTable.getValue( VTX_THREE, triIndex ) );	

	int numberPoints = 0;

	if( indexOne == vtxOne || indexOne == vtxTwo )
		numberPoints++;
	if( indexTwo == vtxOne || indexTwo == vtxTwo )
		numberPoints++;
	if( indexThree == vtxOne || indexThree == vtxTwo )
		numberPoints++;
	
	if( numberPoints >= 2 )
		return true;
	else
		return false;
}

STDMETHODIMP CTin::get_IsNDTriangle(long TriIndex, VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( TriIndex >= 0 && TriIndex < _triTable.size() )
		*pVal = _dTriangles[TriIndex];
	else
		*pVal = TRUE;

	return S_OK;
}

/*Added functions*/

int XYZCompare(const void *v1,const void *v2)
{
	XYZ *p1,*p2;
	p1 = (XYZ *)v1;
	p2 = (XYZ *)v2;
	if (p1->x < p2->x)
		return(-1);
	else if (p1->x > p2->x)
		return(1);
	else
		return(0);
}

STDMETHODIMP CTin::CreateTinFromPoints(SAFEARRAY * Points, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//*************
	ITRIANGLE *tri;
	int ntri;
	XYZ *v;
	int nv;

	nv = (int)Points->rgsabound[0].cElements;
	v = new XYZ[nv];
	IPoint **Data = (IPoint**)Points->pvData;
	double x = 0.0;
	(*Data[0]).get_X(&x);
	for(int i =0; i < nv;i++)
	{
		(*Data[i]).get_X(&v[i].x);//(double)*(xPoints->pvData[0]);// xPoints[i];
		(*Data[i]).get_Y(&v[i].y);//v[i].y = yData[i];//yPoints[i];
		(*Data[i]).get_Z(&v[i].z);//v[i].z = zData[i];//zPoints[i];
	}

	//sort the xyz values based on the x component
	qsort((void *)v,(size_t)nv,sizeof(XYZ),XYZCompare);

	//prune any duplicate vertices, that share x and y values
	Prune(nv,v);

	//allocate space
	tri = new ITRIANGLE[nv*3];

	//Triangulate all points
	Triangulate(nv,v,tri,&ntri);

	//find the min and max points
	XYZ vMax,vMin;
	FindMaxXYZAndMinXYZ(nv,v,vMax,vMin);

	//find all bordering triangles
	FindAllBorders(ntri,tri);

	//Build the tin 
	BuildTin(nv,v,ntri,tri,vMax,vMin);

	*retval = TRUE;
	
	//*************
	return S_OK;
}

int CTin::Triangulate(int nv,XYZ *pxyz,ITRIANGLE *v,int *ntri)
{
   int *complete = NULL;
   IEDGE *edges = NULL;
   int nedge = 0;
   int trimax,emax = 200;
   int status = 0;

   int inside;
   int i,j,k;
   double xp,yp,x1,y1,x2,y2,x3,y3,xc,yc,r;
   double xmin,xmax,ymin,ymax,xmid,ymid;
   double dx,dy,dmax;

   /* Allocate memory for the completeness list, flag for each triangle */
   trimax = 4 * nv;
   if ((complete = (int*)malloc(trimax*sizeof(int))) == NULL) {
      status = 1;
      goto skip;
   }

   /* Allocate memory for the edge list */
   if ((edges = (IEDGE *)malloc(emax*(long)sizeof(IEDGE))) == NULL) {
      status = 2;
      goto skip;
   }

   /*
      Find the maximum and minimum vertex bounds.
      This is to allow calculation of the bounding triangle
   */
   xmin = pxyz[0].x;
   ymin = pxyz[0].y;
   xmax = xmin;
   ymax = ymin;
   for (i=1;i<nv;i++) {
      if (pxyz[i].x < xmin) xmin = pxyz[i].x;
      if (pxyz[i].x > xmax) xmax = pxyz[i].x;
      if (pxyz[i].y < ymin) ymin = pxyz[i].y;
      if (pxyz[i].y > ymax) ymax = pxyz[i].y;
   }
   dx = xmax - xmin;
   dy = ymax - ymin;
   dmax = (dx > dy) ? dx : dy;
   xmid = (xmax + xmin) / 2.0;
   ymid = (ymax + ymin) / 2.0;

   /*
      Set up the supertriangle
      This is a triangle which encompasses all the sample points.
      The supertriangle coordinates are added to the end of the
      vertex list. The supertriangle is the first triangle in
      the triangle list.
   */
   pxyz[nv+0].x = xmid - 20 * dmax;
   pxyz[nv+0].y = ymid - dmax;
   pxyz[nv+0].z = 0.0;
   pxyz[nv+1].x = xmid;
   pxyz[nv+1].y = ymid + 20 * dmax;
   pxyz[nv+1].z = 0.0;
   pxyz[nv+2].x = xmid + 20 * dmax;
   pxyz[nv+2].y = ymid - dmax;
   pxyz[nv+2].z = 0.0;
   v[0].p1 = nv;
   v[0].p2 = nv+1;
   v[0].p3 = nv+2;
   complete[0] = false;
   *ntri = 1;

   /*
      Include each point one at a time into the existing mesh
   */
   for (i=0;i<nv;i++) {

      xp = pxyz[i].x;
      yp = pxyz[i].y;
      nedge = 0;

      /*
         Set up the edge buffer.
         If the point (xp,yp) lies inside the circumcircle then the
         three edges of that triangle are added to the edge buffer
         and that triangle is removed.
      */
      for (j=0;j<(*ntri);j++) {
         if (complete[j])
            continue;
         x1 = pxyz[v[j].p1].x;
         y1 = pxyz[v[j].p1].y;
         x2 = pxyz[v[j].p2].x;
         y2 = pxyz[v[j].p2].y;
         x3 = pxyz[v[j].p3].x;
         y3 = pxyz[v[j].p3].y;
         inside = CircumCircle(xp,yp,x1,y1,x2,y2,x3,y3,&xc,&yc,&r);
         if (xc + r < xp)
            complete[j] = true;
         if (inside) {
            /* Check that we haven't exceeded the edge list size */
            if (nedge+3 >= emax) {
               emax += 100;
               if ((edges = (IEDGE *)realloc(edges,emax*(long)sizeof(IEDGE))) == NULL) {
                  status = 3;
                  goto skip;
               }
            }
            edges[nedge+0].p1 = v[j].p1;
            edges[nedge+0].p2 = v[j].p2;
            edges[nedge+1].p1 = v[j].p2;
            edges[nedge+1].p2 = v[j].p3;
            edges[nedge+2].p1 = v[j].p3;
            edges[nedge+2].p2 = v[j].p1;
            nedge += 3;
            v[j] = v[(*ntri)-1];
            complete[j] = complete[(*ntri)-1];
            (*ntri)--;
            j--;
         }
      }

      /*
         Tag multiple edges
         Note: if all triangles are specified anticlockwise then all
               interior edges are opposite pointing in direction.
      */
      for (j=0;j<nedge-1;j++) {
         for (k=j+1;k<nedge;k++) {
            if ((edges[j].p1 == edges[k].p2) && (edges[j].p2 == edges[k].p1)) {
               edges[j].p1 = -1;
               edges[j].p2 = -1;
               edges[k].p1 = -1;
               edges[k].p2 = -1;
            }
            /* Shouldn't need the following, see note above */
            if ((edges[j].p1 == edges[k].p1) && (edges[j].p2 == edges[k].p2)) {
               edges[j].p1 = -1;
               edges[j].p2 = -1;
               edges[k].p1 = -1;
               edges[k].p2 = -1;
            }
         }
      }

      /*
         Form new triangles for the current point
         Skipping over any tagged edges.
         All edges are arranged in clockwise order.
      */
      for (j=0;j<nedge;j++) {
         if (edges[j].p1 < 0 || edges[j].p2 < 0)
            continue;
         if ((*ntri) >= trimax) {
            status = 4;
            goto skip;
         }
         v[*ntri].p1 = edges[j].p1;
         v[*ntri].p2 = edges[j].p2;
         v[*ntri].p3 = i;
         complete[*ntri] = false;
         (*ntri)++;
      }
   }

   /*
      Remove triangles with supertriangle vertices
      These are triangles which have a vertex number greater than nv
   */
   for (i=0;i<(*ntri);i++) {
      if (v[i].p1 >= nv || v[i].p2 >= nv || v[i].p3 >= nv) {
         v[i] = v[(*ntri)-1];
         (*ntri)--;
         i--;
      }
   }

skip:
   free(edges);
   free(complete);
   return(status);
}

/*
   Return true if a point (xp,yp) is inside the circumcircle made up
   of the points (x1,y1), (x2,y2), (x3,y3)
   The circumcircle centre is returned in (xc,yc) and the radius r
   NOTE: A point on the edge is inside the circumcircle
*/
int CTin::CircumCircle(double xp,double yp,
   double x1,double y1,double x2,double y2,double x3,double y3,
   double *xc,double *yc,double *r)
{
   double m1,m2,mx1,mx2,my1,my2;
   double dx,dy,rsqr,drsqr;
   double const EPSILON = 0.000001;

   /* Check for coincident points */
   if (fabs(y1-y2) < EPSILON && fabs(y2-y3) < EPSILON)
       return false;
  
   if (fabs(y2-y1) < EPSILON) {
      m2 = - (x3-x2) / (y3-y2);
      mx2 = (x2 + x3) / 2.0;
      my2 = (y2 + y3) / 2.0;
      *xc = (x2 + x1) / 2.0;
      *yc = m2 * (*xc - mx2) + my2;
   } else if (fabs(y3-y2) < EPSILON) {
      m1 = - (x2-x1) / (y2-y1);
      mx1 = (x1 + x2) / 2.0;
      my1 = (y1 + y2) / 2.0;
      *xc = (x3 + x2) / 2.0;
      *yc = m1 * (*xc - mx1) + my1;
   } else {
      m1 = - (x2-x1) / (y2-y1);
      m2 = - (x3-x2) / (y3-y2);
      mx1 = (x1 + x2) / 2.0;
      mx2 = (x2 + x3) / 2.0;
      my1 = (y1 + y2) / 2.0;
      my2 = (y2 + y3) / 2.0;
      *xc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
      *yc = m1 * (*xc - mx1) + my1;
   }

   dx = x2 - *xc;
   dy = y2 - *yc;
   rsqr = dx*dx + dy*dy;
   *r = sqrt(rsqr);

   dx = xp - *xc;
   dy = yp - *yc;
   drsqr = dx*dx + dy*dy;

   return((drsqr <= rsqr) ? true : false);
}

void CTin::FindMaxXYZAndMinXYZ(int nv, XYZ v[],XYZ &vMax,XYZ &vMin)
{
	if(nv == 0) return;
	
	//set the initial values to the first vertex
	vMax.x = v[0].x;
	vMax.y = v[0].y;
	vMax.z = v[0].z;
	vMin.x = v[0].x;
	vMin.y = v[0].y;
	vMin.z = v[0].z;

	for(int i=0; i < nv; i++)
	{
		//find the max and min X
		if(v[i].x > vMax.x)
			vMax.x = v[i].x;
		else if(v[i].x < vMin.x)
			vMin.x = v[i].x;

		//find the max and min Y
		if(v[i].y > vMax.y)
			vMax.y = v[i].y;
		else if(v[i].y < vMin.y)
			vMin.y = v[i].y;

		//find the max and min Z
		if(v[i].z > vMax.z)
			vMax.z = v[i].z;
		else if(v[i].z < vMin.z)
			vMin.z = v[i].z;
	}
}

void CTin::FindAllBorders(int ntri,ITRIANGLE *tri)
{
	for(int i = 0; i < ntri;i++)
	{
		tri[i].b1 = FindBorder(ntri,i,tri,tri[i].p1,tri[i].p2);
		tri[i].b2 = FindBorder(ntri,i,tri,tri[i].p1,tri[i].p3);
		tri[i].b3 = FindBorder(ntri,i,tri,tri[i].p2,tri[i].p3);
	}
}

int CTin::FindBorder(int ntri,int curIndex,ITRIANGLE *tri,int v1,int v2)
{
	int count=0;
	for(int i =0; i < ntri; i++)
	{
		count = 0;
		if(i != curIndex)
		{
			if(tri[i].p1 == v1)
				count++;
			else if(tri[i].p2 == v1)
				count++;
			else if(tri[i].p3 == v1)
				count++;

			if(tri[i].p1 == v2)
				count++;
			else if(tri[i].p2 == v2)
				count++;
			else if(tri[i].p3 == v2)
				count++;

			if(count == 2)
				return i;
		}
    }

	return -1;
}

void CTin::Prune(int &nv,XYZ v[])
{
	XYZ *pv = new XYZ[nv];
	int count=0;

	//removes duplicate vertices
	//the array must be sorted before prune is called
	for(int i =0; i < nv;i++)
	{
		if(i != nv && v[i].x == v[i+1].x && v[i].y == v[i+1].y)
		{
			//ignore
		}
		else
		{
			pv[count] = v[i];
			count++;
		}
	}

	//copy the prune array back over to the old
	for(int i=0; i < nv;i++)
	{
		if( i < count)
		{
			v[i].x = pv[i].x;
			v[i].y = pv[i].y;
			v[i].z = pv[i].z;
		}
		else
		{
			v[i].x = NULL;
			v[i].y = NULL;
			v[i].z = NULL;
		}
	}
	
	nv = count;
	delete[] pv;
}

void CTin::BuildTin(int nv, XYZ v[],int ntri,ITRIANGLE *tri,XYZ vMax,XYZ vMin)
{
	//clear the tin tables
	_triTable.clear();
	_vtxTable.clear();

	//Bounding Box	
	_min = vertex(vMin.x,vMin.y,vMin.z);
	_max = vertex(vMax.x,vMax.y,vMax.z);

	//Triangles
	long vtx1 ;
	long vtx2 ;
	long vtx3 ;
	long b1 ;
	long b2 ;
	long b3 ;

	for(long i = 0; i < ntri; i++ )
	{	
		vtx1 = tri[i].p1;
		vtx2 = tri[i].p2;
		vtx3 = tri[i].p3;
		b1 = tri[i].b1;
		b2 = tri[i].b2;
		b3 = tri[i].b3;

		tinTableRow r( vtx1, vtx2, vtx3, b1, b2, b3 );
		_triTable.addRow( r );
	}

	//Vertices
	double x,y,z;
	vertex ver;
	for(long j = 0; j < nv; j++ )
	{
		x = v[j].x;
		y = v[j].y;
		z = v[j].z;
		
		ver = vertex( x, y, z );
		_vtxTable.add( ver );
	}

	_dTriangles = new char[ntri];
	memset(_dTriangles,0,sizeof(char)*ntri);

}




