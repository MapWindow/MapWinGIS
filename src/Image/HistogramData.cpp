#include "stdafx.h"
#include "HistogramData.h"

// *************************************************************
//	  ComputeHistogram()
// *************************************************************
bool HistogramData::ComputeHistogram(GDALDataset* ds, int numBuckets)
{
	if (!ds) return false;

	Clear();

	_numBuckets = numBuckets;

	bool result = ComputeHistogramCore(ds);

	_state = result ? HistogramSuccess : HistogramFailed;

	return result;
}

// *************************************************************
//	  ComputeHistogramCore()
// *************************************************************
bool HistogramData::ComputeHistogramCore(GDALDataset* ds)
{
	_numBands = ds->GetRasterCount();

	int iBand;
	int nHistSize = 0;
	GUIntBig *panHistogram = NULL;
	GDALRasterBand * poBand;

	// For now we always compute min/max
	_padfMin = (double *)CPLCalloc(sizeof(double), _numBands);
	_padfMax = (double *)CPLCalloc(sizeof(double), _numBands);

	_papanLUTs = (int **)CPLCalloc(sizeof(int *), _numBands);

	/* ==================================================================== */
	/*      Process all bands.                                              */
	/* ==================================================================== */
	for (iBand = 0; iBand < _numBands; iBand++)
	{
		GDALColorInterp cInt;
		poBand = ds->GetRasterBand(iBand + 1);
		cInt = poBand->GetColorInterpretation();
		CPLErr eErr;

		/* -------------------------------------------------------------------- */
		/*      Get a reasonable histogram.                                     */
		/* -------------------------------------------------------------------- */
		eErr =
			poBand->GetDefaultHistogram(_padfMin + iBand,
			_padfMax + iBand,
			&nHistSize, &panHistogram,
			TRUE, NULL, NULL);

		if (eErr != CE_None)
			return false;

		panHistogram[0] = 0; // zero out extremes (nodata, etc)
		panHistogram[nHistSize - 1] = 0;

		/* -------------------------------------------------------------------- */
		/*      Total histogram count, and build cumulative histogram.          */
		/*      We take care to use big integers as there may be more than 4    */
		/*      Gigapixels.                                                     */
		/* -------------------------------------------------------------------- */
		GIntBig *panCumHist = (GIntBig *)CPLCalloc(sizeof(GIntBig), nHistSize);
		GIntBig nTotal = 0;
		int iHist;

		for (iHist = 0; iHist < nHistSize; iHist++)
		{
			panCumHist[iHist] = nTotal + panHistogram[iHist] / 2;
			nTotal += panHistogram[iHist];
		}

		CPLFree(panHistogram);

		if (nTotal == 0) nTotal = 1;

		/* -------------------------------------------------------------------- */
		/*      Now compute a LUT from the cumulative histogram.                */
		/* -------------------------------------------------------------------- */
		int *panLUT = (int *)CPLCalloc(sizeof(int), _numBuckets);
		int iLUT;

		for (iLUT = 0; iLUT < _numBuckets; iLUT++)
		{
			iHist = (iLUT * nHistSize) / _numBuckets;
			int nValue = (int)((panCumHist[iHist] * _numBuckets) / nTotal);
			panLUT[iLUT] = MAX(0, MIN(_numBuckets - 1, nValue));
		}
		_papanLUTs[iBand] = panLUT;

		CPLFree(panCumHist);
	}
	return true;
}

// *************************************************************
//	  Clear()
// *************************************************************
void HistogramData::Clear()
{
	_state = HistogramNotComputed;
	_numBuckets = 0;
	_numBands = 0;

	bool allocatedByGdal = false;
	if (!allocatedByGdal)
	{ 
		if (_padfMin)
		{
			CPLFree(_padfMin);
			_padfMin = NULL;
		}

		if (_padfMax)
		{
			CPLFree(_padfMax);
			_padfMax = NULL;
		}

		if (_papanLUTs)
		{
			for (int bandIndex = 1; bandIndex < _numBands; bandIndex++)
			{
				CPLFree(_papanLUTs[bandIndex]);
			}

			CPLFree(_papanLUTs);
			_papanLUTs = NULL;
		}
	}
}

// *************************************************************
//	  GetColorValue()
// *************************************************************
unsigned char HistogramData::GetColorValue(int bandIndex, double value)
{
	double dfScale = _numBuckets / (_padfMax[bandIndex - 1] - _padfMin[bandIndex - 1]);	 // can be cached

	int iBin = (int)((value - _padfMin[bandIndex - 1]) * dfScale);
	iBin = MAX(0, MIN(_numBuckets - 1, iBin));

	const int * panLUT = _papanLUTs[bandIndex - 1];

	return static_cast<unsigned char>(panLUT ? panLUT[iBin] : iBin);
}


