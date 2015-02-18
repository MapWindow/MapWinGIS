/**************************************************************************************
 * File name: JenksBreaks.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of JenksBreaks.h
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
 // Sergei Leschinski (lsu) 25 june 2010 - created the file

#include "stdafx.h"
#include "JenksBreaks.h"

#pragma region Constructor
// ****************************************************
//		Constructor
// ****************************************************
CJenksBreaks::CJenksBreaks(std::vector<double>* values, int numClasses)
{
	_init = false;
	if ((int)values->size() < numClasses)
	{
		// it doesn't make any sense to create Jenks breaks
		// more simple classification shall be used
		return;
	}
	
	if (numClasses > 0)
	{
		_numClasses = numClasses;
		_numValues = values->size();
		
		double classCount = (double)_numValues/(double)numClasses;
		sort(values->begin(), values->end());		//values sould be sorted
		
		// fill values
		for (int i = 0; i < _numValues; i++)
		{
			JenksData data;
			data.value = (*values)[i];
			data.square = pow((*values)[i], 2.0);
			data.classId = int(floor(i/classCount));
			_values.push_back(data);
		}
		
		_classes.resize(_numClasses);

		// calculate initial deviations for classes
		int lastId = -1;
		for (int i = 0; i < _numValues; i++)
		{
			int classId = _values[i].classId;
			if (classId >= 0  && classId < _numClasses)
			{
				_classes[classId].value += _values[i].value;
				_classes[classId].square += _values[i].square;
				_classes[classId].count += 1;

				// saving bound between classes
				if (classId != lastId)
				{
					_classes[classId].startId = i;
					lastId = classId;
					
					if (classId > 0)
					{
						_classes[classId - 1].endId = i - 1;
					}
				}
			}
			else
			{
				// TODO: add error handling
			}
		}
		_classes[_numClasses - 1].endId = _numValues - 1;
		
		for (int i = 0; i < _numClasses; i++)
		{
			_classes[i].RefreshStandardDeviations();
		}
		_init = true;
	}
}
#pragma endregion

#pragma region  Iterative heuristic
// **********************************************************
//     get_Results()
// **********************************************************
// Returning of results (indices of values to start each class)
std::vector<long>* CJenksBreaks::get_Results()
{
	if (_init)
	{
		std::vector<long>* results = new std::vector<long>;
		results->resize(_numClasses);
		for (int i = 0; i < _numClasses; i++ )
		{
			(*results)[i] = _classes[i].startId;
		}
		return results;
	}
	else
	{
		return NULL;
	}
}

// *************************************************************
//  Optimization routine
// *************************************************************
void CJenksBreaks::Optimize()
{
	if (!_init)
		return;
	
	// initialization
	double minValue = get_SumStandardDeviations();	// current best minimum
	_leftBound = 0;							// we'll consider all classes in the beginning
	_rightBound = _classes.size() - 1;
	_previousMaxId = -1;
	_previousTargetId = - 1;
	int numAttemmpts = 0;

	bool proceed = true;
	while (proceed)
	{
		for (int i = 0; i < _numValues; i++)
		{
			if (!FindShift())
			{
				return;
			}
			
			// when there are only 2 classes left we should stop on the local max value
			if (_rightBound - _leftBound == 0)
			{
				double val = get_SumStandardDeviations();	// the final minimum
				numAttemmpts++; 
				
				if ( numAttemmpts > 5)
				{
					return;
				}
			}
		}
		double value = get_SumStandardDeviations();
		proceed = (value < minValue)?true:false;	// if the deviations became smaller we'll execute one more loop
		
		if (value < minValue)
			minValue = value;
	}
}
#pragma endregion

#pragma region Private members
// ******************************************************************
//		get_SumStandardDeviations()
// ******************************************************************
// Calculates the sum of standard deviations of individual variants 
// from the class mean through all class
// It's the objective function - should be minimized
double CJenksBreaks::get_SumStandardDeviations()
{
	double sum = 0.0;
	for (int i = 0; i < _numClasses; i++) 
	{
		sum += _classes[i].SDev;
	}
	return sum;
}

// ******************************************************************
//	  MakeShift()
// ******************************************************************
// Passing the value from one class to another to another. Criteria - standard deviation.
bool CJenksBreaks::FindShift()
{
	// first we'll find classes with the smallest and largest SD
	int maxId = -1, minId = -1; 
	double minValue = DBL_MAX, maxValue = -DBL_MAX;
	for (int i = _leftBound; i <= _rightBound; i++) 
	{
		if (_classes[i].SDev > maxValue)
		{
			maxValue = _classes[i].SDev;
			maxId = i;
		}

		if (_classes[i].SDev < minValue)
		{
			minValue = _classes[i].SDev;
			minId = i;
		}
	}

	if (minValue == -1 || maxValue == -1)
	{
		return false;
	}

	// then pass one observation from the max class in the direction of min class
	int valueId = -1;
	int targetId = -1;
	if (maxId > minId)
	{
		//<-  we should find first value of max class
		valueId = _classes[maxId].startId; 
		targetId = maxId - 1;
		if (_classes[maxId].startId == _values.size() - 1 ||
			_classes[targetId].endId == _values.size() - 1)
		{
			// it's can't be moved any further
			return false;
		}
		else
		{
			_classes[maxId].startId++;
			_classes[targetId].endId++;
		}
	}
	else if (maxId < minId)
	{
		//->  we should find last value of max class
		valueId = _classes[maxId].endId; 
		targetId = maxId + 1;
		if (_classes[maxId].endId == 0 || 
			_classes[targetId].startId == 0)
		{
			// it's can't be moved any further
			return false;
		}
		else
		{
			_classes[maxId].endId--;
			_classes[targetId].startId--;
		}
	}
	else
	{
		// only one class left or the deviations within classes are equal
		return false;
	}
	
	if (valueId == -1)
	{
		return false;
	}

	// Prevents stumbling in local optimum - algorithm will be repeating the same move
	// To prevent this we'll exclude part of classes with less standard deviation
	if (_previousMaxId == targetId && _previousTargetId == maxId)
	{
		// Now we choose which of the two states provides less deviation
		double value1 = get_SumStandardDeviations();
		
		// change to second state
		MakeShift(maxId, targetId, valueId);
		double value2 = get_SumStandardDeviations();
		
		// if first state is better revert to it
		if (value1 < value2)
		{
			MakeShift(targetId, maxId, valueId);
		}
		
		// now we can exclude part of the classes where no improvements can be expected
		int min = MIN(targetId, maxId);
		int max = MAX(targetId, maxId);
		
		double avg = get_SumStandardDeviations()/(_rightBound - _leftBound + 1);

		// analyze left side of distribution
		double sumLeft = 0, sumRight = 0;
		for (int j = _leftBound; j <= min; j++)
		{
			sumLeft += pow(_classes[j].SDev - avg, 2.0);
		}
		sumLeft /= (min - _leftBound + 1);

		// analyze right side of distribution
		for (int j = _rightBound; j >= max; j--)
		{
			sumRight += pow(_classes[j].SDev - avg, 2.0);
		}
		sumRight /= (_rightBound - max + 1);

		// exluding left part
		if (sumLeft >= sumRight)
		{
			_leftBound = max;
		}
		// exluding right part
		else if (sumLeft < sumRight)
		{
			_rightBound = min;
		}
	}
	else
	{
		MakeShift(maxId, targetId, valueId);
	}

	return true;
}

// ******************************************************
//	  Perform shift
// ******************************************************
void CJenksBreaks::MakeShift(int maxId, int targetId, int valueId)
{
	// saving the last shift
	_previousMaxId = maxId;
	_previousTargetId = targetId;

	JenksData* data = &(_values[valueId]);

	// removing from max class
	_classes[maxId].value -= data->value;
	_classes[maxId].square -= data->square;
	_classes[maxId].count -= 1;
	_classes[maxId].RefreshStandardDeviations();
	
	// passing to target class
	_classes[targetId].value += data->value;
	_classes[targetId].square += data->square;
	_classes[targetId].count += 1;
	_classes[targetId].RefreshStandardDeviations();
	
	// mark that the value was passed
	_values[valueId].classId = targetId;
}
#pragma endregion

#pragma region Dynamic programming
// **********************************************************
//    Solves the problem using dynamic programming; 
//    Knapsack-like algorithm
// **********************************************************
std::vector<int>* CJenksBreaks::SolveAsDP(std::vector<double>& data, int numClasses)
{
	std::vector<std::vector<int>> chosen;
	std::vector<std::vector<float>> values;
	
	size_t numValues = data.size();
	chosen.resize(numValues + 1);
	values.resize(numValues + 1);

	for (size_t i = 0; i <= numValues; i++)
	{
		chosen[i].resize(numClasses);
		values[i].resize(numClasses, FLT_MAX);
	}

	for (int j = 0; j < numClasses; j++)
		values[0][j] = 0.0;
	
	for(size_t l = 1; l <= numValues; l++)
	{
		double s1 = 0.0;  // sum of values
		double s2 = 0.0;  // sum of squares
		int w = 0;		  // count
		
	    for(size_t m = 1; m <= l; m++)
	    {
			int i = l - m;
			double val = data[i];
			s2 += val * val;
			s1 += val;
			w++;
			double SSD = s2 - (s1 * s1) / w;   // calculating SSD of elements from l to 0
			
			for(int j = 0; j < numClasses; j++)
			{
				double newVal = values[i][j - 1] + SSD;
 				if(newVal <= values[l][j])		// if new class is better than previous than let's write it
				{
					values[l][j] = (float)newVal;
					chosen[l][j] = i;
				}
			}
	    }
	}
	
	// building result
	int k = numValues;
    std::vector<int>* result = new std::vector<int>();
	result->resize(numClasses);
	for(int j = result->size() - 1; j >= 1; j--)
    {
        int id = chosen[k][j] - 1;
        (*result)[j - 1] = id;
        k = id;
    }
	return result;
}
#pragma endregion

#pragma region Testing
// *****************************************************
//		BuildEqualBreaks()
// *****************************************************
// For testing only
std::vector<int>* CJenksBreaks::BuildEqualBreaks(std::vector<double>& data, int numClasses)
{
	int numValues = data.size();

	std::vector<int>* result = new std::vector<int>;
	result->resize(numClasses + 1);
	(*result)[numClasses] = numValues;
	
	bool equalIntervals = true;
	if (equalIntervals)
	{
		double step = (data[numValues - 1] - data[0])/(numClasses);
		int cnt = 0;
		for (int i = 0; i < numValues; i++)	  // equal intervals
		{
			if (data[i] > step * cnt)
			{
				cnt++;
				if (cnt > numClasses) break;
				(*result)[cnt] = i;
			}
		}
	}
	else
	{
		// ESRI breaks
		/*(*result)[0] = 0;
		(*result)[1] = 387;
		(*result)[2] = 1261;
		(*result)[3] = 2132;
		(*result)[4] = 2698;
		(*result)[5] = 2890;
		(*result)[6] = 2996;
		(*result)[7] = 3064;
		(*result)[8] = 3093;
		(*result)[9] = 3107;*/
	}

	double s1,s2,w,SSD;
	SSD = 0;
	for (int i = 1; i< numClasses + 1; i++)
	{
		int low = (i == 1) ? 0 : (*result)[i-1];
		int high = (i == numClasses) ? numValues : (*result)[i] -1;

		s2 = s1 = w = 0;
		for (int j = low; j < high; j++)
		{
			double val = data[j];
			s2 += val * val;
			s1 += val;
			w++;
		}
		if (w != 0.0)
			SSD += s2 - (s1 * s1) / w;
	}

	return result;
}
#pragma endregion