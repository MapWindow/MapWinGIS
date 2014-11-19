//********************************************************************************************************
//File name: Shapefile.cpp
//Description: Implementation of the CShapefile (see other cpp files as well)
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
// -------------------------------------------------------------------------------------------------------
// lsu 3-02-2011: split the initial Shapefile.cpp file to make entities of the reasonable size
#pragma once
#include "stdafx.h"
#include "Shapefile.h"
#include "TableClass.h"
#include "Charts.h"
#include "Shape.h"

#pragma region StartEditing

// ************************************************************
//		StartEditingShapes()
// ************************************************************
STDMETHODIMP CShapefile::StartEditingShapes(VARIANT_BOOL StartEditTable, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;
	
	bool callbackIsNull = (_globalCallback == NULL);
	if(cBack != NULL && _globalCallback == NULL)
	{
		_globalCallback = cBack;	
		_globalCallback->AddRef();
	}
	
	if( _table == NULL || _sourceType == sstUninitialized)
	{	
		// Error: shapefile is not initialized
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
	}
	else if( _isEditingShapes )
	{	
		// editing is going on already
		*retval = VARIANT_TRUE;
	}
	else if (_writing) 
	{
		ErrorMessage(tkSHP_READ_VIOLATION);
	}
	else
	{
		// quad tree generation
		IExtents * box = NULL;	
		double xm,ym,zm,xM,yM,zM;
		if(_useQTree)
		{
			if (_qtree)
			{
				delete _qtree;
				_qtree = NULL;
			}
			IExtents * box = NULL;	
			this->get_Extents(&box);
			box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
			box->Release();
			_qtree = new QTree(QTreeExtent(xm,xM,yM,ym));
		}
		
		// reading shapes into memory
		IShape * shp = NULL;
		_lastErrorCode = tkNO_ERROR;	
		long percent = 0, newpercent = 0; 
		
		int size = (int)_shapeData.size();
		for( int i = 0; i < size; i++)
		{	
			this->get_Shape(i, &shp);

			if( _lastErrorCode != tkNO_ERROR )
			{	
				ErrorMessage(_lastErrorCode);
				ReleaseMemoryShapes();
				return S_OK;
			}
			
			_shapeData[i]->shape = shp;
			_shapeData[i]->originalIndex = i;
			this->QuickExtentsCore(i, &xm,&ym,&xM,&yM);

			// Neio 2009 07 21 QuadTree
			if(_useQTree)
			{
				QTreeNode node;
				node.Extent.left = xm;
				node.Extent.right= xM;
				node.Extent.top = yM;
				node.Extent.bottom = ym;
				node.index = i;
				_qtree->AddNode(node);
			}
			
			Utility::DisplayProgress(_globalCallback, i, size, "Reading shapes into memory", _key, percent);
		}
		Utility::DisplayProgressCompleted(_globalCallback);

		*retval = VARIANT_TRUE;
	
		// releasing data for the fast non-edit mode
		if (_fastMode )
		{
			for (unsigned int i = 0; i < _shapeData.size(); i++)
			{
				//ASSERT(_shapeData[i]->fastData);
				if (_shapeData[i]->fastData)
				{
					delete _shapeData[i]->fastData;
					_shapeData[i]->fastData = NULL;
				}
			}
		}

		// ------------------------------------------
		// reading table into memory
		// ------------------------------------------
		if(StartEditTable != VARIANT_FALSE)
		{
			StartEditingTable(_globalCallback,retval);
		}
		
		if (*retval == VARIANT_FALSE)
		{
			ErrorMessage(_table->get_LastErrorCode(&_lastErrorCode));
			ReleaseMemoryShapes();
		}
		else
		{
			_isEditingShapes = TRUE;
		}
	}

	if (callbackIsNull)
	{
		_globalCallback = NULL;
	}
	return S_OK;
}
#pragma endregion

#pragma region StopEditing

