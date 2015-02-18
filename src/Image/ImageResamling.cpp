/**************************************************************************************
 * File name: ImageResampling.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: resampling of GDAL-based images. Is used by tkRaster class.
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
 // lsu 15-mar-2010 - Created this file, based upon code from Graphics32 library

#include "stdafx.h"
#include "ImageResampling.h"

#ifndef MAX
#  define MIN(a,b)      ((a<b) ? a : b)
#  define MAX(a,b)      ((a>b) ? a : b)
#endif


// **************************************************************
//		StretchTransfer()
// **************************************************************
// Resamples the image using specified filter; supports both 
// downsampling (zooming out) and upsampling (zooming in)
bool StretchTransfer(int srcXSize, int srcYSize, colour* srcData, int dstXSize, int dstYSize, colour* dstData, tkResamplingType resampleType, bool downSampling)
{  
	if (srcData == NULL || dstData == NULL) return false;
 
	CCustomKernel* Kernel = get_Kernel(resampleType, downSampling);
	if (Kernel == NULL) return false;
	bool RangeCheck = Kernel->RangeCheck();
	
	// ----------------------------------------------------------
	// Building of mapping tables: it stores which source pixels determine the value of every
	// destination pixel separately for x and y axis (pattern is the same for any row or column) 
	// ----------------------------------------------------------
	TMappingTable MapX = BuildMappingTable(/*dst*/0, dstXSize, /*dest clip*/0, dstXSize, /*source*/0, srcXSize, (CCustomKernel*)Kernel);
	TMappingTable MapY = BuildMappingTable(/*dst*/0, dstYSize, /*dest clip*/0, dstYSize, /*source*/0, srcYSize, (CCustomKernel*)Kernel);
	if (MapX == NULL || MapY == NULL) 
	{
		delete Kernel;
		return false;
	}
	
	// X - range for the source pixels
	int MapXLoPos = MapX[0][0].pos;			// [dest][source]
	int MapXHiPos = MapX[dstXSize - 1][MapX[dstXSize - 1][0].length - 1].pos;
	
	// values of one row after y-interpolation (first step)
	CBufferEntry* HorzBuffer = new CBufferEntry[MapXHiPos - MapXLoPos + 1];
	CBufferEntry* entry;
	
	// an entry of mapping table (set of source pixels which determine value of the given dest pixel)
	CPointRec* ClusterX = NULL;	  
	CPointRec* ClusterY = NULL;

	// weight and values of components of colour
	int Wt, Cr, Cg, Cb;
	colour* C;

	for (int j = 0; j < dstYSize; j++)
	{
		// --------------------------------------------------------------------
		//	Calculating the values of destination pixels for the given row
		//	based upon values of several source rows (interpolation by Y axis)
		// --------------------------------------------------------------------
		ClusterY = MapY[j];
		for (int x = MapXLoPos; x <= MapXHiPos; x++)
		{
			Cr = 0; Cg = 0; Cb = 0;
			for (int y = 0; y < ClusterY[0].length; y++)
			{
				C = srcData + (x + ClusterY[y].pos * srcXSize);
				Wt = ClusterY[y].weight;
				Cr += C->red * Wt;
				Cg += C->green * Wt;
				Cb += C->blue * Wt;
			}
			
			// writing the values
			HorzBuffer[x - MapXLoPos].R = Cr;
			HorzBuffer[x - MapXLoPos].G = Cg;
			HorzBuffer[x - MapXLoPos].B = Cb;
		}

		// ----------------------------------------------------------------------
		//	Final calculating the values of destination pixels for the given row
		//	using values from HorzBuffer and interpolation by X axis
		// ----------------------------------------------------------------------
		for (int i = 0; i < dstXSize; i++)
		{
			ClusterX = MapX[i];
			Cr = 0; Cg = 0; Cb = 0;
			for (int x = 0; x < ClusterX[0].length; x++)
			{
				entry = HorzBuffer + (ClusterX[x].pos - MapXLoPos);
				Wt = ClusterX[x].weight;
				Cr += entry->R * Wt;
				Cg += entry->G * Wt;
				Cb += entry->B * Wt;
			}
			
			// the value of each component must be 
			if (RangeCheck)
			{
				if (Cr > 0xFF0000) Cr = 0xFF0000;
				else if (Cr < 0) Cr = 0;
				else Cr = (Cr & 0x00FF0000);

				if (Cg > 0xFF0000) Cg = 0xFF0000;
				else if (Cg < 0) Cg = 0;
				else Cg = (Cg & 0x00FF0000);

				if (Cb > 0xFF0000) Cb = 0xFF0000;
				else if (Cb < 0) Cb = 0;
				else Cb = (Cb & 0x00FF0000);
			}

			// writing the value of destination pixel
			C = dstData + (j * dstXSize + i);
			C->red = Cr / 65536;
			C->blue = Cb / 65536;
			C->green = Cg / 65536;
		}
	}
	
	// -----------------------------------------------
	//   Cleaning 
	// -----------------------------------------------
	delete Kernel;

	for(int j = 0; j < dstXSize; j++)	  
		delete[] MapX[j];
	delete MapX;

	for(int j = 0; j < dstYSize; j++)	  
		delete[] MapY[j];
	delete MapY;

	delete[] HorzBuffer;
	
	return true;
}

