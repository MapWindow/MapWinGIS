/**************************************************************************************
 * File name: ImageResampling.h
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

#pragma once
#include "colour.h"

const int MAX_KERNEL_WIDTH = 4;

// **************************************************************
//		Data structures
// **************************************************************
struct CPointRec
{
	int pos;
	unsigned int weight;
	int length;			// the length of sub-array; is stored in it's first element
};
typedef CPointRec** TMappingTable;

struct CBufferEntry
{
    int B;
	int G;
	int R;
};

// **************************************************************
//		Filter kernels
// **************************************************************
class CCustomKernel
{
protected:
public:
	virtual bool RangeCheck(){return false;};
	CCustomKernel(){};
	void Changed();
	virtual double Filter(double Value) = 0;
	virtual double GetWidth() = 0;
};

// TBoxKernel
class CBoxKernel: public CCustomKernel
{
public:
    double Filter(double Value);
    double GetWidth();
};

// TLinearKernel
class CLinearKernel: public CCustomKernel
{
 public:
    double Filter(double Value);
    double GetWidth();
};

// TCubicKernel
class CCubicKernel: public CCustomKernel
{
protected:
	double coeff;
public:
	bool RangeCheck();
	CCubicKernel();
	double GetCoeff();
	void SetCoeff(const double Value);
    double Filter(double Value);
    double GetWidth();
};  

// TWindowedSincKernel
class CWindowedSincKernel : public CCustomKernel
{
protected:
	double width;
	double widthReciprocal;
	virtual double Window(double Value) = 0;
public:
	bool RangeCheck();
	CWindowedSincKernel();
	double Filter(double Value);
	void SetWidth(double Value);
	double GetWidth();
	double GetWidthReciprocal();
};

 // TLanczosKernel
class CLanczosKernel: public CWindowedSincKernel
{
protected:
	double Window(double Value);
};

// **************************************************************
//		Functions
// **************************************************************
// resampling routine
bool StretchTransfer(int srcXSize, int srcYSize, colour* srcData, int dstXSize, int dstYSize, colour* dstData, tkResamplingType resampleType, bool downSampling);
// build tables for StretchTransfer function
TMappingTable BuildMappingTable( int DstLo, int DstHi, int ClipLo, int ClipHi, int SrcLo, int SrcHi, CCustomKernel* Kernel);
// returns necessary kernel
CCustomKernel* get_Kernel(tkResamplingType resampleKernel, bool downSampling);
