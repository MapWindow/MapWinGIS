#pragma once

#include <set>
#include <iterator>
using namespace std;

namespace Templates
{
	// ********************************************************************
	//					Vector2SafeArray()				               
	// ********************************************************************
	//  Creates safearray with object implementing IDispath interface
	//  Returns true when created safearray has elements, and false otherwise
	template <typename T>
	bool Vector2SafeArray(std::vector<T*>* v, VARIANT* arr)
	{
		SAFEARRAY FAR* psa = NULL;
		SAFEARRAYBOUND rgsabound[1];
		rgsabound[0].lLbound = 0;	

		if( v != NULL && v->size() > 0 )
		{
			rgsabound[0].cElements = v->size();
    		psa = SafeArrayCreate( VT_DISPATCH, 1, rgsabound);
	    			
			if( psa )
			{
				LPDISPATCH *pDispatch = NULL;
				SafeArrayAccessData(psa,(void HUGEP* FAR*)(&pDispatch));
				
				for( int i = 0; i < (int)v->size(); i++)
					pDispatch[i] = (*v)[i];
				
				SafeArrayUnaccessData(psa);
				
				arr->vt = VT_ARRAY|VT_DISPATCH;
				arr->parray = psa;
				return true;
			}
		}

		// no elements or a problem with creation
		rgsabound[0].cElements = 0;
		psa = SafeArrayCreate( VT_DISPATCH, 1, rgsabound);

		arr->vt = VT_ARRAY|VT_DISPATCH;
		arr->parray = psa;
		return false;
	}

	/***********************************************************************/
	/*			Vector2SafeArray()
	/***********************************************************************/
	template <typename T>
	bool Vector2SafeArray(std::vector<T>* v, VARTYPE variantType, VARIANT* arr)
	{
		SAFEARRAY FAR* psa = NULL;
		SAFEARRAYBOUND rgsabound[1];
		rgsabound[0].lLbound = 0;	

		if (v->size() > 0)
		{
			rgsabound[0].cElements = v->size();
			psa = SafeArrayCreate( variantType, 1, rgsabound);
			
			if( psa )
			{
				T/*long HUGEP*/ *plng;
				SafeArrayAccessData(psa,(void HUGEP* FAR*)&plng);
				
				// can we copy bytes from set object directly?
				memcpy(plng,&(v->at(0)),sizeof(T)*v->size());
				SafeArrayUnaccessData(psa);
				
				arr->vt = VT_ARRAY|variantType;
				arr->parray = psa;
				return true;
			}
			
			arr->vt = VT_ARRAY|variantType;
			arr->parray = psa;
			return false;
		}
		else
		{
			// no elements or a problem with creation
			rgsabound[0].cElements = 0;
			psa = SafeArrayCreate( variantType, 1, rgsabound);

			arr->vt = VT_ARRAY|variantType;
			arr->parray = psa;
			return false;
		}
	}

	/***********************************************************************/
	/*		Templates::get_SimpleShapeType()
	/***********************************************************************/
	/*  Creates safearray with numbers of shapes as long values
	 *  @return true when created safearray has elements, and false otherwise
	 */
	template <typename T>
	bool Set2SafeArray(std::set<T>* V, VARTYPE varType, VARIANT* arr)
	{
		std::vector<T> vResult;
		copy(V->begin(), V->end(), std::inserter(vResult, vResult.end()));
		return Templates::Vector2SafeArray(&vResult, varType, arr);		//VT_I4
	}	
}