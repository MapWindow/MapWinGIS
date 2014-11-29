/**************************************************************************************
 * File name: ShapeNetwork.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: declaration of CShapeNetwork
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
#include "graph.h"
#include "heap.h"

struct dPOINT
{
	double x;
	double y;
};

// CShapeNetwork
class ATL_NO_VTABLE CShapeNetwork : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShapeNetwork, &CLSID_ShapeNetwork>,
	public IDispatchImpl<IShapeNetwork, &IID_IShapeNetwork, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CShapeNetwork()
	{
		USES_CONVERSION;
		_lastErrorCode = tkNO_ERROR;
		_globalCallback = NULL;
		_key = A2BSTR("");

		_network = NULL;
		_currentNode = -1;
		_networkSize = 0;
		_netshpfile = NULL;
	}
	~CShapeNetwork()
	{
		::SysFreeString(_key);
		if( _netshpfile != NULL )
			_netshpfile->Release();
		_netshpfile = NULL;
		if( _network != NULL )
			delete [] _network;
		_network = NULL;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SHAPENETWORK)

	DECLARE_NOT_AGGREGATABLE(CShapeNetwork)

	BEGIN_COM_MAP(CShapeNetwork)
		COM_INTERFACE_ENTRY(IShapeNetwork)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()


// IShapeNetwork
public:
	STDMETHOD(Close)(/*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(Open)(/*[in]*/ IShapefile * sf, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(get_ParentIndex)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_ParentIndex)(/*[in]*/ long newVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_AmbigShapeIndex)(/*[in]*/ long Index, /*[out, retval]*/ long *pVal);
	STDMETHOD(get_NetworkSize)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_NumDirectUps)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_DistanceToOutlet)(/*[in]*/ long PointIndex, /*[out, retval]*/ double *pVal);
	STDMETHOD(get_CurrentShapeIndex)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_CurrentShape)(/*[out, retval]*/ IShape * *pVal);
	STDMETHOD(get_Shapefile)(/*[out, retval]*/ IShapefile * *pVal);
	STDMETHOD(MoveToOutlet)(/*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(MoveTo)(/*[in]*/long ShapeIndex, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(MoveDown)(/*[out, retval]*/VARIANT_BOOL *retval);
	STDMETHOD(MoveUp)(/*[in]*/long UpIndex, /*[out, retval]*/VARIANT_BOOL * retval);
	STDMETHOD(RasterizeD8)(/*[in]*/VARIANT_BOOL UseNetworkBounds, /*[in, optional]*/IGridHeader * Header, /*[in, optional]*/ double Cellsize, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ IGrid ** retval);
	STDMETHOD(DeleteShape)(/*[in]*/long ShapeIndex, /*[out, retval]*/VARIANT_BOOL * retval);
	STDMETHOD(Build)(/*[in]*/IShapefile *Shapefile, /*[in]*/long ShapeIndex, /*[in]*/long FinalPointIndex, /*[in]*/double Tolerance,/*[in]*/AmbiguityResolution ar, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ long * retval);

private:
	
	struct snraspnt
	{
	public:
		snraspnt()
		{
			column = 0;
			row = 0;
			length = 0;
		}
		snraspnt(const snraspnt & r)
		{
			column = r.column;
			row = r.row;
			length = r.length;
		}
		snraspnt(long c, long r, double l = 0.0)
		{
			column = c;
			row = r;
			length = l;
		}
		snraspnt operator=(const snraspnt & r)
		{
			column = r.column;
			row = r.row;
			length = r.length;
			return *this;
		}
		bool operator==(const snraspnt & r)
		{
			if (column == r.column && row == r.row)
				return true;
			return false;
		}
		void increment_length(double increment)
		{
			length += increment;
		}

		long row;
		long column;
		double length;
	};

	struct shpNetNode
	{
		shpNetNode()
		{
			distanceToOutlet = -1;
			length = 0;
			parentIndex = -1;
			downIndex = -1;
			upIndex = -1;
			used = false;
		}

		shpNetNode operator= (const shpNetNode & sn)
		{
			distanceToOutlet = sn.distanceToOutlet;
			length = sn.length;
			downIndex = sn.downIndex;
			upIndex = sn.upIndex;
			parentIndex = sn.parentIndex;
			used = sn.used;
			for (int i = 0; i < (int)sn.up.size(); i++)
				up.push_back(sn.up[i]);
			for (int j = 0; j < (int)sn.pbIndex.size(); j++)
				pbIndex.push_back(sn.pbIndex[j]);
			return *this;
		}

		~shpNetNode()
		{
		}

		std::deque<long> up;
		std::deque<long> pbIndex;

		double distanceToOutlet;
		double length;
		long downIndex;
		long upIndex;
		long parentIndex;
		bool used;
	};

private:
	long _lastErrorCode;
	ICallback * _globalCallback;
	BSTR _key;

	shpNetNode * _network;
	long _currentNode;
	long _networkSize;
	IShapefile * _netshpfile;
	std::deque<long> _ambigShapeIndex;

private:
	long UpEnd(edge * e, dPOINT * downpoint, double tolerance);
	long DownEnd(edge * e, dPOINT * downpoint, double tolerance);
	void CopyShape(bool reversePoints, IShape * oldshape, IShape * newshape);
	void CopyField(IField * oldshape, IField * newshape);
	bool IsAligned();
	short RasterDirection(snraspnt & source, snraspnt & sink);
	void recPrintShpNetwork(shpNetNode * allnodes, long index, ofstream & out);
	void PrintShpNetwork(shpNetNode * allnodes, long outlet, const char * filename);
};

OBJECT_ENTRY_AUTO(__uuidof(ShapeNetwork), CShapeNetwork)