// ********************************************************
//		StopEditingShapes()
// ********************************************************
STDMETHODIMP CShapefile::StopEditingShapes(VARIANT_BOOL ApplyChanges, VARIANT_BOOL StopEditTable, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;

	bool callbackIsNull = (_globalCallback == NULL);
	if(cBack != NULL && _globalCallback == NULL)
	{
		_globalCallback = cBack;	
		_globalCallback->AddRef();
	}
	
	if( _table == NULL || _sourceType == sstUninitialized)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED, cBack);
	}
	else if( _isEditingShapes == FALSE )
	{	
		*retval = VARIANT_TRUE;
	}
	else if ( _writing )
	{
		ErrorMessage(tkSHP_WRITE_VIOLATION, cBack);
	}
	else if ( _sourceType == sstInMemory )
	{
		// shapefile wasn't saved before
		if(_shpfileName.GetLength() > 0)
		{
			this->Save(cBack, retval);
			
			if (*retval)
			{
				_isEditingShapes = VARIANT_FALSE;
			}
		}
	}
	else
	{
		USES_CONVERSION;

		if( ApplyChanges )
		{	
			_writing = true;
			
			// verify Shapefile Integrity
			if( VerifyMemShapes(cBack) == FALSE )
			{	
				// error Code is set in function
			}
			else
			{
				_shpfile = _wfreopen(_shpfileName, L"wb+", _shpfile);
				_shxfile = _wfreopen(_shxfileName, L"wb+",_shxfile);

				if( _shpfile == NULL || _shxfile == NULL )
				{	
					if( _shxfile != NULL )
					{	
						fclose( _shxfile );
						_shxfile = NULL;
						
					}
					if( _shpfile != NULL )
					{	
						fclose( _shpfile );
						_shpfile = NULL;
						ErrorMessage(tkCANT_OPEN_SHP, cBack);
					}
				}
				else
				{
					// force computation of Extents
					VARIANT_BOOL vbretval;
					this->RefreshExtents(&vbretval);

					WriteShp(_shpfile,cBack);
					WriteShx(_shxfile,cBack);

					_shpfile = _wfreopen(_shpfileName,L"rb+", _shpfile);
					_shxfile = _wfreopen(_shxfileName,L"rb+",_shxfile);
					
					if( _shpfile == NULL || _shxfile == NULL )
					{	
						if( _shxfile != NULL )
						{	
							fclose( _shxfile );
							_shxfile = NULL;
							ErrorMessage(tkCANT_OPEN_SHX, cBack);
						}
						if( _shpfile != NULL )
						{	
							fclose( _shpfile );
							_shpfile = NULL;
							ErrorMessage(tkCANT_OPEN_SHP, cBack);
						}
					}
					else
					{	
						_isEditingShapes = FALSE;
						ReleaseMemoryShapes();
						*retval = VARIANT_TRUE;

						if(StopEditTable != VARIANT_FALSE)
							StopEditingTable(ApplyChanges,cBack,retval);
					}
				}
			}
			_writing = false;
		}
		else
		{	
			// discard the changes
			_isEditingShapes = FALSE;
			ReleaseMemoryShapes();

			// reload the shx file
			this->ReadShx();

			if(StopEditTable != VARIANT_FALSE)
			{
				StopEditingTable(ApplyChanges,cBack,retval);
			}

			RestoreShapeRecordsMapping();

			*retval = VARIANT_TRUE;
		}
		
		// restoring fast mode
		if (*retval == VARIANT_TRUE && _fastMode)
		{
			this->put_FastMode(VARIANT_FALSE);
			this->put_FastMode(VARIANT_TRUE);
		}
	}
	
	// restoring callback state
	if (callbackIsNull)
		_globalCallback = NULL;

	return S_OK;
}

