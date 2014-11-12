#include "stdafx.h"
#include "FieldHelper.h"
#include <set>
#include "ShapefileHelper.h"

// *****************************************************************
//	   FindNewShapeID()
// *****************************************************************
long FieldHelper::FindNewShapeID(IShapefile* sf, long FieldIndex)
{
	if (!sf) return 0;
	CComPtr<ITable> table = NULL;
	sf->get_Table(&table);
	if (!table) return 0;

	long rt = -1;
	long lo = 0;

	long size = ShapefileHelper::GetNumShapes(sf);
	for (long i = 0; i < size - 1; i++)
	{
		CComVariant pVal;
		table->get_CellValue(FieldIndex, i, &pVal);
		lVal(pVal, lo);
		if (lo > rt) rt = lo;
	}
	return rt + 1;
}

//*********************************************************************
//*						UniqueFieldNames()				              
//*********************************************************************
// Makes name of fields in the table unique. In case of duplicated names adds _# to them.
bool FieldHelper::UniqueFieldNames(IShapefile* sf)
{
	VARIANT_BOOL editing;
	USES_CONVERSION;

	// Do we need edit mode for editing of the field names?
	// Yes we do, shapelib doesn't allow it otherwise ;)
	sf->get_EditingShapes(&editing);
	if (!editing)
		return false;

	long numFields;
	sf->get_NumFields(&numFields);

	set<CString> fields;

	for (long i = 0; i< numFields; i++)
	{
		BSTR name;
		IField* fld;
		sf->get_Field(i, &fld);
		fld->get_Name(&name);

		if (fields.find(OLE2CA(name)) == fields.end())
		{
			fields.insert(OLE2CA(name));
		}
		else
		{
			bool found = false;
			for (int j = 0; !found; j++)
			{
				CString temp = OLE2CA(name);
				temp.AppendFormat("_%d", j);
				if (fields.find(temp) == fields.end())
				{
					fields.insert(temp);
					name = temp.AllocSysString();
					fld->put_Name(name);
					found = true;
				}
			}
		}
		fld->Release();
	}
	fields.clear();
	return true;
}

// ****************************************************************
//		FieldsAreEqual()
// ****************************************************************
bool FieldHelper::FieldsAreEqual(IField* field1, IField* field2)
{
	if (!field1 || !field2)
		return false;
	
	BSTR name1, name2;
	field1->get_Name(&name1);
	field2->get_Name(&name2);

	USES_CONVERSION;
	CString s1 = OLE2CA(name1);
	CString s2 = OLE2CA(name2);

	bool equal = s1 == s2;
	SysFreeString(name1);
	SysFreeString(name2);

	if (!equal)
	{
		return false;
	}
	else
	{
		FieldType type1, type2;
		field1->get_Type(&type1);
		field2->get_Type(&type2);

		if (type1 != type2)
		{
			return false;
		}
		else
		{
			if (type1 == INTEGER_FIELD)
			{
				return true;
			}
			else if (type1 == STRING_FIELD)
			{
				long width1, width2;
				field1->get_Width(&width1);
				field2->get_Width(&width2);
				return width1 == width2;
			}
			else // DOUBLE_FIELD
			{
				/*long precision1, precision2;
				field1->get_Precision(&precision1);
				field2->get_Precision(&precision2);
				return precision1 == precision2;*/
				return true;
			}
		}
	}
}
