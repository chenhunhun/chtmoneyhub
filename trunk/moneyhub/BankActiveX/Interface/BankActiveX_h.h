

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Tue Aug 02 14:38:10 2011
 */
/* Compiler settings for .\Interface\BankActiveX.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __BankActiveX_h_h__
#define __BankActiveX_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IBankCube_FWD_DEFINED__
#define __IBankCube_FWD_DEFINED__
typedef interface IBankCube IBankCube;
#endif 	/* __IBankCube_FWD_DEFINED__ */


#ifndef ___IBankCubeEvents_FWD_DEFINED__
#define ___IBankCubeEvents_FWD_DEFINED__
typedef interface _IBankCubeEvents _IBankCubeEvents;
#endif 	/* ___IBankCubeEvents_FWD_DEFINED__ */


#ifndef __BankCube_FWD_DEFINED__
#define __BankCube_FWD_DEFINED__

#ifdef __cplusplus
typedef class BankCube BankCube;
#else
typedef struct BankCube BankCube;
#endif /* __cplusplus */

#endif 	/* __BankCube_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IBankCube_INTERFACE_DEFINED__
#define __IBankCube_INTERFACE_DEFINED__

/* interface IBankCube */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IBankCube;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("02B40C37-EEF0-4b9b-A197-0A8F7C5AA6C8")
    IBankCube : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SendData( 
            /* [in] */ BSTR strURL,
            /* [in] */ BSTR strPostInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBankCubeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBankCube * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBankCube * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBankCube * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBankCube * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBankCube * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBankCube * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBankCube * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE *SendData )( 
            IBankCube * This,
            /* [in] */ BSTR strURL,
            /* [in] */ BSTR strPostInfo);
        
        END_INTERFACE
    } IBankCubeVtbl;

    interface IBankCube
    {
        CONST_VTBL struct IBankCubeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBankCube_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBankCube_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBankCube_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBankCube_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBankCube_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBankCube_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBankCube_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IBankCube_SendData(This,strURL,strPostInfo)	\
    ( (This)->lpVtbl -> SendData(This,strURL,strPostInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBankCube_INTERFACE_DEFINED__ */



#ifndef __BankCubeActiveXLib_LIBRARY_DEFINED__
#define __BankCubeActiveXLib_LIBRARY_DEFINED__

/* library BankCubeActiveXLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_BankCubeActiveXLib;

#ifndef ___IBankCubeEvents_DISPINTERFACE_DEFINED__
#define ___IBankCubeEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IBankCubeEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IBankCubeEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("76FFF751-E2FB-4cbc-B7B0-B42C370F97A4")
    _IBankCubeEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IBankCubeEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IBankCubeEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IBankCubeEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IBankCubeEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IBankCubeEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IBankCubeEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IBankCubeEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IBankCubeEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IBankCubeEventsVtbl;

    interface _IBankCubeEvents
    {
        CONST_VTBL struct _IBankCubeEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IBankCubeEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IBankCubeEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IBankCubeEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IBankCubeEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IBankCubeEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IBankCubeEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IBankCubeEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IBankCubeEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_BankCube;

#ifdef __cplusplus

class DECLSPEC_UUID("3A62635B-689F-40d6-81AA-47832F843A81")
BankCube;
#endif
#endif /* __BankCubeActiveXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