// ***********************************************************
//		RestoreShapeRecordsMapping()
// ***********************************************************
void CShapefile::RestoreShapeRecordsMapping()
{
	// if in memory records still match the disk ones
	bool clearRecords = _shpOffsets.size() != _shapeData.size();
	for (size_t i = 0; i < _shapeData.size(); i++)
	{
		if (_shapeData[i]->originalIndex != i) {
			clearRecords = true;
			break;
		}
	}

	// clear in-memory shape records as mapping between disk shapefile and in-memory one is lost
	if (clearRecords)
	{
		for (unsigned int i = 0; i < _shapeData.size(); i++)
			delete _shapeData[i];	// all the releasing done in the destructor
		_shapeData.clear();
		_shapeData.reserve(_shpOffsets.size());
		for (size_t i = 0; i < _shpOffsets.size(); i++)
		{
			_shapeData.push_back(new ShapeData());
		}

		// reapply categories
		long categoriesCount;
		_categories->get_Count(&categoriesCount);
		if (categoriesCount > 0) {
			_categories->ApplyExpressions();
		}
	}
}

#pragma endregion

#pragma region Operations

// ***********************************************************
//		RegisterNewShape()
// ***********************************************************
// Must be called after inserting or swapping shape in shape vector
void CShapefile::RegisterNewShape(IShape* Shape, long ShapeIndex)
{
	// shape must have correct underlying data structure
	// shapes not bound to shapefile all use CShapeWrapperCOM underlying class
	// and if fast mode is set to true, CShapeWrapper class is expected
	if ((this->_fastMode ? true : false) != ((CShape*)Shape)->get_fastMode())
	{
		((CShape*)Shape)->put_fastMode(this->_fastMode?true:false);
	}

	VARIANT_BOOL bSynchronized;
	_labels->get_Synchronized(&bSynchronized);

	// updating labels
	if (_table) 
	{
		double x = 0.0, y = 0.0, rotation = 0.0;
		VARIANT_BOOL vbretval;

		bool chartsExist = ((CCharts*)_charts)->_chartsExist;
		if (bSynchronized || chartsExist)
		{
			// position
			tkLabelPositioning positioning;
			_labels->get_Positioning(&positioning);

			tkLineLabelOrientation orientation;
			_labels->get_LineOrientation(&orientation);
			
			((CShape*)Shape)->get_LabelPosition(positioning, x, y, rotation, orientation);
		}
		
		if (bSynchronized)
		{
			// it doesn't make sense to recalculate expression as dbf cells are empty all the same
			CString text;
			_labels->InsertLabel(ShapeIndex, A2BSTR(text), x, y, rotation, -1, &vbretval);
		}

		if (chartsExist)
		{
			if (!_shapeData[ShapeIndex]->chart)
			{
				_shapeData[ShapeIndex]->chart = new CChartInfo();
				_shapeData[ShapeIndex]->chart->x = x;
				_shapeData[ShapeIndex]->chart->y = y;
			}
		}
	}
	
	// extending the bounds of the shapefile we don't care if the bounds became less
	// it's necessary to call RefreshExtents in this case, for zoom to layer working right
	IExtents * box;
	Shape->get_Extents(&box);
	double xm,ym,zm,xM,yM,zM;
	box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
	box->Release();

	if (_shapeData.size() == 1)
	{
		_minX = xm;
		_maxX = xM;
		_minY = ym;
		_maxY = yM;
		_minZ = zm;
		_maxZ = zM;
	}
	else
	{
		if (xm < _minX) _minX = xm;
		if (xM > _maxX) _maxX = xM;
		if (ym < _minY) _minY = ym;
		if (yM > _maxY) _maxY = yM;
		if (zm < _minZ) _minZ = zm;
		if (zM > _maxZ) _maxZ = zM;
	}

	// Neio 07/23/2009 - add qtree
	if(_useQTree)
	{
		QTreeNode node;
		node.index = ShapeIndex;
		node.Extent.left = xm;
		node.Extent.right = xM;
		node.Extent.top = yM;
		node.Extent.bottom = ym;
		_qtree->AddNode(node);
	}
}

