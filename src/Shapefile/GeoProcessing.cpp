#include "stdafx.h"
#include "GeoProcessing.h"
#include "ShapefileHelper.h"
#include "FieldHelper.h"
#include "Shapefile.h"

// ****************************************************************
//		CopyFields()
// ****************************************************************
void GeoProcessing::CopyFields(IShapefile* sfSubject, IShapefile* sfOverlay, IShapefile* sfResult, map<long, long>& fieldMap, bool mergeFields)
{
	// fields of the subject shapefile
	LONG numFields, position;
	VARIANT_BOOL vbretval;
	sfSubject->get_NumFields(&numFields);
	ShapefileHelper::CopyFields(sfSubject, sfResult);

	// passing the fields of overlay shapefile
	if (sfOverlay)
	{

		LONG numFields2;
		sfOverlay->get_NumFields(&numFields2);
		for (long i = 0; i < numFields2; i++)
		{
			IField * field1 = NULL;
			sfOverlay->get_Field(i, &field1);

			// checking whether we have such field already
			bool found = false;
			if (mergeFields)
			{
				for (int j = 0; j < numFields; j++)
				{
					IField * field2 = NULL;
					sfResult->get_Field(j, &field2);
					if (FieldHelper::FieldsAreEqual(field1, field2))
					{
						fieldMap[i] = j;
						found = true;
					}
					field2->Release();
				}
			}

			if (!found)
			{
				IField* fieldNew = NULL;
				field1->Clone(&fieldNew);
				sfResult->get_NumFields(&position);
				sfResult->EditInsertField(fieldNew, &position, NULL, &vbretval);
				fieldNew->Release();

				fieldMap[i] = position;
			}
			field1->Release();
		}
		FieldHelper::UniqueFieldNames(sfResult);
	}
}

