/**************************************************************************************
 * File name: ShapeNetwork.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of CShapeNetwork
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
#include "ShapeNetwork.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CShapeNetwork

bool dPOINT_EQUAL( void * one, void * two, void * exParam )
{	double Tolerance = *((double*)exParam);
	dPOINT * d1 = (dPOINT*)one;
	dPOINT * d2 = (dPOINT*)two;

	if( fabs( d1->x - d2->x ) <= Tolerance )
	{	if( fabs( d1->y - d2->y ) <= Tolerance )
			return true;
	}
	return false;
}

void dPRINT_DATA( ofstream & out, void * data )
{	out<<((dPOINT*)data)->x<<" "<<((dPOINT*)data)->y;
}

long CShapeNetwork::UpEnd( edge * e, dPOINT * downpoint, double tolerance )
{	
	if( dPOINT_EQUAL( e->one->data, (void *)downpoint, (void*)&tolerance ) )
		return e->twoIndex;
	else
		return e->oneIndex;
}

long CShapeNetwork::DownEnd( edge * e, dPOINT * downpoint, double tolerance )
{	
	if( dPOINT_EQUAL( e->one->data, (void *)downpoint, (void*)&tolerance ) )
		return e->oneIndex;
	else
		return e->twoIndex;
}

void CShapeNetwork::CopyShape( bool reversePoints, IShape * oldshape, IShape * newshape )
{	
	ShpfileType shptype;
	VARIANT_BOOL vbretval;
	oldshape->get_ShapeType(&shptype);
	newshape->Create(shptype,&vbretval);
	long pIndex = 0;
	newshape->InsertPart(0,&pIndex,&vbretval);
	
	long numPoints = 0;
	oldshape->get_NumPoints(&numPoints);
	
	IPoint * oldpnt = NULL;
	IPoint * newpnt = NULL;
	double x, y, z;
	long pos = 0;
	VARIANT_BOOL retval = FALSE;
	for( int i = 0; i < numPoints; i++ )
	{	
		pos = i;
		oldshape->get_Point(pos,&oldpnt);
		oldpnt->get_X(&x);
		oldpnt->get_Y(&y);
		oldpnt->get_Z(&z);
		oldpnt->Release();
		oldpnt = NULL;

		CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&newpnt);		
		newpnt->put_X(x);
		newpnt->put_Y(y);
		newpnt->put_Z(z);
		if( reversePoints == true )
			pos = 0;
		newshape->InsertPoint(newpnt,&pos,&retval);		
		newpnt->Release();	
		newpnt = NULL;
	}
}

void CShapeNetwork::CopyField( IField * oldfield, IField * newfield )
{	BSTR name;
	long precision = 0,width = 0;
	FieldType ftype;
	oldfield->get_Name(&name);
	oldfield->get_Precision(&precision);
	oldfield->get_Type(&ftype);
	oldfield->get_Width(&width);
	newfield->put_Name(name);
	newfield->put_Precision(precision);
	newfield->put_Type(ftype);
	newfield->put_Width(width);
	::SysFreeString(name);
}

void recPrintShpNetwork( shpNetNode * allnodes, long index, ofstream & out )
{	out<<index<<" : ";
	int i = 0;
	for( i = 0; i < (int)allnodes[index].up.size(); i++ )
		out<<allnodes[index].up[i]<<" ";
	out<<endl;
	
	for( i = 0; i < (int)allnodes[index].up.size(); i++ )
		recPrintShpNetwork( allnodes, allnodes[index].up[i], out );
}

void PrintShpNetwork( shpNetNode * allnodes, long outlet, const char * filename )
{	
	ofstream out(filename);
	recPrintShpNetwork( allnodes, outlet, out );
	out.close();
}

STDMETHODIMP CShapeNetwork::Build(IShapefile *Shapefile, long ShapeIndex, long FinalPointIndex, double Tolerance, AmbiguityResolution ar, ICallback *cBack, long *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//Verify the Shapefile
	if( Shapefile == NULL )
	{	*retval = 0;
		lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}

	//Verify the Shapefile Type
	ShpfileType shpfiletype;
	Shapefile->get_ShapefileType(&shpfiletype);
	if( shpfiletype != SHP_POLYLINE && shpfiletype != SHP_POLYLINEZ && shpfiletype != SHP_POLYLINEM )
	{	*retval = 0;
		lastErrorCode = tkINCOMPATIBLE_SHAPEFILE_TYPE;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}

	long numShapes = 0;
	Shapefile->get_NumShapes(&numShapes);

	//Verify the ShapeIndex
	if( ShapeIndex < 0 || ShapeIndex >= numShapes )
	{	*retval = 0;
		lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}

	//Verify the PointIndex
	IShape * outshp = NULL;
	long numPoints = 0;
	Shapefile->get_Shape(ShapeIndex,&outshp);	
	outshp->get_ShapeType(&shpfiletype);
	outshp->get_NumPoints(&numPoints);
	outshp->Release();
	outshp = NULL;

	if( shpfiletype != SHP_POLYLINE && shpfiletype != SHP_POLYLINEZ && shpfiletype != SHP_POLYLINEM )
	{	*retval = 0;
		lastErrorCode = tkINCOMPATIBLE_SHAPE_TYPE;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}
	if( FinalPointIndex != 0 && FinalPointIndex != numPoints - 1 )
	{	*retval = 0;
		lastErrorCode = tkINVALID_FINAL_POINT_INDEX;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}

	graph undir_graph;	

	long percent = 0, newpercent = 0;
	double total = numShapes;

	double x1, y1, x2, y2;
	//Build the Undirected Graph
	for( int i = 0; i < numShapes; i++ )
	{	IShape * shape = NULL;
		Shapefile->get_Shape(i,&shape);
		shape->get_ShapeType(&shpfiletype);

		if( shpfiletype == SHP_POLYLINE || shpfiletype == SHP_POLYLINEZ || shpfiletype == SHP_POLYLINEM )
		{	IPoint * pnt1 = NULL, * pnt2 = NULL;

			shape->get_NumPoints(&numPoints);
			shape->get_Point(0,&pnt1);
			shape->get_Point(numPoints-1,&pnt2);

			pnt1->get_X(&x1);
			pnt1->get_Y(&y1);

			pnt2->get_X(&x2);
			pnt2->get_Y(&y2);

			pnt1->Release();
			pnt2->Release();
			
			dPOINT * p1 = new dPOINT;
			dPOINT * p2 = new dPOINT;
			p1->x = x1;
			p1->y = y1;
			p2->x = x2;
			p2->y = y2;

			double distance = sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );

			if( distance > Tolerance )
			{
				graphnode * gnOne = new graphnode();
				graphnode * gnTwo = new graphnode();		
				gnOne->data = (void*)p1;
				gnTwo->data = (void*)p2;

				edge * newEdge = new edge();
				newEdge->one = gnOne;
				newEdge->two = gnTwo;
				
				undir_graph.Insert(newEdge,(void*)&Tolerance,dPOINT_EQUAL);
			}
			else
			{	
				//Add the shape as a blank to the graph
				undir_graph.InsertBlank();

				//Verify that this isn't the outlet shape
				if( i == ShapeIndex )
				{	
					shape->Release();
					shape = NULL;	
					*retval = 0;			
					lastErrorCode = tkTOLERANCE_TOO_LARGE;
					if( cBack != NULL )
						cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
					else if( globalCallback != NULL )
						globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
					return S_OK;			
				}
			}
		}
		else
		{	
			//Verify that this isn't the outlet shape
			if( i == ShapeIndex )
			{	shape->Release();
				shape = NULL;
				*retval = 0;			
				lastErrorCode = tkINCOMPATIBLE_SHAPE_TYPE;
				if( cBack != NULL )
					cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
				else if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				return S_OK;
			}
			else
				undir_graph.InsertBlank();
		}

		shape->Release();
		shape = NULL;
	
		newpercent = (long)((i/total)*100);
		if( newpercent > percent )
		{	percent = newpercent;
			if( cBack != NULL )
				cBack->Progress(OLE2BSTR(key),percent,A2BSTR("Building ShapeNetwork"));
			else if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Building ShapeNetwork"));
		}
	}

	//undir_graph.Save("graph.txt",dPRINT_DATA);
	
	//Get the info on the outlet node
	double length = 0;
	double fx, fy;
	dPOINT dpnt;
	IShape * shape = NULL;
	IPoint * pnt = NULL;
	IUtils * gen = NULL;
	
	CoCreateInstance(CLSID_Utils,NULL,CLSCTX_INPROC_SERVER,IID_IUtils,(void**)&gen);	
	for( int ls = 0; ls < numShapes; ls++ )
	{	if( undir_graph.edges[ls] != NULL )
		{	Shapefile->get_Shape(ls,&shape);		
			gen->get_Length(shape,&length);

			if( length <= Tolerance )
			{	gen->Release();
				gen = NULL;
				shape->Release();
				shape = NULL;
				
				*retval = 0;			
				lastErrorCode = tkTOLERANCE_TOO_LARGE;
				if( cBack != NULL )
					cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
				else if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				return S_OK;
			}

			undir_graph.edges[ls]->length = length;
			if( ShapeIndex == ls )
			{	shape->get_Point(FinalPointIndex,&pnt);	
				pnt->get_X(&fx);
				pnt->get_Y(&fy);
				dpnt.x = fx;
				dpnt.y = fy;	
				pnt->Release();
				pnt = NULL;
			}

			shape->Release();
			shape = NULL;	
		}			
	}
	gen->Release();
	gen = NULL;

	//convert the undirected graph into a directed acyclic graph	
	shpNetNode * edgeNetwork = new shpNetNode[numShapes];

	//Find the up end of the outlet	
	long up = UpEnd( undir_graph.edges[ShapeIndex], &dpnt, Tolerance );
	long down = DownEnd( undir_graph.edges[ShapeIndex], &dpnt, Tolerance );		
	edgeNetwork[ShapeIndex].downIndex = down;
	edgeNetwork[ShapeIndex].upIndex = up;
	edgeNetwork[ShapeIndex].distanceToOutlet = 0;

	std::deque<long> tmp_ambigShapeIndex;
	
	heap minheap;
	heapnode hn;
	long parentIndex;
	double distance = 0;
	
	minheap.insert(ShapeIndex,0);
 	while( minheap.size() > 0 )
	{	
		hn = minheap.top();
		minheap.pop();
		parentIndex = hn.index;

		if( edgeNetwork[parentIndex].used == false )
		{
			edge * e = undir_graph.edges[parentIndex];

			networkSize++;
			edgeNetwork[parentIndex].used = true;
			edgeNetwork[parentIndex].length = e->length;		

			//Set up the parent
			if( edgeNetwork[parentIndex].pbIndex.size() > 0 )
			{	
				long parentparent = edgeNetwork[parentIndex].pbIndex[0];
				edgeNetwork[parentIndex].parentIndex = parentparent;

				if( parentparent >= 0 )
					edgeNetwork[parentparent].up.push_back(parentIndex);
				
				//Mark an ambiguous path
				if( edgeNetwork[parentIndex].pbIndex.size() > 1 )
					tmp_ambigShapeIndex.push_back(parentIndex);				
			}

			up = edgeNetwork[parentIndex].upIndex;
			down = edgeNetwork[parentIndex].downIndex;			
			
			distance = edgeNetwork[parentIndex].distanceToOutlet + e->length;				
			graphnode * gn = undir_graph.graphnodes[up];

			for( int i = 0; i < (int)gn->edges.size(); i++ )
			{	
				long childIndex = gn->edges[i];

				if( edgeNetwork[childIndex].used == false )
				{
					if( edgeNetwork[childIndex].distanceToOutlet < 0 ||
						distance < edgeNetwork[childIndex].distanceToOutlet )
					{
						up = UpEnd( undir_graph.edges[childIndex],(dPOINT*)gn->data,Tolerance );
						down = DownEnd( undir_graph.edges[childIndex],(dPOINT*)gn->data,Tolerance );

						edgeNetwork[childIndex].pbIndex.clear();
						edgeNetwork[childIndex].upIndex = up;
						edgeNetwork[childIndex].downIndex = down;
						edgeNetwork[childIndex].pbIndex.push_back(parentIndex);
						edgeNetwork[childIndex].distanceToOutlet = distance;
						minheap.insert(childIndex,distance);
					}
					//Ambiguous Path
					else if( distance == edgeNetwork[gn->edges[i]].distanceToOutlet )
						edgeNetwork[childIndex].pbIndex.push_back(parentIndex);					
				}
			}
		}
	}
	
	//CString ccnt;
	//ccnt.Format("%i",networkSize);
	//AfxMessageBox(ccnt);

	//PrintShpNetwork(edgeNetwork,ShapeIndex,"Network.txt");

	//copy the network into a new shapefile
	if( network != NULL )
		delete [] network;
	network = new shpNetNode[networkSize];
	long * shapeMap = new long[numShapes];
	long shapepos = 0;
	VARIANT_BOOL vbretval = FALSE;
	
	IShape * newshape = NULL;
	IShape * oldshape = NULL;

	//_unlink("d:\\data\\netfile.shp");
	//_unlink("d:\\data\\netfile.shx");
	//_unlink("d:\\data\\netfile.dbf");

	if( netshpfile != NULL )
		netshpfile->Release();
	netshpfile = NULL;
	//Create the shapefile
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)&netshpfile);
	Shapefile->get_ShapefileType(&shpfiletype);
	netshpfile->CreateNew(A2BSTR(""),shpfiletype,&vbretval);

	//Copy all of the Fields
	long numFields = 0;
	Shapefile->get_NumFields(&numFields);
	IField * newfield = NULL;	
	IField * oldfield = NULL;
	long fieldpos =0;
	int f = 0;
	for( f = 0; f < numFields; f++ )
	{	CoCreateInstance(CLSID_Field,NULL,CLSCTX_INPROC_SERVER,IID_IField,(void**)&newfield);
		Shapefile->get_Field(f,&oldfield);
		CopyField(oldfield,newfield);
		fieldpos = f;
		netshpfile->EditInsertField(newfield,&fieldpos,cBack,&vbretval);
		oldfield->Release();
		oldfield = NULL;
		newfield->Release();
		newfield = NULL;
	}

	//Create two new fields
	IField * id = NULL;
	IField * did = NULL;
	CoCreateInstance(CLSID_Field,NULL,CLSCTX_INPROC_SERVER,IID_IField,(void**)&id);
	CoCreateInstance(CLSID_Field,NULL,CLSCTX_INPROC_SERVER,IID_IField,(void**)&did);
				
	id->put_Name(A2BSTR("NET_ID"));
	id->put_Precision(0);
	id->put_Type(INTEGER_FIELD);
	id->put_Width(5);

	did->put_Name(A2BSTR("NET_DID"));
	did->put_Precision(0);
	did->put_Type(INTEGER_FIELD);
	did->put_Width(5);

	fieldpos = 0;
	netshpfile->EditInsertField(did,&fieldpos,cBack,&vbretval);
	netshpfile->EditInsertField(id,&fieldpos,cBack,&vbretval);
	id->Release();
	id = NULL;
	did->Release();
	did = NULL;

	//Create the outlet shape
	CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&newshape);
	Shapefile->get_Shape(ShapeIndex,&oldshape);	

	dPOINT zeroPnt;
	IPoint * izpnt = NULL;
	oldshape->get_Point(0,&izpnt);
	double izx,izy;
	izpnt->get_X(&izx);
	izpnt->get_Y(&izy);
	izpnt->Release();
	izpnt = NULL;
	zeroPnt.x = izx;
	zeroPnt.y = izy;

	bool reversePoints = false;
	if( FinalPointIndex != 0 )
		reversePoints = true;
	else
		reversePoints = false;

	CopyShape(reversePoints,oldshape,newshape);	
	netshpfile->EditInsertShape(newshape,&shapepos,&vbretval);
	oldshape->Release();
	oldshape = NULL;
	newshape->Release();
	newshape = NULL;

	VARIANT cID,cDID;
	VariantInit(&cID); //added by Rob Cairns 4-Jan-06
	VariantInit(&cDID); //added by Rob Cairns 4-Jan-06
	cID.vt = VT_I4;
	cDID.vt = VT_I4;

	cID.lVal = shapepos;
	cDID.lVal = -1;
	netshpfile->EditCellValue(0,shapepos,cID,&vbretval);
	netshpfile->EditCellValue(1,shapepos,cDID,&vbretval);

	
	VARIANT cVal;
	VariantInit(&cVal); //added by Rob Cairns 4-Jan-06
	for( f = 0; f < numFields; f++ )
	{	Shapefile->get_CellValue(f,ShapeIndex,&cVal);
		//Adjust for the two new fields
		netshpfile->EditCellValue(f + 2,shapepos,cVal,&vbretval);
	}
		
	shapeMap[ShapeIndex] = shapepos;	

	network[shapepos].distanceToOutlet = 0;
	network[shapepos].parentIndex = -1;
	network[shapepos].length = edgeNetwork[ShapeIndex].length;
	
	std::deque<long> netPath;
	netPath.push_back(ShapeIndex);
	
	percent = 0, newpercent = 0;
	total = networkSize;
	
	//add all other shapes by simulating a recursive search
	long shpcnt = 1;
	long insertedShapes = 1;
	while( netPath.size() > 0 )
	{			
		long parent = netPath[0];
		netPath.pop_front();
		
		shpcnt++;
		
		for( int i = 0; i < (int)edgeNetwork[parent].up.size(); i++ )
		{	
			shapepos = insertedShapes;

			long childIndex = edgeNetwork[parent].up[i];
			netPath.push_back(childIndex);			

			CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&newshape);
			Shapefile->get_Shape(childIndex,&oldshape);				
			
			oldshape->get_Point(0,&izpnt);
			izpnt->get_X(&izx);
			izpnt->get_Y(&izy);
			izpnt->Release();
			izpnt = NULL;
			zeroPnt.x = izx;
			zeroPnt.y = izy;

			IShape * parentShape = NULL;
			Shapefile->get_Shape(parent,&parentShape);
			long pnumPts = 0;
			parentShape->get_NumPoints(&pnumPts);			
			IPoint * parentPnt1 = NULL;
			IPoint * parentPnt2 = NULL;
			parentShape->get_Point(0,&parentPnt1);
			parentShape->get_Point(pnumPts-1,&parentPnt2);
			double pp1x, pp1y, pp2x, pp2y;
			parentPnt1->get_X(&pp1x);
			parentPnt1->get_Y(&pp1y);
			parentPnt2->get_X(&pp2x);
			parentPnt2->get_Y(&pp2y);
			parentPnt1->Release();
			parentPnt1 = NULL;
			parentPnt2->Release();
			parentPnt2 = NULL;
			parentShape->Release();
			parentShape = NULL;

			dPOINT dpp1;
			dpp1.x = pp1x;
			dpp1.y = pp1y;
			dPOINT dpp2;
			dpp2.x = pp2x;
			dpp2.y = pp2y;

			if( dPOINT_EQUAL((void*)&dpp1,(void*)&zeroPnt,(void*)&Tolerance) != FALSE ||
				dPOINT_EQUAL((void*)&dpp2,(void*)&zeroPnt,(void*)&Tolerance) != FALSE )
				reversePoints = false;
			else
				reversePoints = true;

			CopyShape(reversePoints,oldshape,newshape);				
			netshpfile->EditInsertShape(newshape,&shapepos,&vbretval);
			oldshape->Release();
			oldshape = NULL;
			newshape->Release();
			newshape = NULL;

			cID.lVal = shapepos;
			cDID.lVal = shapeMap[parent];
			netshpfile->EditCellValue(0,shapepos,cID,&vbretval);
			netshpfile->EditCellValue(1,shapepos,cDID,&vbretval);

			for( f = 0; f < numFields; f++ )
			{	Shapefile->get_CellValue(f,childIndex,&cVal);
				//Adjust for the two new fields
				netshpfile->EditCellValue(f + 2,shapepos,cVal,&vbretval);
			}
		
			distance = edgeNetwork[parent].distanceToOutlet + undir_graph.edges[parent]->length;							

			shapeMap[childIndex] = shapepos;
			network[shapeMap[parent]].up.push_back(shapepos);
			network[shapepos].distanceToOutlet = distance;
			network[shapepos].parentIndex = shapeMap[parent];
			network[shapepos].length = edgeNetwork[childIndex].length;

			insertedShapes++;			
		}

		newpercent = (long)((shpcnt/total)*100);
		if( newpercent > percent )
		{	percent = newpercent;
			if( cBack != NULL )
				cBack->Progress(OLE2BSTR(key),percent,A2BSTR("ShpNetwork::Copying Shapefile"));
			else if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("ShpNetwork::Copying Shapefile"));
		}
	}
	currentNode = 0;

	//ShapeMap the Ambiguous Shapes
	ambigShapeIndex.clear();
	for( int ti = 0; ti < (int)tmp_ambigShapeIndex.size(); ti++ )
	{	long shpindex = tmp_ambigShapeIndex[ti];
		ambigShapeIndex.push_back(shapeMap[shpindex]);
	}
		
	delete [] shapeMap;
	delete [] edgeNetwork;

	netshpfile->AddRef();
	*retval = ambigShapeIndex.size()+1; //ARA 02/03/06 Possible to have size() of 0 so needed to be incremented by 1 so it wasn't returning error code
	
	VariantClear(&cID); //added by Rob Cairns 4-Jan-06
	VariantClear(&cDID); //added by Rob Cairns 4-Jan-06
	VariantClear(&cVal); //added by Rob Cairns 4-Jan-06

	return S_OK;
}

STDMETHODIMP CShapeNetwork::DeleteShape(long ShapeIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*retval = FALSE;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( ShapeIndex >= 0 && ShapeIndex < networkSize )
	{	
		VARIANT_BOOL vbretval = FALSE;
		//Verify that the shapefile and dbf are in editing mode
		netshpfile->get_EditingShapes(&vbretval);
		if( vbretval == FALSE )
		{	*retval = FALSE;
			lastErrorCode = tkSHPFILE_NOT_IN_EDIT_MODE;
			if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
			return S_OK;
		}
		netshpfile->get_EditingTable(&vbretval);
		if( vbretval == FALSE )
		{	*retval = FALSE;
			lastErrorCode = tkDBF_NOT_IN_EDIT_MODE;
			if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));						
			return S_OK;
		}

		//Mark all nodes as not used
		for( int n = 0; n < networkSize; n++ )
			network[n].used = false;
		
		//Find the path along the nodes
		std::deque<long> netPath;
		netPath.push_back(ShapeIndex);
		long localNetSize = 0;
		while( netPath.size() > 0 )
		{			
			long parent = netPath[0];
			netPath.pop_front();

			localNetSize++;
			network[parent].used = true;

			for( int i = 0; i < (int)network[parent].up.size(); i++ )
			{	long index = network[parent].up[i];
				netPath.push_back(index);
			}
		}

		if( networkSize == localNetSize )
		{	Close(retval);
			*retval = TRUE;
			return S_OK;
		}

		//Recopy the network
		long * shifts = new long[networkSize];
		memset(shifts,0,sizeof(long)*networkSize);

		long delcnt = 0;
		int m = 0;
		for( m = 0; m < networkSize; m++ )
		{	if( network[m].used == true )
			{	netshpfile->EditDeleteShape(m - delcnt,&vbretval);					
				++delcnt;			

				for( int k = m + 1; k < networkSize; k++ )
					shifts[k]++;				
			}			
		}

		//Change any indexes in the old network			
		for( m = 0; m < networkSize; m++ )
		{	long parentIndex = network[m].parentIndex;
			if( parentIndex >= 0 )
				network[m].parentIndex -= shifts[parentIndex];
					
			int kp = 0;
			for( kp = 0; kp < (int)network[m].up.size(); kp++ )
			{	long childIndex = network[m].up[kp];
				if( network[childIndex].used == true )
					network[m].up.erase( network[m].up.begin() + kp );				
			}

			for( kp = 0; kp < (int)network[m].up.size(); kp++ )
				network[m].up[kp] -= shifts[network[m].up[kp]];
		}
		
		delete [] shifts;
		shifts = NULL;

		shpNetNode * newNetwork = new shpNetNode[networkSize - localNetSize];				
		long indcnt = 0;
		for( m = 0; m < networkSize; m++ )
		{	if( network[m].used == false )			
				newNetwork[indcnt++] = network[m];
		}
		
		delete [] network;
		network = newNetwork;
		networkSize = networkSize - localNetSize;

		//Reset the parents and id's in the dbf
		VARIANT cID, cDID;
		VariantInit(&cID); //added by Rob Cairns 4-Jan-06
		VariantInit(&cDID); //added by Rob Cairns 4-Jan-06
		cID.vt = VT_I4;
		cDID.vt = VT_I4;
		for( int fn = 0; fn < networkSize; fn++ )
		{	cID.lVal = fn;
			cDID.lVal = network[fn].parentIndex;
			netshpfile->EditCellValue(0,fn,cID,&vbretval);
			netshpfile->EditCellValue(1,fn,cDID,&vbretval);
		}		
		
		VariantClear(&cID); //added by Rob Cairns 4-Jan-06
		VariantClear(&cDID); //added by Rob Cairns 4-Jan-06
		
		*retval = TRUE;
	}
	else
	{	*retval = FALSE;
		lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));						
		return S_OK;			
	}
	

	return S_OK;
}

STDMETHODIMP CShapeNetwork::MoveUp(long UpIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*retval = FALSE;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( currentNode >= 0 )
	{	long numUps = network[currentNode].up.size();
		if( UpIndex >= 0 && UpIndex < numUps )
		{	currentNode = network[currentNode].up[UpIndex];
			*retval = TRUE;
		}
		else
		{	*retval = FALSE;
			lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
			if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));									
			return S_OK;
		}
	}
	else
	{	*retval = FALSE;
		lastErrorCode = tkINVALID_NODE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));									
		return S_OK;
	}	

	return S_OK;
}

STDMETHODIMP CShapeNetwork::MoveDown(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*retval = FALSE;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( currentNode >= 0 )
	{	
		if( network[currentNode].parentIndex >= 0 )
		{	currentNode = network[currentNode].parentIndex;
			*retval = TRUE;
		}
		else
		{	*retval = FALSE;
			lastErrorCode = tkNODE_AT_OUTLET;
			if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));									
			return S_OK;
		}
	}
	else
	{	*retval = FALSE;
		lastErrorCode = tkINVALID_NODE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));									
		return S_OK;
	}	

	return S_OK;
}

STDMETHODIMP CShapeNetwork::MoveTo(long ShapeIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( IsAligned() == false )
	{	*retval = FALSE;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( ShapeIndex >= 0 && ShapeIndex < networkSize )
	{	currentNode = ShapeIndex;
		*retval = TRUE;
	}
	else
	{	*retval = FALSE;
		lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CShapeNetwork::MoveToOutlet(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*retval = FALSE;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( networkSize > 0 )
	{	currentNode = 0;
		*retval = TRUE;
	}
	else
	{	*retval = FALSE;
		lastErrorCode = tkINVALID_NODE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_Shapefile(IShapefile **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*pVal = NULL;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( netshpfile != NULL )
		netshpfile->AddRef();

	*pVal = netshpfile;	
	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_CurrentShape(IShape **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*pVal = NULL;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( currentNode >= 0 )
	{	if( netshpfile != NULL )
			netshpfile->get_Shape(currentNode,pVal);
		else
		{	*pVal = NULL;
			lastErrorCode = tkNO_NETWORK;
			if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
			return S_OK;
		}
	}
	else
	{	*pVal = NULL;
		lastErrorCode = tkINVALID_NODE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_CurrentShapeIndex(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*pVal = -1;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( currentNode >= 0 )
		*pVal = currentNode;
	else
	{	*pVal = -1;	
		lastErrorCode = tkINVALID_NODE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_DistanceToOutlet(long PointIndex, double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*pVal = 0.0;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( currentNode >= 0 )
	{	double distance = 0;
		
		IShape * shp = NULL;
		netshpfile->get_Shape(currentNode,&shp);
		long numPoints = 0;
		shp->get_NumPoints(&numPoints);

		if( PointIndex < 0 || PointIndex >= numPoints )
		{	*pVal = 0;
			lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
			if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));						
			return S_OK;
		}
		else
		{	distance = network[currentNode].distanceToOutlet;			
			IPoint * pnt = NULL;
			double x1, y1, z1;
			double x2, y2, z2;

			shp->get_Point(0,&pnt);
			pnt->get_X(&x1);
			pnt->get_Y(&y1);
			pnt->get_Z(&z1);
			pnt->Release();
			pnt = NULL;

			for( int i = 1; i < PointIndex; i++ )
			{	
				shp->get_Point(i,&pnt);
				pnt->get_X(&x2);
				pnt->get_Y(&y2);
				pnt->get_Z(&z2);
				pnt->Release();
				pnt = NULL;

				distance += sqrt( pow( x2 - x1, 2 ) + pow( y2 - y1, 2 ) + pow( z2 - z1, 2 ) );

				x1 = x2;
				y1 = y2;
				z1 = z2;
			}
		}
		
		shp->Release();
		shp = NULL;

		*pVal = distance;
	}
	else
	{	*pVal = 0.0;		
		lastErrorCode = tkINVALID_NODE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_NumDirectUps(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( IsAligned() == false )
	{	*pVal = 0;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( currentNode >= 0 )
		*pVal = network[currentNode].up.size();
	else
	{	*pVal = 0;
		lastErrorCode = tkINVALID_NODE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}	

	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_NetworkSize(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*pVal = 0;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( currentNode >= 0 )
	{
		std::deque<long> netPath;
		netPath.push_back(currentNode);

		long localNetSize = 0;
		while( netPath.size() > 0 )
		{			
			long parent = netPath[0];
			netPath.pop_front();
			localNetSize++;

			for( int i = 0; i < (int)network[parent].up.size(); i++ )
			{	long index = network[parent].up[i];
				netPath.push_back(index);
			}
		}
		*pVal = localNetSize;
	}
	else
	{	*pVal = 0;
		lastErrorCode = tkINVALID_NODE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_AmbigShapeIndex(long Index, long * pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*pVal = -1;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( Index >= 0 && Index < (long)ambigShapeIndex.size() )
		*pVal = ambigShapeIndex[Index];
	else
	{	*pVal = -1;
		lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*pVal = lastErrorCode;
	lastErrorCode = tkNO_ERROR;
	
	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*pVal = globalCallback;
	if( globalCallback != NULL )
	{	
		globalCallback->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CShapeNetwork::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&globalCallback);
	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	*pVal = OLE2BSTR(key);

	return S_OK;
}

STDMETHODIMP CShapeNetwork::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	::SysFreeString(key);
	key = OLE2BSTR(newVal);

	return S_OK;
}

STDMETHODIMP CShapeNetwork::get_ParentIndex(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( IsAligned() == false )
	{	*pVal = -2;
		lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( currentNode >= 0 )
		*pVal = network[currentNode].parentIndex;	
	else
	{	*pVal = -2;
		lastErrorCode = tkINVALID_NODE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CShapeNetwork::put_ParentIndex(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())	

	if( IsAligned() == false )
	{	lastErrorCode = tkNOT_ALIGNED;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	if( currentNode == 0 )
	{	lastErrorCode = tkCANT_CHANGE_OUTLET_PARENT;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}
	else if(currentNode > 0 )
	{	if( newVal >= 0 && newVal < networkSize && newVal != currentNode )
		{	
			//Verify that this shape can get to the outlet
			for( shpNetNode snn = network[newVal]; snn.parentIndex > 0; snn = network[snn.parentIndex] )
			{	if( snn.parentIndex == currentNode )
				{	//There would create a loop
					lastErrorCode = tkNET_LOOP;
					if( globalCallback != NULL )
						globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
					return S_OK;
				}
			}

			//Possibly flip the shape so that Point0 is down
			IShape * parentShape = NULL;
			netshpfile->get_Shape(newVal,&parentShape);

			long numPoints = 0;
			parentShape->get_NumPoints(&numPoints);
			if( numPoints < 2 )
			{	lastErrorCode = tkINVALID_SHP_FILE;
				if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				return S_OK;
			}
			
			IPoint * ppnt = NULL;
			parentShape->get_Point(numPoints-1,&ppnt);
			double ppx, ppy;
			ppnt->get_X(&ppx);
			ppnt->get_Y(&ppy);
			ppnt->Release();
			ppnt = NULL;
			parentShape->Release();
			parentShape = NULL;

			IShape * shp = NULL;
			netshpfile->get_Shape(currentNode,&shp);
			shp->get_NumPoints(&numPoints);
			if( numPoints < 2 )
			{	lastErrorCode = tkINVALID_SHP_FILE;
				if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				return S_OK;
			}

			IPoint * pnt1 = NULL, * pnt2 = NULL;
			shp->get_Point(0,&pnt1);
			shp->get_Point(numPoints-1,&pnt2);
			double px1, px2, py1, py2;
			pnt1->get_X(&px1);
			pnt1->get_Y(&py1);
			pnt2->get_X(&px2);
			pnt2->get_Y(&py2);
			shp->Release();
			shp = NULL;
			pnt1->Release();
			pnt1 = NULL;
			pnt2->Release();
			pnt2 = NULL;

			double distance1 = sqrt( fabs( pow( px1 - ppx, 2 ) + pow( py1 - ppy, 2 ) ) );
			double distance2 = sqrt( fabs( pow( px2 - ppx, 2 ) + pow( py2 - ppy, 2 ) ) );
				
			VARIANT_BOOL retval = FALSE;
			netshpfile->get_EditingShapes(&retval);
			if( retval == FALSE )
			{	lastErrorCode = tkSHPFILE_NOT_IN_EDIT_MODE;
				if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				return S_OK;
			}

			netshpfile->get_EditingTable(&retval);
			if( retval == FALSE )
			{	lastErrorCode = tkDBF_NOT_IN_EDIT_MODE;
				if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				return S_OK;
			}

			//Reverse the points
			if( distance2 < distance1 )
			{	netshpfile->get_Shape(currentNode,&shp);
				shp->get_NumPoints(&numPoints);			
				IPoint * pnt = NULL;
				long pntcnt = numPoints;
				int i = 0;
				for( i = numPoints - 1; i >= 0; i-- )
				{	shp->get_Point(i,&pnt);
					long tmppntcnt = pntcnt;
					shp->InsertPoint(pnt,&tmppntcnt,&retval);
					pnt->Release();
					pnt = NULL;
					pntcnt++;
				}
				for( i = 0; i < numPoints; i++ )
					shp->DeletePoint(0,&retval);
				//Rob Cairns 20-Dec-05
				shp->Release();
			}

			retval = FALSE;
			VARIANT vnv;
			VariantInit(&vnv); //added by Rob Cairns 4-Jan-06
			vnv.vt = VT_I4;
			vnv.lVal = newVal;
			netshpfile->EditCellValue(1,currentNode,vnv,&retval);

			network[currentNode].parentIndex = newVal;
			network[newVal].up.push_back(currentNode);

			//Push all of the nodes up's onto the parent
			for( int i = 0; i < (int)network[currentNode].up.size(); i++ )
			{	long childIndex = network[currentNode].up[i];
				netshpfile->EditCellValue(1,childIndex,vnv,&retval);

				network[childIndex].parentIndex = newVal;
				network[newVal].up.push_back(childIndex);
			}
			network[currentNode].up.clear();
			VariantClear(&vnv); //added by Rob Cairns 4-Jan-06
		}
		else
		{	if( newVal == currentNode )
			{	lastErrorCode = tkNET_LOOP;
				if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				return S_OK;
			}
			else
			{	lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
				if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				return S_OK;
			}
		}
	}
	else
	{	lastErrorCode = tkINVALID_NODE;
		if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;	
	}

	return S_OK;
}

STDMETHODIMP CShapeNetwork::Open(IShapefile *sf, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( sf == NULL )
	{	*retval = FALSE;
		lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}
	
	//Verify the number of shapes
	long numShapes = 0;
	sf->get_NumShapes(&numShapes);
	if( numShapes <= 0 )
	{	*retval = FALSE;
		lastErrorCode = tkINVALID_SHP_FILE;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	//Verify the first two fields
	long numFields = 0;
	sf->get_NumFields(&numFields);
	if( numFields < 2 )
	{	*retval = FALSE;
		lastErrorCode = tkMISSING_FIELD;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}	
	FieldType foneType;
	FieldType ftwoType;
	IField * fone = NULL;
	IField * ftwo = NULL;
	sf->get_Field(0,&fone);
	sf->get_Field(1,&ftwo);
	fone->get_Type(&foneType);
	ftwo->get_Type(&ftwoType);
	fone->Release();
	ftwo->Release();
	fone = NULL;
	ftwo = NULL;
	if( foneType != INTEGER_FIELD || ftwoType != INTEGER_FIELD )
	{	*retval = FALSE;
		lastErrorCode = tkINVALID_FIELD;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;
	}

	Close(retval);
	
	netshpfile = sf;
	netshpfile->AddRef();	
	
	networkSize = numShapes;
	currentNode = 0;
	network = new shpNetNode[numShapes];
	ShpfileType shptype = SHP_NULLSHAPE;
	VARIANT cID, cDID;
	VariantInit(&cID); //added by Rob Cairns 4-Jan-06
	VariantInit(&cDID); //added by Rob Cairns 4-Jan-06
	long lval1 = 0, lval2 = 0;

	IUtils * gen = NULL;	
	CoCreateInstance(CLSID_Utils,NULL,CLSCTX_INPROC_SERVER,IID_IUtils,(void**)&gen);	
	
	long percent = 0, newpercent = 0;
	double total = numShapes;

	double length = 0;
	int i = 0;
	for( i = 0; i < numShapes; i++ )
	{	
		IShape * shp = NULL;		
		netshpfile->get_Shape(i,&shp);
		if( shp == NULL )
		{	
			netshpfile->Release();
			netshpfile = NULL;
			*retval = FALSE;
			lastErrorCode = tkINVALID_SHP_FILE;
			if( cBack != NULL )
				cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
			else if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		
			VariantClear(&cID); //added by Rob Cairns 4-Jan-06
			VariantClear(&cDID); //added by Rob Cairns 4-Jan-06

			return S_OK;
		}

		shp->get_ShapeType(&shptype);

		gen->get_Length(shp,&length);

		shp->Release();
		shp = NULL;

		if( shptype != SHP_POLYLINE && shptype != SHP_POLYLINEZ && shptype != SHP_POLYLINEM )
		{	gen->Release();
			Close(retval);
			*retval = FALSE;
			lastErrorCode = tkINCOMPATIBLE_SHAPEFILE_TYPE;
			if( cBack != NULL )
				cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
			else if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			

			VariantClear(&cID); //added by Rob Cairns 4-Jan-06
			VariantClear(&cDID); //added by Rob Cairns 4-Jan-06

			return S_OK;
		}
		else
		{	netshpfile->get_CellValue(0,i,&cID);
			netshpfile->get_CellValue(1,i,&cDID);
			
			lVal(cID,lval1);

			//assert( lval1 == i && "The ID != ShapeIndex" );

			if( lval1 != i )
			{	gen->Release();
				Close(retval);
				*retval = FALSE;
				lastErrorCode = tkINVALID_FIELD_VALUE;
				if( cBack != NULL )
					cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
				else if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				
				VariantClear(&cID); //added by Rob Cairns 4-Jan-06
				VariantClear(&cDID); //added by Rob Cairns 4-Jan-06

				return S_OK;
			}		

			lVal(cDID,lval2);		
			if( i == 0 )
			{
				//assert( lval2 == -1 && "The DID != -1" );

				if( lval2 != -1 )
				{	gen->Release();
					Close(retval);
					*retval = FALSE;
					lastErrorCode = tkINVALID_FIELD_VALUE;
					if( cBack != NULL )
						cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
					else if( globalCallback != NULL )
						globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				
					VariantClear(&cID); //added by Rob Cairns 4-Jan-06
					VariantClear(&cDID); //added by Rob Cairns 4-Jan-06

					return S_OK;
				}	
			}
			else
			{	
				//assert( ( lval2 >= 0 && lval2 < numShapes ) && "The DID is out of range." );

				if( lval2 < 0 || lval2 >= numShapes )
				{	gen->Release();
					Close(retval);
					*retval = FALSE;
					lastErrorCode = tkINVALID_FIELD_VALUE;
					if( cBack != NULL )
						cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
					else if( globalCallback != NULL )
						globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
					
					VariantClear(&cID); //added by Rob Cairns 4-Jan-06
					VariantClear(&cDID); //added by Rob Cairns 4-Jan-06

					return S_OK;
				}	
			}
			
			network[i].length = length;
			network[i].used = false;
			if( lval2 != -1 )
				network[lval2].up.push_back(i);
			network[i].parentIndex = lval2;
		}
		
		newpercent = (long)((i/total)*100);
		if( newpercent > percent )
		{	percent = newpercent;
			if( cBack != NULL )
				cBack->Progress(OLE2BSTR(key),percent,A2BSTR("ShpNetwork::Open"));
			else if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("ShpNetwork::Open"));
		}
	}
	gen->Release();
	gen = NULL;
	
	//Verify that every end shape can get to the outlet
	std::deque<long> netEnds;
	for( i = 0; i < networkSize; i++ )
	{	if( network[i].up.size() == 0 )
			netEnds.push_back(i);
	}
	percent = 0, newpercent = 0;
	total = netEnds.size();
	for( i = 0; i < (int)netEnds.size(); i++ )
	{
		bool * used = new bool[networkSize];
		memset(used,0,sizeof(bool)*networkSize);

		for( shpNetNode snn = network[i]; snn.parentIndex > 0; snn = network[snn.parentIndex] )
		{	if( used[snn.parentIndex] == true )
			{
				delete [] used;

				//This would create a loop				
				Close(retval);
				*retval = FALSE;

				lastErrorCode = tkNET_LOOP;
				if( cBack != NULL )
					cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
				else if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
				
				VariantClear(&cID); //added by Rob Cairns 4-Jan-06
				VariantClear(&cDID); //added by Rob Cairns 4-Jan-06

				return S_OK;				
			}
			used[snn.parentIndex] = true;
		}
		
		delete [] used;

		newpercent = (long)((i/total)*100);
		if( newpercent > percent )
		{	percent = newpercent;
			if( cBack != NULL )
				cBack->Progress(OLE2BSTR(key),percent,A2BSTR("ShpNetwork::Verifying Integrity of Network"));
			else if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("ShpNetwork::Verifying Integrity of Network"));
		}		
	}

	//Compute the distance to outlet
	std::deque<long> travNet;
	travNet.push_back(0);
	network[0].distanceToOutlet = 0;
	while( travNet.size() > 0 )
	{	long parent = travNet[0];
		travNet.pop_front();
		
		for( int i = 0; i < (int)network[parent].up.size(); i++ )
		{	long index = network[parent].up[i];
			network[index].distanceToOutlet = network[parent].distanceToOutlet + network[parent].length;
			travNet.push_back(index);
		}		
	}

	*retval = TRUE;
	VariantClear(&cID); //added by Rob Cairns 4-Jan-06
	VariantClear(&cDID); //added by Rob Cairns 4-Jan-06
	return S_OK;
}

STDMETHODIMP CShapeNetwork::Close(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( netshpfile != NULL )
		netshpfile->Release();
	netshpfile = NULL;
	if( network != NULL )
		delete [] network;
	network = NULL;
	currentNode = -1;
	networkSize = 0;

	return S_OK;
}

bool CShapeNetwork::IsAligned()
{	if( netshpfile == NULL )
	{	if( currentNode == -1 )
			return true;
		else
			return false;
	}
	else
	{	long numShapes = 0;
		netshpfile->get_NumShapes(&numShapes);
		if( numShapes == networkSize )
			return true;
		else
			return false;
	}
}

long roundCustom( double d )
{	
	if( ceil(d) - d <= .5 )
		return (int)ceil(d);
	else
		return (int)floor(d);
}

STDMETHODIMP CShapeNetwork::RasterizeD8(VARIANT_BOOL UseNetworkBounds, IGridHeader * Header, double cellsize, ICallback *cBack, IGrid **retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if( IsAligned() == false )
	{	*retval = NULL;
		lastErrorCode = tkNOT_ALIGNED;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
		return S_OK;		
	}

	if( cBack == NULL )
		cBack = globalCallback;

	short nodata = -1;
		
	if( UseNetworkBounds != VARIANT_FALSE )
	{	
		IExtents * box = NULL;
		netshpfile->get_Extents(&box);
		double xllcenter = 0, yllcenter = 0;
		double xurcenter = 0, yurcenter = 0;
		box->get_xMin(&xllcenter);
		box->get_yMin(&yllcenter);
		box->get_xMax(&xurcenter);
		box->get_yMax(&yurcenter);
		box->Release();
		
		if( cellsize <= 0 )
		{	*retval = NULL;
			lastErrorCode = tkINVALID_PARAMETER_VALUE;
			if( cBack != NULL )
				cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
			else if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
			return S_OK;
		}

		long ncols = roundCustom((xurcenter - xllcenter) / cellsize) + 1;
		long nrows = roundCustom((yurcenter - yllcenter) / cellsize) + 1;

		IGridHeader * nbheader = NULL;
		CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)&nbheader);
		nbheader->put_dX(cellsize);
		nbheader->put_dY(cellsize);
		VARIANT vndv;
		VariantInit(&vndv); //added by Rob Cairns 4-Jan-06
		vndv.vt = VT_I4;
		vndv.lVal = nodata;
		nbheader->put_NodataValue(vndv);
		nbheader->put_NumberCols(ncols);
		nbheader->put_NumberRows(nrows);
		nbheader->put_XllCenter(xllcenter);
		nbheader->put_YllCenter(yllcenter);

		VARIANT_BOOL vbretval = FALSE;
		CoCreateInstance(CLSID_Grid,NULL,CLSCTX_INPROC_SERVER,IID_IGrid,(void**)retval);

		if( cBack != NULL )
			cBack->Progress(OLE2BSTR(key),0,A2BSTR("ShpNetwork::RasterizeD8"));
		else if( globalCallback != NULL )
			globalCallback->Progress(OLE2BSTR(key),0,A2BSTR("ShpNetwork::RasterizeD8"));

		(*retval)->CreateNew(A2BSTR(""),nbheader,ShortDataType,vndv,VARIANT_TRUE,UseExtension,cBack,&vbretval);
		nbheader->Release();
		VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
		if( vbretval == VARIANT_FALSE )
		{	(*retval)->Release();
			*retval = NULL;
			lastErrorCode = tkGRID_NOT_INITIALIZED;
			if( cBack != NULL )
				cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
			else if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
			return S_OK;
		}
	}
	else
	{	if( Header == NULL )
		{	*retval = NULL;
			lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
			if( cBack != NULL )
				cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
			else if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
			return S_OK;
		}	
			
		double dx = 0.0, dy = 0.0;
		Header->get_dX(&dx);
		Header->get_dY(&dy);
		cellsize = dx;
		if( dy < cellsize )
			cellsize = dy;

		if( cellsize <= 0 )
		{	*retval = NULL;
			
			if( cellsize == dx )
				lastErrorCode = tkINVALID_DX;
			else
				lastErrorCode = tkINVALID_DY;

			if( cBack != NULL )
				cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
			else if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
			return S_OK;
		}		

		//Reset the nodata value
		VARIANT vndv;
		VariantInit(&vndv); //added by Rob Cairns 4-Jan-06
		vndv.vt = VT_I2;
		vndv.iVal = nodata;
		Header->put_NodataValue(vndv);

		VARIANT_BOOL vbretval = FALSE;
		CoCreateInstance(CLSID_Grid,NULL,CLSCTX_INPROC_SERVER,IID_IGrid,(void**)retval);

		if( cBack != NULL )
			cBack->Progress(OLE2BSTR(key),0,A2BSTR("ShpNetwork::RasterizeD8"));
		else if( globalCallback != NULL )
			globalCallback->Progress(OLE2BSTR(key),0,A2BSTR("ShpNetwork::RasterizeD8"));

		(*retval)->CreateNew(A2BSTR(""),Header,ShortDataType,vndv,VARIANT_TRUE,UseExtension,cBack,&vbretval);
		VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
		if( vbretval == FALSE )
		{	(*retval)->Release();
			*retval = NULL;
			lastErrorCode = tkGRID_NOT_INITIALIZED;
			if( cBack != NULL )
				cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));		
			else if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));			
			return S_OK;
		}
	}	
	
	//Generate a stack that has the ups at elements 0 -
	std::deque<long> upsfirst;
	std::deque<long> netstack;
	netstack.push_back(0);

	while( netstack.size() > 0 )//&& upsfirst.size() < 30 )
	{	
		long parent = netstack[0];
		netstack.pop_front();
		upsfirst.push_front(parent);		

		for( int i = 0; i < (int)network[parent].up.size(); i++ )
		{	long index = network[parent].up[i];
			netstack.push_back(index);			
		}
	}

	//Rasterize each segment
	//	Point0 is always the outlet

	long percent = 0, newpercent = 0;
	double total = upsfirst.size();

	for( int s = 0; s < (int)upsfirst.size(); s++ )
	{	long shapeIndex = upsfirst[s];

		IShape * shp = NULL;
		netshpfile->get_Shape(shapeIndex,&shp);

		long numPoints = 0;
		shp->get_NumPoints(&numPoints);
			
		std::deque<snraspnt> Rasterize;
		//Create a raster_cell that will be written
		IPoint * downpnt = NULL;
		shp->get_Point(0,&downpnt);
		double dpx, dpy;
		downpnt->get_X(&dpx);
		downpnt->get_Y(&dpy);
		downpnt->Release();
		
		snraspnt current_point;
		(*retval)->ProjToCell(dpx,dpy,&(current_point.column),&(current_point.row));
		current_point.length = cellsize;
		Rasterize.push_back( current_point );
		snraspnt temp_point;

		//Step through the line incrementing the length inside of the cell
		for( int i = 0; i < numPoints - 1; i++ )
		{		
			IPoint * one = NULL;
			IPoint * two = NULL;

			shp->get_Point(i,&one);
			shp->get_Point(i+1,&two);

			double x1, x2, y1, y2;
			one->get_X(&x1);
			one->get_Y(&y1);
			two->get_X(&x2);
			two->get_Y(&y2);

			one->Release();
			two->Release();

			double dx = x2 - x1;
			double dy = y2 - y1;

			//Step by 1/30th of the cellsize
			double increment = cellsize/30.0;

			double step_x = -increment;
			double step_y = -increment;
		
			if( dx > 0 )
				step_x = increment;
			if( dy > 0 )
				step_y = increment;		
					
			if( dx != 0.0 )
			{	
				double m = dy/dx;
				double b = y1 - m*x1;

				if( fabs(dx) > fabs(dy) )
				{	
					for( double i = 0; i < fabs(dx); i=i+increment )
					{	y1 = ( m*x1 + b );
						
						(*retval)->ProjToCell(x1,y1,&(temp_point.column),&(temp_point.row));						
						current_point.column = temp_point.column;
						current_point.row = temp_point.row;

						if( Rasterize[ Rasterize.size() - 1 ] == current_point )
						{	current_point.increment_length( increment );
							Rasterize[ Rasterize.size() - 1 ] = current_point;
						}
						else
						{	//Check to see if this raster has been used before
							bool prevUsed = false;
							for( int pu = 0; pu < (int)Rasterize.size(); pu++ )
							{	if( Rasterize[pu] == current_point )
								{	prevUsed = true;
									current_point = Rasterize[pu];
									Rasterize.erase( Rasterize.begin() + pu );
									current_point.increment_length( increment );
									Rasterize.push_back( current_point );
									break;
								}
							}
							if( !prevUsed )
							{
								current_point.length = increment;
								Rasterize.push_back( current_point );
							}
							
						}
																
						x1 = x1 + step_x;
					}
				}
				else
				{
					for( double j = 0; j < fabs(dy); j=j+increment )
					{	x1 = ( ( y1 - b )/m );						

						(*retval)->ProjToCell(x1,y1,&(temp_point.column),&(temp_point.row));						
						current_point.column = temp_point.column;
						current_point.row = temp_point.row;

						if( Rasterize[ Rasterize.size() - 1 ] == current_point )
						{	current_point.increment_length( increment );
							Rasterize[ Rasterize.size() - 1 ] = current_point;
						}
						else
						{	//Check to see if this raster has been used before
							bool prevUsed = false;
							for( int pu = 0; pu < (int)Rasterize.size(); pu++ )
							{	if( Rasterize[pu] == current_point )
								{	prevUsed = true;
									current_point = Rasterize[pu];
									Rasterize.erase( Rasterize.begin() + pu );
									current_point.increment_length( increment );
									Rasterize.push_back( current_point );
									break;
								}
							}
							if( !prevUsed )
							{
								current_point.length = increment;
								Rasterize.push_back( current_point );
							}
						}
										
						y1 = y1 + step_y;
					}
				}
			}
			else
			{	
				for( double j = 0; j < fabs(dy); j=j+increment )
				{	
					(*retval)->ProjToCell(x1,y1,&(temp_point.column),&(temp_point.row));						
					current_point.column = temp_point.column;
					current_point.row = temp_point.row;

					if( Rasterize[ Rasterize.size() - 1 ] == current_point )
					{	current_point.increment_length( increment );
						Rasterize[ Rasterize.size() - 1 ] = current_point;
					}
					else
					{	//Check to see if this raster has been used before
						bool prevUsed = false;
						for( int pu = 0; pu < (int)Rasterize.size(); pu++ )
						{	if( Rasterize[pu] == current_point )
							{	prevUsed = true;
								current_point = Rasterize[pu];
								Rasterize.erase( Rasterize.begin() + pu );
								current_point.increment_length( increment );
								Rasterize.push_back( current_point );
								break;
							}
						}
						if( !prevUsed )
						{
							current_point.length = increment;
							Rasterize.push_back( current_point );
						}
					}
					
					y1 = y1 + step_y;
				}
			}			
		}

		//Push the last point onto the deque
		IPoint * uppnt = NULL;
		shp->get_Point(numPoints - 1,&uppnt);
		double uppx, uppy;
		uppnt->get_X(&uppx);
		uppnt->get_Y(&uppy);
		uppnt->Release();
		(*retval)->ProjToCell(uppx,uppy,&(temp_point.column),&(temp_point.row));
		temp_point.length = cellsize;
		if( Rasterize[ Rasterize.size() - 1 ] == temp_point )
			Rasterize[ Rasterize.size() - 1 ] = temp_point;
		else
			Rasterize.push_back( temp_point );		

		shp->Release();
		shp = NULL;

		//Write the raster_cells to the grid
		//	with directional information of network flow
		//
		//	Flow Diagram
		//
		//	4 3 2		
		//  5 X 1
		//  6 7 8
		
		short direction = 0;
		double half_csize = cellsize*.5 - cellsize/30.0;

		bool first_point = true;
		VARIANT vval;
		VariantInit(&vval); //added by Rob Cairns 4-Jan-06
		short val;
			
		snraspnt last_raster = Rasterize[0];

		for( int k = 1; k < (int)Rasterize.size(); k++ )
		{	
			//Test if the Length is long enough to write in the final_grid
			double length = Rasterize[k].length;
			if( Rasterize[k].length >= half_csize )
			{	
				//Do not set the first point if it has already been set
				if( first_point == true )
				{	first_point = false;
					(*retval)->get_Value(Rasterize[k].column,Rasterize[k].row,&vval);
					sVal(vval,val);
					if( val != nodata )
					{	last_raster = Rasterize[k];
						continue;					
					}
				}

				//direction = length;
				direction = RasterDirection( Rasterize[k], last_raster );
				vval.vt = VT_I2;
				vval.iVal = direction;
				//vval.iVal = length;
				(*retval)->put_Value( Rasterize[k].column, Rasterize[k].row, vval );
				last_raster = Rasterize[k];
			}
		}				

		Rasterize.clear();	
		
		newpercent = (long)((s/total)*100);
		if( newpercent > percent )
		{	percent = newpercent;
			if( cBack != NULL )
				cBack->Progress(OLE2BSTR(key),percent,A2BSTR("ShpNetwork::RasterizeD8"));
			else if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("ShpNetwork::RasterizeD8"));
		}

		VariantClear(&vval); //added by Rob Cairns 4-Jan-06
	}

	return S_OK;
}

short CShapeNetwork::RasterDirection( snraspnt & source, snraspnt & sink )
{	
	long src_i = source.column; 
	long src_j = source.row;
	long snk_i = sink.column;
	long snk_j = sink.row;

	//Up or Down
	if( src_i == snk_i )
	{	if( src_j < snk_j )
			return 7;
		else if( src_j > snk_j )
			return 3;
	}
	//Left or Right
	else if( src_j == snk_j )
	{	if( src_i < snk_i )
			return 1;
		else if( src_i > snk_i )
			return 5;
	}	
	//UpRight or DownRight
	else if( src_i < snk_i )
	{	if( src_j < snk_j )
			return 8;
		else if( src_j > snk_j )
			return 2;
	}
	//UpLeft or DownLeft
	else if( src_i > snk_i )
	{	if( src_j < snk_j )
			return 6;
		else if( src_j > snk_j )
			return 4;
	}

	return 0;
}