// ***********************************************************
//		EditUpdateShape()
// ***********************************************************
// Substitutes one shape with another without formal remove/add call,
// so that attribute table will be intact
STDMETHODIMP CShapefile::EditUpdateShape(long shapeIndex, IShape* shpNew, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;
	if (!_isEditingShapes) 
	{
		ErrorMessage(tkSHPFILE_NOT_IN_EDIT_MODE);
		return S_FALSE;
	}

	if (shapeIndex < 0 || shapeIndex >= (long)_shapeData.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_FALSE;
	}
	else
	{
		Utility::put_ComReference(shpNew, (IDispatch**)&_shapeData[shapeIndex]->shape, false);
		ReregisterShape(shapeIndex);
		return S_OK;
	}
}

// ***********************************************************
//		UpdateShapeCore()
// ***********************************************************
// should be called when geometry of shape changed
void CShapefile::ReregisterShape(int shapeIndex)
{
	if (!_isEditingShapes) return;

	if (shapeIndex < 0  || shapeIndex >= (int)_shapeData.size())
		return;

	IShape* shp = _shapeData[shapeIndex]->shape;
	_shapeData[shapeIndex]->fastData;
	
	bool fastMode = _fastMode ? true : false;
	if (fastMode != ((CShape*)shp)->get_fastMode())
	{
		((CShape*)shp)->put_fastMode(fastMode);
	}

	IExtents * box;
	shp->get_Extents(&box);
	double xm,ym,zm,xM,yM,zM;
	box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
	box->Release();

	if (_shapeData.size() == 1)
	{
		_minX = xm;
		_maxX = xM;
		_minY = ym;
		_maxY = yM;
		_minZ = zm;
		_maxZ = zM;
	}
	else
	{
		if (xm < _minX) _minX = xm;
		if (xM > _maxX) _maxX = xM;
		if (ym < _minY) _minY = ym;
		if (yM > _maxY) _maxY = yM;
		if (zm < _minZ) _minZ = zm;
		if (zM > _maxZ) _maxZ = zM;
	}

	if(_useQTree)
	{
		_qtree->RemoveNode(shapeIndex);
		
		QTreeNode node;
		node.index = shapeIndex;
		node.Extent.left = xm;
		node.Extent.right = xM;
		node.Extent.top = yM;
		node.Extent.bottom = ym;
		_qtree->AddNode(node);
	}
}

// ***********************************************************
//		EditInsertShape()
// ***********************************************************
STDMETHODIMP CShapefile::EditInsertShape(IShape *Shape, long *ShapeIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;
	
	if(_useValidationList)
	{
		Debug::WriteLine("Error: shape inserted when validation list in action");
	}

 	if( _table == NULL || _sourceType == sstUninitialized )
	{	
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
	}
	else if(!_isEditingShapes)
	{
		ErrorMessage(tkSHPFILE_NOT_IN_EDIT_MODE);
	}
	else
	{
		VARIANT_BOOL isEditingTable;
		_table->get_EditingTable(&isEditingTable);
		
		if(!isEditingTable)
		{
			ErrorMessage(tkDBF_NOT_IN_EDIT_MODE);
		}
		else
		{
			if( Shape == NULL )
			{	
				ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
			}
			else
			{
				ShpfileType shapetype;
				Shape->get_ShapeType(&shapetype);
				
				if( shapetype != SHP_NULLSHAPE && shapetype != _shpfiletype)
				{	
					ErrorMessage(tkINCOMPATIBLE_SHAPEFILE_TYPE);
				}
				else
				{
					// wrong index will be corrected
					if( *ShapeIndex < 0 )
					{
						*ShapeIndex = 0;
					}
					else if( *ShapeIndex > (int)_shapeData.size() )
					{
						*ShapeIndex = _shapeData.size();
					}
					
					// adding the row in table
					_table->EditInsertRow( ShapeIndex, retval );
					
					if( *retval == VARIANT_FALSE )
					{	
						_table->get_LastErrorCode(&_lastErrorCode);
						ErrorMessage(_lastErrorCode);
					}			
					else
					{	
						ShapeData* data = new ShapeData();
						Shape->AddRef();
						data->shape = Shape;
						data->modified = true;
						_shapeData.insert(_shapeData.begin() + *ShapeIndex, data);
						
						RegisterNewShape(Shape, *ShapeIndex);
						
						*retval = VARIANT_TRUE;
					}
					
					((CTableClass*)_table)->set_IndexValue(*ShapeIndex);
				
				}
			}
		}
	}
	return S_OK;
}

