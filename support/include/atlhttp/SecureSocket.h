// This is a part of the Active Template Library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __SECURESOCKET_H__
#define __SECURESOCKET_H__

#pragma once
#ifndef _WINSOCKAPI_
#include <winsock2.h>
#endif

#ifndef	_WINSOCK2API_
#error Winsock2.h has to be included before including windows.h or use atlbase.h instead of windows.h
#endif

#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "ws2_32.lib")
#endif  // !_ATL_NO_DEFAULT_LIBS

#include <svcguid.h>
#include <atlcoll.h>


#include <wincrypt.h>
#include <wintrust.h>
#include <schannel.h>

#define SECURITY_WIN32
#include <security.h>
#include <sspi.h>

#define ATL_READ_BUFF_SIZE 2048

// One of these objects can be created globally to turn
// on the security stuff at CRT startup and shut it down
// on CRT term.
class _Sample_SecurityInit
{

public:


	bool					_securityInitialized;
	SecurityFunctionTableA	_SecurityFunc;
	
	_Sample_SecurityInit() throw()
	{
		_securityInitialized	=	false;
	}

	bool Init()
	{
		if (!IsStarted())
		{
			PSecurityFunctionTable          pSecurityFunc;

			pSecurityFunc = InitSecurityInterface();

			if(pSecurityFunc != NULL)
			{
                memcpy_s(&_SecurityFunc, sizeof(_SecurityFunc), pSecurityFunc, sizeof(_SecurityFunc));
				_securityInitialized	=	true;
			}
			else
				_securityInitialized	=	false;
			
		}
		return _securityInitialized;
	}

	bool IsStarted(){ return _securityInitialized; }


	void	traceVerifyTrustError(DWORD Status)
	{
		LPSTR pszName = NULL;

		switch(Status)
		{
			case CERT_E_EXPIRED:                pszName = "CERT_E_EXPIRED";                 break;
			case CERT_E_VALIDITYPERIODNESTING:  pszName = "CERT_E_VALIDITYPERIODNESTING";   break;
			case CERT_E_ROLE:                   pszName = "CERT_E_ROLE";                    break;
			case CERT_E_PATHLENCONST:           pszName = "CERT_E_PATHLENCONST";            break;
			case CERT_E_CRITICAL:               pszName = "CERT_E_CRITICAL";                break;
			case CERT_E_PURPOSE:                pszName = "CERT_E_PURPOSE";                 break;
			case CERT_E_ISSUERCHAINING:         pszName = "CERT_E_ISSUERCHAINING";          break;
			case CERT_E_MALFORMED:              pszName = "CERT_E_MALFORMED";               break;
			case CERT_E_UNTRUSTEDROOT:          pszName = "CERT_E_UNTRUSTEDROOT";           break;
			case CERT_E_CHAINING:               pszName = "CERT_E_CHAINING";                break;
			case TRUST_E_FAIL:                  pszName = "TRUST_E_FAIL";                   break;
			case CERT_E_REVOKED:                pszName = "CERT_E_REVOKED";                 break;
			case CERT_E_UNTRUSTEDTESTROOT:      pszName = "CERT_E_UNTRUSTEDTESTROOT";       break;
			case CERT_E_REVOCATION_FAILURE:     pszName = "CERT_E_REVOCATION_FAILURE";      break;
			case CERT_E_CN_NO_MATCH:            pszName = "CERT_E_CN_NO_MATCH";             break;
			case CERT_E_WRONG_USAGE:            pszName = "CERT_E_WRONG_USAGE";             break;
			default:                            pszName = "(unknown)";                      break;
		}

		ATLTRACE("Error 0x%x (%s) returned by CertVerifyCertificateChainPolicy!\n", Status, pszName);
	}


	~_Sample_SecurityInit() throw()
	{
		_securityInitialized	=	false;
	}

};

#ifndef _SAMPLE_ATL_NO_GLOBAL_SSL_SUPPORT_STARTUP
	__declspec(selectany)_Sample_SecurityInit	g_Sample_SecurityInit;
#endif



class CSecureEvtSyncSocket
{
public:
	CSecureEvtSyncSocket() throw();
	~CSecureEvtSyncSocket() throw();
	operator SOCKET() throw();
	void Close() throw();
	void Term() throw();
	bool Create(const addrinfo* pAI, WORD wFlags=0) throw();
	bool Create(short af, short st, short proto, WORD wFlags=0) throw();
	bool Connect(LPCTSTR szAddr, unsigned short nPort) throw();
	bool Connect(const addrinfo *pAI)throw();
	bool Connect(const SOCKADDR* psa,int len) throw();
	bool Write(const unsigned char *pBuffIn, DWORD *pdwSize) throw();
	bool Write(WSABUF *pBuffers, int nCount, DWORD *pdwSize) throw();
	bool Read(const unsigned char *pBuff, DWORD *pdwSize) throw();
	bool Init(SOCKET hSocket, void * /*pData=NULL*/) throw();
	DWORD GetSocketTimeout();
	DWORD SetSocketTimeout(DWORD dwNewTimeout);

	bool	SupportsScheme(ATL_URL_SCHEME	scheme)
	{ 
		return ( (scheme == ATL_URL_SCHEME_HTTP) |
				 (scheme == ATL_URL_SCHEME_HTTPS) );
	}



protected:
	DWORD m_dwCreateFlags;
	WSAEVENT m_hEventRead;
	WSAEVENT m_hEventWrite;
	WSAEVENT m_hEventConnect;

	CComAutoCriticalSection m_csRead;
	CComAutoCriticalSection m_csWrite;
	SOCKET m_socket;
	bool m_bConnected;
	DWORD m_dwLastError;
	DWORD m_dwSocketTimeout;

protected:
	// overrides for HTTPS
	bool internalWrite(WSABUF *pBuffers, int nCount, DWORD *pdwSize) throw();
	bool internalRead(const unsigned char *pBuff, DWORD *pdwSize) throw();


public:
	// HTTPS data
	LPCTSTR			m_pszUserName;
	DWORD			m_dwProtocol;
	ALG_ID			m_aiKeyExch;
	bool			m_bHttpsCommunication;

protected:
	HCERTSTORE      m_hMyCertStore;
	SCHANNEL_CRED   m_SchannelCred;
	CredHandle		m_hClientCreds;
	CtxtHandle		m_hContext;
	CStringA		m_strServerName;

	bool			createCredentials();
	void			getNewClientCredentials();
	bool			freeCredentials();
	bool			performClientHandshake(	SOCKET          Socket,         // in
											LPCSTR          pszServerName,  // in
											SecBuffer *     pExtraData);     // out

	bool			clientHandshakeLoop(    SOCKET          Socket,         // in
											BOOL            fDoInitialRead, // in
                                            SecBuffer *     pExtraData);
	bool			disconnectFromSecureServer();

	bool			verifyServerCertificate(
									LPCSTR          pszServerName,
									DWORD           dwCertFlags);


};

#include "secureSocket.inl"

#endif // __SECURESOCKET_H__
