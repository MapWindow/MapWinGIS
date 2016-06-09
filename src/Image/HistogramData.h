#pragma once

class HistogramData
{
private:
	int _numBands;
	int		_numBuckets;
	double  *_padfMin;		// minimum histogram value for each band
	double  *_padfMax;		// maximum histogram value for each band
	int     **_papanLUTs;			// histogram lookout table for each band
	HistogramState _state;

private:
	bool ComputeHistogramCore(GDALDataset* ds);

public:
	HistogramData()
	{
		_state = HistogramNotComputed;
		_padfMin = NULL;
		_padfMax = NULL;
		_papanLUTs = NULL;
		_numBuckets = 0;
		_numBands = 0;
	};

	~HistogramData() {
		Clear();
	}

	HistogramState GetState() { return _state; }
	bool CanUse() { return _state ==  HistogramSuccess; }

	void Clear() ;
	bool ComputeHistogram(GDALDataset* ds, int numBuckets);
	unsigned char GetColorValue(int bandIndex, double value);
};

