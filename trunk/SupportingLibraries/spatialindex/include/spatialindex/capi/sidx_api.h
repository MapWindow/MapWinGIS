/******************************************************************************
 * Project:  libsidx - A C API wrapper around libspatialindex
 * Purpose:	 C API.
 * Author:   Howard Butler, hobu.inc@gmail.com
 ******************************************************************************
 * Copyright (c) 2009, Howard Butler
 *
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
******************************************************************************/

#ifndef SIDX_API_H_INCLUDED
#define SIDX_API_H_INCLUDED

#define SIDX_C_API 1

#include "sidx_config.h"

IDX_C_START

IndexH Index_Create(IndexPropertyH properties);

IndexH Index_CreateWithStream( IndexPropertyH properties,
										int (*readNext)(int64_t *id, double **pMin, double **pMax, uint32_t *nDimension, const uint8_t **pData, size_t *nDataLength)
									   );

void Index_Destroy(IndexH index);
IndexPropertyH Index_GetProperties(IndexH index);

RTError Index_DeleteData(	IndexH index, 
									int64_t id, 
									double* pdMin, 
									double* pdMax, 
									uint32_t nDimension);
							
RTError Index_InsertData(	IndexH index, 
									int64_t id, 
									double* pdMin, 
									double* pdMax, 
									uint32_t nDimension, 
									const uint8_t* pData, 
									size_t nDataLength);
							
uint32_t Index_IsValid(IndexH index);

RTError Index_Intersects_obj(	IndexH index, 
										double* pdMin, 
										double* pdMax, 
										uint32_t nDimension, 
										IndexItemH** items, 
										uint64_t* nResults);

RTError Index_Intersects_id(	IndexH index, 
										double* pdMin, 
										double* pdMax, 
										uint32_t nDimension, 
										int64_t** items, 
										uint64_t* nResults);
										
RTError Index_Intersects_count(	IndexH index, 
										double* pdMin, 
										double* pdMax, 
										uint32_t nDimension, 
										uint64_t* nResults);
RTError Index_NearestNeighbors_obj(IndexH index, 
											double* pdMin, 
											double* pdMax, 
											uint32_t nDimension, 
											IndexItemH** items, 
											uint64_t* nResults);

RTError Index_NearestNeighbors_id( IndexH index, 
											double* pdMin, 
											double* pdMax, 
											uint32_t nDimension, 
											int64_t** items, 
											uint64_t* nResults);

RTError Index_GetBounds(	IndexH index,
									double** ppdMin,
									double** ppdMax,
									uint32_t* nDimension);


SIDX_C_DLL RTError Index_GetLeaves( IndexH index, 
									uint32_t* nLeafNodes,
									uint32_t** nLeafSizes, 
									int64_t** nLeafIDs, 
									int64_t*** nLeafChildIDs,
									double*** pppdMin, 
									double*** pppdMax, 
									uint32_t* nDimension);

void Index_DestroyObjResults(IndexItemH* results, uint32_t nResults);
void Index_ClearBuffer(IndexH index);
void Index_Free(void* object);

void IndexItem_Destroy(IndexItemH item);
int64_t IndexItem_GetID(IndexItemH item);

RTError IndexItem_GetData(IndexItemH item, uint8_t** data, uint64_t* length);

RTError IndexItem_GetBounds(	IndexItemH item,
										double** ppdMin,
										double** ppdMax,
										uint32_t* nDimension);
									
IndexPropertyH IndexProperty_Create();
void IndexProperty_Destroy(IndexPropertyH hProp);

RTError IndexProperty_SetIndexType(IndexPropertyH iprop, RTIndexType value);
RTIndexType IndexProperty_GetIndexType(IndexPropertyH iprop);

RTError IndexProperty_SetDimension(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetDimension(IndexPropertyH iprop);

RTError IndexProperty_SetIndexVariant(IndexPropertyH iprop, RTIndexVariant value);
RTIndexVariant IndexProperty_GetIndexVariant(IndexPropertyH iprop);

RTError IndexProperty_SetIndexStorage(IndexPropertyH iprop, RTStorageType value);
RTStorageType IndexProperty_GetIndexStorage(IndexPropertyH iprop);

RTError IndexProperty_SetPagesize(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetPagesize(IndexPropertyH iprop);

RTError IndexProperty_SetIndexCapacity(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetIndexCapacity(IndexPropertyH iprop);

RTError IndexProperty_SetLeafCapacity(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetLeafCapacity(IndexPropertyH iprop);

RTError IndexProperty_SetLeafPoolCapacity(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetLeafPoolCapacity(IndexPropertyH iprop);

RTError IndexProperty_SetIndexPoolCapacity(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetIndexPoolCapacity(IndexPropertyH iprop);

RTError IndexProperty_SetRegionPoolCapacity(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetRegionPoolCapacity(IndexPropertyH iprop);

RTError IndexProperty_SetPointPoolCapacity(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetPointPoolCapacity(IndexPropertyH iprop);

RTError IndexProperty_SetBufferingCapacity(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetBufferingCapacity(IndexPropertyH iprop);

RTError IndexProperty_SetEnsureTightMBRs(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetEnsureTightMBRs(IndexPropertyH iprop);

RTError IndexProperty_SetOverwrite(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetOverwrite(IndexPropertyH iprop);

RTError IndexProperty_SetNearMinimumOverlapFactor(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetNearMinimumOverlapFactor(IndexPropertyH iprop);

RTError IndexProperty_SetWriteThrough(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetWriteThrough(IndexPropertyH iprop);

RTError IndexProperty_SetFillFactor(IndexPropertyH iprop, double value);
double IndexProperty_GetFillFactor(IndexPropertyH iprop);

RTError IndexProperty_SetSplitDistributionFactor(IndexPropertyH iprop, double value);
double IndexProperty_GetSplitDistributionFactor(IndexPropertyH iprop);

RTError IndexProperty_SetTPRHorizon(IndexPropertyH iprop, double value);
double IndexProperty_GetTPRHorizon(IndexPropertyH iprop);

RTError IndexProperty_SetReinsertFactor(IndexPropertyH iprop, double value);
double IndexProperty_GetReinsertFactor(IndexPropertyH iprop);

RTError IndexProperty_SetFileName(IndexPropertyH iprop, const char* value);
char* IndexProperty_GetFileName(IndexPropertyH iprop);

RTError IndexProperty_SetFileNameExtensionDat(IndexPropertyH iprop, const char* value);
char* IndexProperty_GetFileNameExtensionDat(IndexPropertyH iprop);

RTError IndexProperty_SetFileNameExtensionIdx(IndexPropertyH iprop, const char* value);
char* IndexProperty_GetFileNameExtensionIdx(IndexPropertyH iprop);

RTError IndexProperty_SetCustomStorageCallbacksSize(IndexPropertyH iprop, uint32_t value);
uint32_t IndexProperty_GetCustomStorageCallbacksSize(IndexPropertyH iprop);

RTError IndexProperty_SetCustomStorageCallbacks(IndexPropertyH iprop, const void* value);
void* IndexProperty_GetCustomStorageCallbacks(IndexPropertyH iprop);

RTError IndexProperty_SetIndexID(IndexPropertyH iprop, int64_t value);
int64_t IndexProperty_GetIndexID(IndexPropertyH iprop);

SIDX_C_DLL void* SIDX_NewBuffer(size_t bytes);
SIDX_C_DLL void  SIDX_DeleteBuffer(void* buffer);

SIDX_C_DLL char* SIDX_Version();

IDX_C_END

#endif