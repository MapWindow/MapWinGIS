/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Jul 17 13:12:29 2001
 */
/* Compiler settings for C:\components\Callback\Callback.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __Callback_h__
#define __Callback_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ItkCallback_FWD_DEFINED__
#define __ItkCallback_FWD_DEFINED__
typedef interface ItkCallback ItkCallback;
#endif 	/* __ItkCallback_FWD_DEFINED__ */


#ifndef __tkCallback_FWD_DEFINED__
#define __tkCallback_FWD_DEFINED__

#ifdef __cplusplus
typedef class tkCallback tkCallback;
#else
typedef struct tkCallback tkCallback;
#endif /* __cplusplus */

#endif 	/* __tkCallback_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ItkCallback_INTERFACE_DEFINED__
#define __ItkCallback_INTERFACE_DEFINED__

/* interface ItkCallback */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ItkCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("26899FE5-6834-4B7A-8C28-BBA5F45BE462")
    ItkCallback : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Progress( 
            /* [in] */ BSTR Key,
            /* [in] */ long Percent,
            /* [in] */ BSTR Message) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Error( 
            /* [in] */ BSTR Key,
            /* [in] */ long ErrorCode,
            /* [in] */ BSTR ErrorMsg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ItkCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ItkCallback __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ItkCallback __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ItkCallback __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ItkCallback __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ItkCallback __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ItkCallback __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ItkCallback __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Progress )( 
            ItkCallback __RPC_FAR * This,
            /* [in] */ BSTR Key,
            /* [in] */ long Percent,
            /* [in] */ BSTR Message);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Error )( 
            ItkCallback __RPC_FAR * This,
            /* [in] */ BSTR Key,
            /* [in] */ long ErrorCode,
            /* [in] */ BSTR ErrorMsg);
        
        END_INTERFACE
    } ItkCallbackVtbl;

    interface ItkCallback
    {
        CONST_VTBL struct ItkCallbackVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ItkCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ItkCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ItkCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ItkCallback_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ItkCallback_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ItkCallback_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ItkCallback_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ItkCallback_Progress(This,Key,Percent,Message)	\
    (This)->lpVtbl -> Progress(This,Key,Percent,Message)

#define ItkCallback_Error(This,Key,ErrorCode,ErrorMsg)	\
    (This)->lpVtbl -> Error(This,Key,ErrorCode,ErrorMsg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ItkCallback_Progress_Proxy( 
    ItkCallback __RPC_FAR * This,
    /* [in] */ BSTR Key,
    /* [in] */ long Percent,
    /* [in] */ BSTR Message);


void __RPC_STUB ItkCallback_Progress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ItkCallback_Error_Proxy( 
    ItkCallback __RPC_FAR * This,
    /* [in] */ BSTR Key,
    /* [in] */ long ErrorCode,
    /* [in] */ BSTR ErrorMsg);


void __RPC_STUB ItkCallback_Error_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ItkCallback_INTERFACE_DEFINED__ */



#ifndef __CALLBACKLib_LIBRARY_DEFINED__
#define __CALLBACKLib_LIBRARY_DEFINED__

/* library CALLBACKLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_CALLBACKLib;

EXTERN_C const CLSID CLSID_tkCallback;

#ifdef __cplusplus

class DECLSPEC_UUID("2B97634D-A3F9-4351-9008-8151CBE188D6")
tkCallback;
#endif
#endif /* __CALLBACKLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