// **************************************************************
//		Constrain()
// **************************************************************
// Check that the value falls into the range
inline int Constrain(const int value, const int low, const int high)
{
  if (value < low)
  	return low;
  else if (value > high)
  	return high;
  else 
  	return value;
}

// **************************************************************
//		BuildMappingTable()
// **************************************************************
TMappingTable BuildMappingTable( int DstLo, int DstHi, int ClipLo, int ClipHi, int SrcLo, int SrcHi, CCustomKernel* Kernel)
{
	// checking input values
	int SrcW = SrcHi - SrcLo;
	int DstW = DstHi - DstLo;
	int ClipW = ClipHi - ClipLo;
	if (SrcW == 0) return NULL;
	if (ClipW == 0) return NULL;

	bool FullEdge = false;		// TODO: try, how it works
	CPointRec** tbl = NULL;		// resulting table
	
	// ------------------------------------------------------
	// there is only one source pixel; it'll determine
	// the values of all destination pixels
	// ------------------------------------------------------
	if( SrcW == 1)
	{
		tbl = new CPointRec*[ClipW];		// destination pixels
		for( int i = 0; i < ClipW; i++)
		{
			tbl[i] = new CPointRec[1];		// source pixels which correspond to them
			tbl[i][0].pos = 0;				
			tbl[i][0].weight = 256;			
			tbl[i][0].length = 1;
		}
		return tbl;
	}
	else
	{
		double Scale;
		double Center;	 // center of filter window in src coordinates
		double Count;	 // sum of weights
		int Left, Right; // borders of filter window
		int	Weight;

		tbl = new CPointRec*[ClipW];	// destination pixels
		double FilterWidth = Kernel->GetWidth();
		
		if (FullEdge)	Scale = (double)DstW / SrcW;
		else	  		Scale = (double)(DstW - 1) / (SrcW - 1);
		
		// ------------------------------------------------------------
		//  Destination width is 0. In fact no need to return anything
		// ------------------------------------------------------------
		if (Scale == 0)
		{
			  tbl[0] = new CPointRec[1];
			  tbl[0][0].pos = (SrcLo + SrcHi) / 2;
			  tbl[0][0].weight = 256;
			  tbl[0][0].length = 1;
			  return tbl;
		}
		
		// ------------------------------------------------------------
		//  Downsampling (zooming out). For each dest pixel there is
		//	a number of source pixels
		// ------------------------------------------------------------
		else if (Scale < 1)
		{
			double OldScale = Scale;
			Scale = 1 / Scale;
			FilterWidth = FilterWidth * Scale;
			for (int i = 0; i < ClipW; i++)
			{
				if (FullEdge)
					Center = SrcLo - 0.5 + (i - DstLo + ClipLo + 0.5) * Scale;
				else
					Center = SrcLo + (i - DstLo + ClipLo) * Scale;

				Left = (int)floor(Center - FilterWidth);
				Right = (int)ceil(Center + FilterWidth);
				Count = -256;

				// we'll allocate memory for all possible pixels, in fact part of the pixels may have zero weight
				int subArrayLength = Right - Left + 1;
				tbl[i] = new CPointRec[subArrayLength]; 	// source pixels for each destination
				
				int k = 0;
				for (int j = Left; j <= Right; j++)
				{
					Weight = (int)floor(256 * Kernel->Filter((Center - j) * OldScale) * OldScale + 0.5);
					if (Weight != 0)
					{
						Count += Weight;								
						tbl[i][k].pos = Constrain(j, SrcLo, SrcHi - 1);
						tbl[i][k].weight = Weight;
						k++;
					}
				}

				// there is no source pixels with non-zero weight we'll just take the nearest neigbour
				if ( k == 0 )
				{
					tbl[i][0].pos = (int)floor(Center);
			        tbl[i][0].weight = 256;
					k = 1;
				}
			    
				// if the sum of weights for source pixel not equal to 256,  we'll correct the value of center-most pixel
				else 
				{
					if (Count != 0)  
						tbl[i][k/2].weight -= (unsigned int)Count;	
				}

				// setting the length (some weights of surce could be zeroes and we won't use them in calcuations)
				tbl[i][0].length = k;
			}
			return tbl;
		}
		
		// ------------------------------------------------------------
		//  Upsamling (zooming in). In place of each source pixel
		//	will be more than one destination pixels
		// ------------------------------------------------------------
		else // scale > 1
		{
			Scale = 1 / Scale;
			for (int i = 0; i < ClipW; i++)
			{
				if (FullEdge)
					Center = SrcLo - 0.5 + (i - DstLo + ClipLo + 0.5) * Scale;
				else
					Center = SrcLo + (i - DstLo + ClipLo) * Scale;
				
				Left = (int)floor(Center - FilterWidth);
				Right = (int)ceil(Center + FilterWidth);
				Count = -256;
				
				// we'll allocate memory for all possible pixels, in fact part of the pixels may have zero weight
				int subArrayLength = Right - Left + 1;
				tbl[i] = new CPointRec[subArrayLength];	// source pixels for each destination
				
				int k = 0;
				for (int j = Left; j <= Right; j++)
				{
					Weight = (int)floor(256 * Kernel->Filter(Center - j) + 0.5);
					if (Weight != 0)
					{
						Count += Weight;
						tbl[i][k].pos = Constrain(j, SrcLo, SrcHi - 1);
						tbl[i][k].weight = Weight;
						k++;
					}		         
				}
				
				// if the sum of weights for source pixel not equal to 256,  we'll correct the value of center-most pixel
				if (Count != 0)
					tbl[i][k/2].weight -= (unsigned int)Count;

				// setting the length (some weights of surce could be zeroes and we won't use them in calcuations)
				tbl[i][0].length = k;

			}
			return tbl;
		}
	}
}

