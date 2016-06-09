#include "stdafx.h"
#include "FieldClassification.h"
#include <set>
#include <algorithm>
#include <iterator>
#include "JenksBreaks.h"

// *****************************************************************
//			GetMinValue()
// *****************************************************************
void FieldClassification::GetMinValue(vector<VARIANT*>& srcValues, CComVariant& result, bool seekMin)
{
	CComVariant min, val;
	for (unsigned long i = 0; i < srcValues.size(); i++)
	{
		val.Copy(srcValues[i]);
		if (i == 0)	min = val;
		else {
			if (seekMin) {
				if (val < min)	min = val;
			}
			else {
				if (val > min)	min = val;
			}
		}
		val.Clear();
	}
	result.Copy(&min);
}

// *****************************************************************
//			GenerateCategories()
// *****************************************************************
vector<CategoriesData>* FieldClassification::GenerateCategories(CString fieldName, FieldType fieldType, 
		vector<VARIANT*>& srcValues, tkClassificationType ClassificationType, 
		long numClasses, long& errorCode)
{
	CComVariant minValue, maxValue;
	minValue.vt = VT_EMPTY;
	maxValue.vt = VT_EMPTY;

	long numShapes = srcValues.size();

	/* we won't define intervals for string values */
	if (ClassificationType != ctUniqueValues && fieldType == STRING_FIELD)
	{
		errorCode = tkNOT_UNIQUE_CLASSIFICATION_FOR_STRINGS;
		return NULL;
	}

	if ((numClasses <= 0 || numClasses > 1000) && (ClassificationType != ctUniqueValues))
	{
		errorCode = tkTOO_MANY_CATEGORIES;
		return NULL;
	}

	if (ClassificationType == ctStandardDeviation)
	{
		errorCode = tkINVALID_PARAMETER_VALUE;
		return NULL;
	}

	// natural breaks aren't designed to work otherwise
	if (numShapes < numClasses && ClassificationType == ctNaturalBreaks)
	{
		numClasses = numShapes;
	}

	// values in specified range should be classified
	bool useRange = minValue.vt != VT_EMPTY && maxValue.vt != VT_EMPTY && fieldType == DOUBLE_FIELD;

	if (useRange) //fieldType == DOUBLE_FIELD)
	{
		double max, min;
		dVal(minValue, min);
		dVal(maxValue, max);
		minValue.vt = VT_R8;
		maxValue.vt = VT_R8;
		minValue.dblVal = min;
		maxValue.dblVal = max;
	}

	//bool useRange = minValue.vt == VT_R8 && maxValue.vt == VT_R8 && fieldType != STRING_FIELD;

	std::vector<CategoriesData>* result = new std::vector<CategoriesData>;
	if (ClassificationType == ctUniqueValues)
	{
		std::set<CComVariant> dict;
		CComVariant val;

		for (long i = 0; i < numShapes; i++)
		{
			VariantCopy(&val, srcValues[i]);
			if (useRange && (val.dblVal < minValue.dblVal || val.dblVal > maxValue.dblVal))
				continue;

			if (dict.find(val) == dict.end())
				dict.insert(val);
		}
		/* creating categories */
		std::vector<CComVariant> values;
		copy(dict.begin(), dict.end(), inserter(values, values.end()));

		for (int i = 0; i < (int)values.size(); i++)
		{
			CategoriesData data;
			data.minValue = values[i];
			data.maxValue = values[i];
			result->push_back(data);
		}
		dict.clear();
		values.clear();
	}
	else if (ClassificationType == ctEqualSumOfValues)
	{
		CComVariant val;

		// sorting the values
		std::vector<double> values;
		double totalSum = 0, dValue;
		for (int i = 0; i < numShapes; i++)
		{
			VariantCopy(&val, srcValues[i]);
			if (useRange && (val.dblVal < minValue.dblVal || val.dblVal > maxValue.dblVal))
				continue;

			dVal(val, dValue); val.Clear();
			values.push_back(dValue);
			totalSum += dValue;
		}
		sort(values.begin(), values.end());

		double step = totalSum / (double)numClasses;
		int index = 1;
		double sum = 0;

		for (int i = 0; i < (int)values.size(); i++)
		{
			sum += values[i];
			if (sum >= step * (double)index || i == numShapes - 1)
			{
				CategoriesData data;

				if (index == numClasses)
					data.maxValue = values[values.size() - 1];
				else if (i != 0)
					data.maxValue = (values[i] + values[i - 1]) / 2;
				else
					data.maxValue = values[0];

				if (index == 1)
					data.minValue = values[0];
				else
					data.minValue = (*result)[result->size() - 1].maxValue;

				result->push_back(data);
				index++;
			}
		}
	}
	else if (ClassificationType == ctEqualIntervals)
	{
		CComVariant vMin, vMax;

		if (useRange)
		{
			vMin = minValue;
			vMax = maxValue;
		}
		else
		{
			GetMinValue(srcValues, vMin, true);
			GetMinValue(srcValues, vMax, false);
		}

		double dMin, dMax;
		dVal(vMin, dMin); dVal(vMax, dMax);
		vMin.Clear(); vMax.Clear();

		/*	creating classes */
		double dStep = (dMax - dMin) / (double)numClasses;
		while (dMin < dMax)
		{
			CategoriesData data;
			data.minValue = dMin;
			data.maxValue = dMin + dStep;
			result->push_back(data);

			dMin += dStep;
		}
	}
	else if (ClassificationType == ctEqualCount)
	{
		CComVariant vMin, vMax;
		if (useRange)
		{
			vMin = minValue;
			vMax = maxValue;
		}
		else
		{
			GetMinValue(srcValues, vMin, true);
			GetMinValue(srcValues, vMax, false);
		}

		double dMin, dMax;
		dVal(vMin, dMin); dVal(vMax, dMax);
		vMin.Clear(); vMax.Clear();

		// sorting the values
		std::vector<double> values;
		for (int i = 0; i < numShapes; i++)
		{
			VariantCopy(&vMin, srcValues[i]);
			dVal(vMin, dMin); vMin.Clear();
			values.push_back(dMin);
		}
		sort(values.begin(), values.end());

		/*	creating classes */
		int i = 0;
		int count = numShapes / numClasses;

		for (int i = 0; i < numShapes; i += count)
		{
			dMin = values[i];
			if (i + count < numShapes)
				dMax = values[i + count];
			else
				dMax = values[numShapes - 1];

			CategoriesData data;
			data.minValue = dMin;
			data.maxValue = dMax;
			result->push_back(data);
		}
		values.clear();
	}
	else if (ClassificationType == ctNaturalBreaks)
	{
		CComVariant vMin; double dMin;
		// sorting the values
		std::vector<double> values;
		for (int i = 0; i < numShapes; i++)
		{
			VariantCopy(&vMin, srcValues[i]);

			if (useRange && (vMin.dblVal < minValue.dblVal || vMin.dblVal > maxValue.dblVal))
				continue;

			dVal(vMin, dMin); vMin.Clear();
			values.push_back(dMin);
		}
		sort(values.begin(), values.end());

		CJenksBreaks breaks(&values, numClasses);
		if (breaks.Initialized())
		{
			breaks.Optimize();
			std::vector<long>* startIndices = breaks.get_Results();
			//std::vector<int>* startIndices = breaks.TestIt(&values, numClasses);

			if (startIndices)
			{
				for (unsigned int i = 0; i < startIndices->size(); i++)
				{
					CategoriesData data;
					data.minValue = values[(*startIndices)[i]];
					if (i == startIndices->size() - 1)
						data.maxValue = values[values.size() - 1];
					else
						data.maxValue = values[(*startIndices)[i + 1]];

					result->push_back(data);
				}
				delete startIndices;
			}
		}
	}
	
	// TODO: implement this as well; then it can be used in table class
	//else if (ClassificationType == ctStandardDeviation)

	// ------------------------------------------------------
	//		generating text expressions
	// ------------------------------------------------------
	if (ClassificationType == ctUniqueValues)
	{
		USES_CONVERSION;
		for (int i = 0; i < (int)result->size(); i++)
		{
			//CString strExpression;
			CString strValue;
			CComVariant* val = &(*result)[i].minValue;
			switch (val->vt)
			{
				case VT_BSTR:
					strValue = OLE2CA(val->bstrVal);
					(*result)[i].name = strValue;
					(*result)[i].expression = "[" + fieldName + "] = \"" + strValue + "\"";
					break;
				case VT_R8:
					strValue.Format("%g", val->dblVal);
					(*result)[i].name = strValue;
					(*result)[i].expression = "[" + fieldName + "] = " + strValue;
					break;
				case VT_I4:
					strValue.Format("%i", val->lVal);
					(*result)[i].name = strValue;
					(*result)[i].expression = "[" + fieldName + "] = " + strValue;
					break;
			}
		}
	}
	else //if (ClassificationType == ctEqualIntervals || ClassificationType == ctEqualCount)
	{
		// in case % is present, we need to put to double it for proper formatting
		fieldName.Replace("%", "%%");

		for (int i = 0; i < (int)result->size(); i++)
		{
			CategoriesData* data = &((*result)[i]);

			CString strExpression, strName, sFormat;

			if (i == 0)
			{
				data->minValue.dblVal = floor(data->minValue.dblVal);
			}
			else if (i == result->size() - 1)
			{
				data->maxValue.dblVal = ceil(data->maxValue.dblVal);
			}

			CString upperBound = (i == result->size() - 1) ? "<=" : "<";

			switch (data->minValue.vt)
			{
				case VT_R8:
					sFormat = "%g";
					data->name = Utility::FormatNumber(data->minValue.dblVal, sFormat) + " - " + Utility::FormatNumber(data->maxValue.dblVal, sFormat);
					data->expression.Format("[" + fieldName + "] >= %f AND [" + fieldName + "] " + upperBound + " %f", data->minValue.dblVal, data->maxValue.dblVal);
					break;
				case VT_I4:
					sFormat = "%i";
					data->name = Utility::FormatNumber(data->minValue.dblVal, sFormat) + " - " + Utility::FormatNumber(data->maxValue.dblVal, sFormat);
					data->expression.Format("[" + fieldName + "] >= %i AND [" + fieldName + "] " + upperBound + " %i", data->minValue.lVal, data->maxValue.lVal);
					break;
			}
		}
	}

	if (result->size() > 0)
	{
		return result;
	}
	else
	{
		delete result;
		return NULL;
	}
}