// *********************************************************************
//		EditDeleteShape()
// *********************************************************************
STDMETHODIMP CShapefile::EditDeleteShape(long ShapeIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;

	if(_useValidationList)
	{
		Debug::WriteLine("Error: EditDelete called when validation list in action");
	}

	if( _table == NULL || _sourceType == sstUninitialized )
	{	
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
	}
	else if(!_isEditingShapes)
	{
		ErrorMessage(tkSHPFILE_NOT_IN_EDIT_MODE);
	}
	else
	{
		VARIANT_BOOL isEditingTable;
		_table->get_EditingTable(&isEditingTable);
		
		if(!isEditingTable)
		{
			ErrorMessage(tkDBF_NOT_IN_EDIT_MODE);
		}
		else if( ShapeIndex < 0 || ShapeIndex >= (int)_shapeData.size() )
		{	
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
		else
		{
			VARIANT_BOOL vbretval;
			_table->EditDeleteRow( ShapeIndex, &vbretval);
			
			if(!vbretval)
			{	
				_table->get_LastErrorCode(&_lastErrorCode);
				ErrorMessage(_lastErrorCode);
			}			
			else
			{	
				VARIANT_BOOL bSynchronized;
				_labels->get_Synchronized(&bSynchronized);
				if (bSynchronized)
					_labels->RemoveLabel(ShapeIndex, &vbretval);
				
				delete _shapeData[ShapeIndex];
				_shapeData.erase( _shapeData.begin() + ShapeIndex );
				
				// TODO: why haven't we updated QTree?

				*retval = VARIANT_TRUE;
			}
		}
	}
	return S_OK;
}

// ***********************************************************
//		EditClear()
// ***********************************************************
STDMETHODIMP CShapefile::EditClear(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;

	if(_useValidationList)
	{
		Debug::WriteLine("Error: EditClear called when validation list in action");
	}

	if (_table == NULL || _sourceType == sstUninitialized)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
	}
	else if( _isEditingShapes == FALSE )
	{	
		ErrorMessage(tkSHPFILE_NOT_IN_EDIT_MODE);
	}
	else
	{
		VARIANT_BOOL isEditingTable;
		_table->get_EditingTable(&isEditingTable);
	
		if( isEditingTable == FALSE )
		{	
			ErrorMessage(tkDBF_NOT_IN_EDIT_MODE);
		}
		else
		{	
			_table->EditClear(retval);
			if( *retval == VARIANT_FALSE )
			{	
				_table->get_LastErrorCode(&_lastErrorCode);
				ErrorMessage(_lastErrorCode);
			}
			
			for (unsigned int i = 0; i < _shapeData.size(); i++)
			{
				delete _shapeData[i];	// all the releasing done in the destructor
			}
			_shapeData.clear();
			
			if(_useQTree == VARIANT_TRUE)
			{
				delete _qtree;
				_qtree = NULL;
			}

			// deleting the labels
			VARIANT_BOOL bSynchronized;
			_labels->get_Synchronized(&bSynchronized);
			if (bSynchronized)
			{
				_labels->Clear();
			}
			*retval = VARIANT_TRUE;
		}
	}
	return S_OK;
}
#pragma endregion

#pragma region CacheExtents
// ****************************************************************
//		get_CacheExtents()
// ****************************************************************
STDMETHODIMP CShapefile::get_CacheExtents(VARIANT_BOOL * pVal)
{
	// The property no longer used
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = VARIANT_FALSE;
	return S_OK;
}

