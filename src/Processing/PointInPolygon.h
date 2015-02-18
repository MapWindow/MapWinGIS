#pragma once
using namespace std;

class CPointInPolygon {
private:
	deque<long> _polyParts;	// poly data
	deque<double> _polyX;
	deque<double> _polyY;
	deque<double> _scanX;		// line data for a single scan (y = const)
	deque<long> _scanParts;
public:	
	// **************************************************
	//		setPolygon
	// **************************************************
	bool SetPolygon(IShape* poly)
	{
		if (!poly)
			return false;

		ShpfileType shptype;
		poly->get_ShapeType(&shptype);

		if(shptype != SHP_POLYGON && shptype != SHP_POLYGONZ && shptype != SHP_POLYGONM )
		{	
			return false;
		}
		
		_polyParts.clear();
		_polyX.clear();
		_polyY.clear();

		long numParts = 0;
		long numPoints = 0;
		poly->get_NumParts(&numParts);
		poly->get_NumPoints(&numPoints);
		
		if(numParts == 0)
		{
			_polyParts.push_back(0);			
		}
		else
		{
			long part =0;
			for( int j = 0; j < numParts; j++ )
			{	
				poly->get_Part(j,&part);
				_polyParts.push_back(part);
			}
		}
		
		VARIANT_BOOL ret;
		double x = 0.0;
		double y = 0.0;
		for( int i = 0; i < numPoints; i++ )
		{	
			poly->get_XY(i, &x, &y, &ret);
			_polyX.push_back(x);
			_polyY.push_back(y);
		}

		return true;
	}

	// **************************************************
	//		PrepareScanLine
	// **************************************************
	void PrepareScanLine(double y)
	{
		_scanX.clear();
		_scanParts.clear();

		long numParts = _polyParts.size();

		for (long j = 0; j < numParts; j++)
		{
			long start = _polyParts[j];
			long end = (j == numParts - 1) ? _polyX.size(): _polyParts[j + 1];
			
			bool partEmpty = true;

			for (long i = start; i < end - 1; i++)
			{
				double x1 = _polyX[i];
				double y1 = _polyY[i] - y;
				double x2 = _polyX[i + 1];
				double y2 = _polyY[i + 1] - y;
				
				if (y1 * y2 < 0 || (y1 == 0 && y2 != 0)) // there is an intersection or we are in vertex,
														 // y2 excluded because we don't want to count intersection twice
														 // (the vertex on the scanline will belong to 2 neibouring segments)
														 // choosing y1 or y2 is arbitrary
														 // it also eliminates the case of segment lying exactly along the scanline (it won't be accounted as intersection)
				{
					double x = x1 + (x2 - x1) * abs(y1 / (y1 - y2));
					_scanX.push_back(x);
					if (partEmpty)
					{
						_scanParts.push_back(_scanX.size() - 1);	// save the index of the first intersection for a part
						partEmpty = false;
					}
				}
			}
			// points of each part must be in increasing order
			if (!partEmpty) {
				sort(_scanX.begin() + _scanParts[_scanParts.size() - 1], _scanX.end());
			}
		}
		
		#ifdef _DEBUG
			CString temp;
			CString s;
			for (size_t i = 0; i < _scanX.size(); i++) {
				temp.Format("%f;", _scanX[i]);
				s += temp;
			}
			Debug::WriteLine(s);
		#endif
	}

	// **************************************************
	//		TestScanPoint
	// **************************************************
	bool ScanPoint(double x)
	{
		if (_scanParts.size() == 1)	// treat single-part case separately as it can be a bit faster and it's a common one
		{
			int count = 0;
			for (size_t i = 0; i < _scanX.size(); i++)
			{
				if (_scanX[i] < x)
					count++;
				else
					break;			// a fix suggested here: http://bugs.mapwindow.org/view.php?id=2349
			}
			return (count % 2 != 0);
		}
		else
		{
			int ringCount = 0;
			size_t numParts = _scanParts.size();
			for (size_t j = 0; j < numParts; j++)
			{
				int count = 0;
				long start = _scanParts[j];
				long end = (j == numParts - 1) ? _scanX.size(): _scanParts[j + 1];
				for (long i = start; i < end; i++)
				{
					if (_scanX[i] < x)
						count++;
					else
						break;		// a fix suggested here: http://bugs.mapwindow.org/view.php?id=2349
				}
				if (count % 2 != 0)
					ringCount++;
			}
			return (ringCount % 2 != 0);
		}
	}
};