// **************************************************************
//   Choosing resampling kernel
// **************************************************************
CCustomKernel* get_Kernel(tkResamplingType resampleKernel, bool downSampling)
{
	CCustomKernel* kernel;
	if (downSampling)
	{
		// we'll take this one as it's the fastest; the quality isn't critical here
		kernel = new CBoxKernel();
	}
	else
	{
		switch(resampleKernel)
		{
			case rtLinear:
				kernel = new CLinearKernel();
				break;
			case rtCubic:
				kernel = new CCubicKernel();
				break;
			case rtLanczos:
				kernel = new CLanczosKernel();
				break;
			default:
				kernel = new CLinearKernel();
				break;
		}
	}
	return kernel;
}

// ***********************************************************
//		CBoxKernel
// ***********************************************************
double CBoxKernel::Filter(double Value)
{
	if (Value >= -0.5 && Value <= 0.5) return 1.0;
	else return 0.0;
}
double CBoxKernel::GetWidth()
{
	return 1.0;
}

// ***********************************************************
//		CLinearKernel
// ***********************************************************
double CLinearKernel::Filter(double Value)
{
  if (Value < -1) return 0.0;
  else if (Value < 0) return 1.0 + Value;
  else if (Value < 1) return 1.0 - Value;
  else return 0.0;
}
double CLinearKernel::GetWidth()
{
  return 1.0;
}

// ***********************************************************
//		CCubicKernel
// ***********************************************************
CCubicKernel::CCubicKernel()
{
	coeff = -0.5;
}
double CCubicKernel::Filter(double Value)
{
  Value = fabs(Value);
  double tt = Value * Value;
  double ttt = tt * Value;
  if (Value < 1)
    return (coeff + 2.0) * ttt - (coeff + 3.0) * tt + 1;
  else if (Value < 2)
    return coeff * (ttt - 5.0 * tt + 8.0 * Value - 4);
  else
    return 0.0;
}
bool CCubicKernel::RangeCheck()
{
	return true;
}
double CCubicKernel::GetWidth()
{
	return 2.0;
}
void CCubicKernel::SetCoeff(const double Value)
{
	coeff = Value;
}
double CCubicKernel::GetCoeff()
{
	return coeff;
}

// ***********************************************************
//		CWindowedSincKernel
// ***********************************************************
double Sinc(double Value)
{
	if (Value != 0)
	{
		Value = Value * pi_;
		return sin(Value) / Value;
	}
	else return 1.0;
}
CWindowedSincKernel::CWindowedSincKernel()
{
	width = 3;
	widthReciprocal = 1 / width;
}
double CWindowedSincKernel::Filter(double Value)
{
	Value = fabs(Value);
	if (Value < width)
		return Sinc(Value) * Window(Value);
	else
		return 0.0;
}
bool CWindowedSincKernel::RangeCheck()
{
  return true;
}
void CWindowedSincKernel::SetWidth(double Value)
{
	Value = MIN(MAX_KERNEL_WIDTH, Value);
	width = Value;
	widthReciprocal = 1 / width;
	
}
double CWindowedSincKernel::GetWidth()
{
	return width;
}

// ***********************************************************
//		CLanczosKernel
// ***********************************************************
double CLanczosKernel::Window(double Value)
{
	return Sinc(Value * widthReciprocal);
}