// ****************************************************************
//		put_CacheExtents()
// ****************************************************************
STDMETHODIMP CShapefile::put_CacheExtents(VARIANT_BOOL newVal)
{
	// The property no longer used
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

// ********************************************************************
//		RefreshExtents()
// ********************************************************************
STDMETHODIMP CShapefile::RefreshExtents(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_isEditingShapes)
	{	
		IExtents * box=NULL;
		double Xmin, Ymin, Zmin, Mmin, Xmax, Ymax, Zmax, Mmax;
		
		_minX = 0.0, _maxX = 0.0;
		_minY = 0.0, _maxY = 0.0;
		_minZ = 0.0, _maxZ = 0.0;
		_minM = 0.0, _maxM = 0.0;
		
		for( int i = 0; i < (int)_shapeData.size(); i++ )
		{	
			CShape* shp = ((CShape*)_shapeData[i]->shape);
			shp->get_ExtentsXYZM(Xmin, Ymin, Xmax, Ymax, Zmin, Zmax, Mmin, Mmax);
			
			if (Xmin == 0.0) 
			{
				shp->get_ExtentsXYZM(Xmin, Ymin, Xmax, Ymax, Zmin, Zmax, Mmin, Mmax);
			}

			// refresh shapefile extents
			if (i==0)
			{
				_minX = Xmin, _maxX = Xmax;
				_minY = Ymin, _maxY = Ymax;
				_minZ = Zmin, _maxZ = Zmax;
				_minM = Mmin, _maxM = Mmax;
			}
			else	
			{	if( Xmin < _minX )	_minX = Xmin; 
				if( Xmax > _maxX )	_maxX = Xmax;
				if( Ymin < _minY )	_minY = Ymin;
				if( Ymax > _maxY )	_maxY = Ymax;
				if( Zmin < _minZ )	_minZ = Zmin;
				if( Zmax > _maxZ )	_maxZ = Zmax;
				if( Mmin < _minM )	_minM = Mmin;
				if( Mmax > _maxM )	_maxM = Mmax;
			}
		}
	}
	*retval = VARIANT_TRUE;
	return S_OK;
}

// ********************************************************************
//		RefreshShapeExtents()
// ********************************************************************
STDMETHODIMP CShapefile::RefreshShapeExtents(LONG ShapeId, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// The method is no longer used
	*retval = VARIANT_TRUE;
	return S_OK;
}
#pragma endregion

#pragma region Utilities
// ********************************************************************
//		ReleaseMemoryShapes()
// ********************************************************************
BOOL CShapefile::ReleaseMemoryShapes()
{
	int size = (int)_shapeData.size();
	for( int i = 0; i < size; i++ )
	{	
		if (_shapeData[i]->shape)
		{
			_shapeData[i]->shape->Release();
			_shapeData[i]->shape = NULL;
		}
	}

	if(_useQTree == VARIANT_TRUE)
	{
		delete _qtree;
		_qtree = NULL;
	}
	return S_OK;
}

