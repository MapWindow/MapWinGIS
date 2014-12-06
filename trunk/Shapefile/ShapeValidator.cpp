#include "stdafx.h"
#include "ShapeValidator.h"
#include "ShapeValidationInfo.h"
#include "Shapefile.h"

// ***************************************************************
//		Validate()
// ***************************************************************
IShapeValidationInfo* ShapeValidator::Validate(IShapefile* isf, tkShapeValidationMode validationMode, 
			tkShapeValidationType validationType, CString className, CString methodName, CString parameterName,
			ICallback* callback, BSTR& key, bool selectedOnly, bool reportOnly)
{
	if (!isf)
		return NULL;

	tkShapefileSourceType sourceType;
	isf->get_SourceType(&sourceType);
	if (sourceType == tkShapefileSourceType::sstUninitialized)
		return NULL;
	
	CShapefile* sf = (CShapefile*)isf;

	VARIANT_BOOL vb;
	
	IShapeValidationInfo* iinfo = NULL;
	ComHelper::CreateInstance(idShapeValidationInfo, (IDispatch**)&iinfo);
	CShapeValidationInfo* info = (CShapeValidationInfo*)iinfo;
	info->className = className;
	info->methodName = methodName;
	info->parameterName = parameterName;
	info->validationType = validationType;
	info->validationMode = validationMode;
	info->validationStatus = tkShapeValidationStatus::Valid;

	if (validationMode == NoValidation)
	{
		info->validationStatus = tkShapeValidationStatus::WasntValidated;
		info->wereInvalidCount = -1;
		info->stillInvalidCount = -1;
		info->fixedCount = -1;
		info->skippedCount = -1;
	}
	else
	{
		info->wereInvalidCount = 0;
		info->stillInvalidCount = 0;
		info->fixedCount = 0;
		info->skippedCount = 0;

		VARIANT_BOOL canEdit;
		isf->get_EditingShapes(&canEdit);
		if (validationType == svtInput)
			canEdit = VARIANT_FALSE;
		if (!canEdit)
		{
			sf->CreateValidationList(selectedOnly);
		}

		long numShapes;
		isf->get_NumShapes(&numShapes);

		bool checkEditMode = true;

		long percent = 0;
		for (int i = numShapes - 1; i >= 0 ; i--)
		{
			CallbackHelper::Progress(callback, numShapes - 1 - i, numShapes, "Validating shapes...", key, percent);
			
			IShape* shp = NULL;
			isf->get_Shape(i, &shp);
			
			shp->get_IsValid(&vb);
			if (vb)
			{
				shp->Release();
			}
			else
			{
				info->wereInvalidCount++;

				if (reportOnly)
				{
					info->stillInvalidCount++;
					shp->Release();
				}
				else if (validationMode == AbortOnErrors)	// retreat on the first error
				{
					info->validationStatus = tkShapeValidationStatus::OperationAborted;
					shp->Release();
					goto stop_operation;
				}
				else
				{
					IShape* fixedShape = NULL;
					shp->FixUp(&fixedShape);
					shp->Release();

					if (fixedShape)
					{
						info->fixedCount++;
						sf->SetValidatedShape(i, ShapeValidationStatus::Fixed, fixedShape);
					}
					else
					{
						info->stillInvalidCount++;
						switch(validationMode)
						{
							case TryFixProceedOnFailure:
								// do nothing
								break;
							case TryFixSkipOnFailure:
								if (canEdit) {
									sf->EditDeleteShape(i, &vb);
								}
								else {
									sf->SetValidatedShape(i, ShapeValidationStatus::Skip);
								}
								break;
							case TryFixAbortOnFailure:
								info->validationStatus = tkShapeValidationStatus::OperationAborted;
								goto stop_operation;
						}
					}
				}
			}
		}
	}

stop_operation:		
	if (info->validationStatus == OperationAborted)
	{
		info->stillInvalidCount = 1;
		info->wereInvalidCount = 1;
		sf->ClearValidationList();
	}
	else
	{
		if (info->stillInvalidCount > 0)
		{
			info->validationStatus = InvalidReturned;
		}
		else if (info->skippedCount > 0)
		{
			info->validationStatus = InvalidSkipped;
		}
		else if (info->fixedCount > 0)
		{
			info->validationStatus = InvalidFixed;
		}
	}
	return iinfo;
}

