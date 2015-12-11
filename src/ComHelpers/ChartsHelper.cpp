#include "stdafx.h"
#include "ChartsHelper.h"

// *******************************************************************
//		ReadChartFields()
// *******************************************************************
// Fills array with data values from the selected fields
bool ChartsHelper::ReadChartFields(IShapefile* sf, std::vector<double*>* values)
{
	if (!sf) return false;

	struct FieldIndex
	{
		FieldType type;
		int index;
	};

	long numShapes;
	sf->get_NumShapes(&numShapes);

	CComPtr<ICharts> charts = NULL;
	sf->get_Charts(&charts);

	long numBars;
	charts->get_NumFields(&numBars);

	if (numBars == 0) return false;

	// reading types of fields
	std::vector<FieldIndex> fields;
	for (int j = 0; j < numBars; j++)
	{
		long fieldIndex;
		CComPtr<IChartField> chartField = NULL;
		charts->get_Field(j, &chartField);
		if (chartField)
		{
			chartField->get_Index(&fieldIndex);
		}

		//m_charts->get_FieldIndex(j, &fieldIndex);
		IField* fld = NULL;
		sf->get_Field(fieldIndex, &fld);
		if (fld)
		{
			FieldIndex ind;
			fld->get_Type(&ind.type);
			ind.index = fieldIndex;
			fields.push_back(ind);
			fld->Release(); fld = NULL;
		}
		else
		{
			FieldIndex ind;
			ind.index = -1;
			ind.type = STRING_FIELD;
			fields.push_back(ind);
		}
	}

	// reading data
	VARIANT val;
	VariantInit(&val);

	values->resize(numShapes);
	for (int i = 0; i < numShapes; i++)
	{
		(*values)[i] = new double[numBars];
		double* arr = (*values)[i];
		for (int j = 0; j < numBars; j++)
		{
			if (fields[j].type == INTEGER_FIELD || fields[j].type == DOUBLE_FIELD)
			{
				sf->get_CellValue(fields[j].index, i, &val);
				dVal(val, arr[j]);
			}
			else
			{
				arr[j] = 0.0;
			}
		}
	}
	VariantClear(&val);
	return true;
}

// *******************************************************************
//		ReadChartField()
// *******************************************************************
bool ChartsHelper::ReadChartField(IShapefile* sf, std::vector<double>* values, int fieldIndex)
{
	if (!sf) return false;

	IField* fld = NULL;
	sf->get_Field(fieldIndex, &fld);
	if (!fld)
		return false;
	FieldType type;
	fld->get_Type(&type);
	if (type != INTEGER_FIELD && type != DOUBLE_FIELD)
		return false;
	fld->Release(); fld = NULL;

	// reading data
	VARIANT val;
	VariantInit(&val);

	long numShapes;
	sf->get_NumShapes(&numShapes);
	values->resize(numShapes);
	for (int i = 0; i < numShapes; i++)
	{
		sf->get_CellValue(fieldIndex, i, &val);
		dVal(val, (*values)[i]);
	}
	VariantClear(&val);
	return true;
}

// *************************************************************
//		GetCollisionBuffer()
// *************************************************************
long ChartsHelper::GetCollisionBuffer(ICharts* charts)
{
	long collisionBuffer;
	charts->get_CollisionBuffer(&collisionBuffer);
	return collisionBuffer;
}

// *************************************************************
//		GetOffsetX()
// *************************************************************
long ChartsHelper::GetOffsetX(ICharts* charts)
{
	long offsetX;
	charts->get_OffsetX(&offsetX);
	return offsetX;
}

// *************************************************************
//		GetOffsetY()
// *************************************************************
long ChartsHelper::GetOffsetY(ICharts* charts)
{
	long offsetY;
	charts->get_OffsetY(&offsetY);
	return offsetY;
}