// ****************************************************************
//		verifyMemShapes
// ****************************************************************
//Verify Shapefile Integrity
BOOL CShapefile::VerifyMemShapes(ICallback * cBack)
{
	ShpfileType shapetype;
	long numPoints;
	long numParts;
	IPoint * firstPnt = NULL;
	IPoint * lastPnt = NULL;
	VARIANT_BOOL vbretval = VARIANT_FALSE;
	
	if (!_globalCallback && cBack)
	{
		_globalCallback = cBack;
		_globalCallback->AddRef();
	}

	for( int i = 0; i < (int)_shapeData.size(); i++ )
	{						
		IShape* shp = _shapeData[i]->shape;
		if ( !shp ) 
			continue;
		
		shp->get_ShapeType(&shapetype);
		shp->get_NumPoints(&numPoints);
		shp->get_NumParts(&numParts);

		if( shapetype != SHP_NULLSHAPE && shapetype != _shpfiletype )
		{	
			
			ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
			return FALSE;
		}
		else if( shapetype == SHP_POINT || shapetype == SHP_POINTZ || shapetype == SHP_POINTM )
		{	
			if( numPoints == 0 )
			{	
				ShpfileType tmpshptype = SHP_NULLSHAPE;
				shp->put_ShapeType(tmpshptype);
			}
		}
		else if( shapetype == SHP_POLYLINE || shapetype == SHP_POLYLINEZ || shapetype == SHP_POLYLINEM )
		{	
			if( numPoints < 2 )
			{	
				ShpfileType tmpshptype = SHP_NULLSHAPE;
				shp->put_ShapeType(tmpshptype);
			}
			else if( numParts == 0 )
			{	
				long partindex = 0;
				shp->InsertPart(0,&partindex,&vbretval);						
			}
		}
		else if( shapetype == SHP_POLYGON || shapetype == SHP_POLYGONZ || shapetype == SHP_POLYGONM )
		{	
			if( numPoints < 3 )
			{	
				ShpfileType tmpshptype = SHP_NULLSHAPE;
				shp->put_ShapeType(tmpshptype);
			}
			else 
			{	
				if( numParts == 0 )
				{	
					long partindex = 0;
					shp->InsertPart(0,&partindex,&vbretval);
					numParts = 1;
				}
				
				//force the first and last point of a ring to be the same
				long partOffset = 0;
				for( int p = 0; p < numParts; p++ )
				{
					long startRing;
					shp->get_Part(p,&startRing);
					long endRing = 0;
					if( p == numParts - 1 )
						endRing = numPoints;	
					else
						shp->get_Part(p+1,&endRing);

					if( startRing < 0 || startRing >= numPoints + partOffset )
						startRing = 0;
					if( endRing < startRing || endRing >= numPoints + partOffset )
						endRing = numPoints + partOffset;
					
					shp->get_Point(startRing,&firstPnt);
					shp->get_Point(endRing - 1,&lastPnt);
					
					double x1, y1, z1;
					double x2, y2, z2;
					
					if ( firstPnt && lastPnt )
					{
						firstPnt->get_X(&x1);
						firstPnt->get_Y(&y1);
						firstPnt->get_Z(&z1);

						lastPnt->get_X(&x2);
						lastPnt->get_Y(&y2);
						lastPnt->get_Z(&z2);

						// make sure first and last point are the same for each part
						if( x1 != x2 || y1 != y2 || z1 != z2 )
						{	
							VARIANT_BOOL retval;
							shp->InsertPoint(firstPnt, &endRing, &retval);
							for( int t = p+1; t < numParts; t++ )
							{	
								shp->get_Part(t,&startRing);
								shp->put_Part(t,startRing+1);
								partOffset++;
							}
						}
					}
					if ( firstPnt )
					{
						firstPnt->Release();
						firstPnt = NULL;		
					}
					
					if ( lastPnt )
					{
						lastPnt->Release();
						lastPnt = NULL;
					}
				}
			}
		}
		else if( shapetype == SHP_MULTIPOINT || shapetype == SHP_MULTIPOINTZ || shapetype == SHP_MULTIPOINTM )
		{	
			if( numPoints == 0 )
			{	
				ShpfileType tmpshptype = SHP_NULLSHAPE;
				shp->put_ShapeType(tmpshptype);
			}
		}
	}
	return TRUE;
}
#pragma endregion

// ****************************************************************
//		get_InteractiveEditing
// ****************************************************************
STDMETHODIMP CShapefile::get_InteractiveEditing(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _isEditingShapes && _interactiveEditing;
	return S_OK;
}
STDMETHODIMP CShapefile::put_InteractiveEditing(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!_isEditingShapes && newVal)
	{
		// start edit mode; naturally no interactive editing without it
		StartEditingShapes(VARIANT_TRUE, NULL, &_interactiveEditing);
		return S_OK;  // error code in previous code
	}
	_interactiveEditing = newVal;   // don't stop edit mode; only interactive mode was stopped
	return S_OK;
}
