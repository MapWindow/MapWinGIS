#ifndef __SECURESOCKET_INL__
#define __SECURESOCKET_INL__

#define HTTPS_IO_BUFFER_SIZE  0x10000


#pragma once
#pragma comment(lib, "Crypt32")
inline CSecureEvtSyncSocket::CSecureEvtSyncSocket() 
{
	m_dwCreateFlags = WSA_FLAG_OVERLAPPED;
	m_hEventRead = m_hEventWrite = m_hEventConnect = NULL;
	m_socket = INVALID_SOCKET;
	m_bConnected = false;
	m_dwLastError = 0;
	m_dwSocketTimeout = ATL_SOCK_TIMEOUT;
	g_HttpInit.Init();
	g_Sample_SecurityInit.Init();

	// HTTPS initialization
	m_pszUserName		=	NULL;
	m_dwProtocol		=	SP_PROT_NONE;
	m_aiKeyExch			=	0;
	m_hMyCertStore		=	NULL;

	m_bHttpsCommunication	=	false;

}

inline CSecureEvtSyncSocket::~CSecureEvtSyncSocket() 
{
	Close();
}

inline CSecureEvtSyncSocket::operator SOCKET() 
{
	return m_socket;
}

inline void CSecureEvtSyncSocket::Close()
{
	if (m_socket != INVALID_SOCKET)
	{
		if( m_bHttpsCommunication )
			disconnectFromSecureServer();
		m_bConnected = false;
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		Term();
	}
	if( m_bHttpsCommunication )
		freeCredentials();
}

inline void CSecureEvtSyncSocket::Term() 
{
	if (m_hEventRead)
	{
		WSACloseEvent(m_hEventRead);
		m_hEventRead = NULL;
	}
	if (m_hEventWrite)
	{
		WSACloseEvent(m_hEventWrite);
		m_hEventWrite = NULL;
	}
	if (m_hEventConnect)
	{
		WSACloseEvent(m_hEventConnect);
		m_hEventConnect = NULL;
	}
	m_socket = INVALID_SOCKET;
}

inline bool CSecureEvtSyncSocket::Create(const addrinfo* pAI,WORD wFlags)
{
	//return Create(PF_INET, SOCK_STREAM, IPPROTO_TCP, wFlags);
	return Create(pAI->ai_family, pAI->ai_socktype, pAI->ai_protocol, wFlags);
}

inline bool CSecureEvtSyncSocket::Create(short af, short st, short proto, WORD wFlags) 
{
	bool bRet = true;
	if (m_socket != INVALID_SOCKET)
	{
		m_dwLastError = WSAEALREADY;
		return false; // Must close this socket first
	}

	m_socket = WSASocket(af, st, proto, NULL, 0,
		wFlags | m_dwCreateFlags);
	if (m_socket == INVALID_SOCKET)
	{
		m_dwLastError = ::WSAGetLastError();
		bRet = false;
	}
	else
		bRet = Init(m_socket, NULL);
	return bRet;
}

inline bool CSecureEvtSyncSocket::Connect(LPCTSTR szAddr, unsigned short nPort) 
{
	if (m_bConnected)
		return true;

	
	m_bHttpsCommunication	=	(nPort	==	ATL_URL_DEFAULT_HTTPS_PORT);
	bool bRet = true;

	m_strServerName	=	szAddr;
	CSocketAddr address;
	if (SOCKET_ERROR == address.FindAddr(szAddr,nPort,AI_CANONNAME,PF_UNSPEC,SOCK_STREAM,IPPROTO_IP))
	{
		m_dwLastError = WSAGetLastError();
		bRet = false;
	}
	else
	{
		bRet = Connect(address.GetAddrInfo());
	}
	
	return bRet;
}

// HTTPS-IPV6 modified
inline bool CSecureEvtSyncSocket::Connect(const addrinfo *pAI)
{
	if (m_socket == INVALID_SOCKET && !Create(pAI))
		return false;

	return Connect((SOCKADDR*)pAI->ai_addr, (int)pAI->ai_addrlen);
}

// HTTPS modified
inline bool CSecureEvtSyncSocket::Connect(const SOCKADDR* psa,int len) 
{
	if (m_bConnected)
		return true; // already connected

	DWORD dwLastError;
	bool bRet = true;

	if( m_bHttpsCommunication )
		bRet	=	createCredentials();

	if( !bRet )
		return false;

	// if you try to connect the socket without
	// creating it first it's unreasonable to automatically
	// try the create for you.
	if (m_socket == INVALID_SOCKET)
	{
		if( m_bHttpsCommunication )
			freeCredentials();
		return false;
	}

	if (WSAConnect(m_socket, 
		psa, len,
		NULL, NULL, NULL, NULL))
	{
		dwLastError = WSAGetLastError();
		if (dwLastError != WSAEWOULDBLOCK)
		{
			m_dwLastError = dwLastError;
			bRet = false;
		}
		else
		{
			dwLastError = WaitForSingleObject((HANDLE)m_hEventConnect, 10000);
			if (dwLastError == WAIT_OBJECT_0)
			{
				// make sure there were no connection errors.
				WSANETWORKEVENTS wse;
				ZeroMemory(&wse, sizeof(wse));
				WSAEnumNetworkEvents(m_socket, NULL, &wse);
				if (wse.iErrorCode[FD_CONNECT_BIT]!=0)
				{
					m_dwLastError = (DWORD)(wse.iErrorCode[FD_CONNECT_BIT]);
					if( m_bHttpsCommunication )
						freeCredentials();
					return false;
				}
			}
		}

	}

	m_bConnected = bRet;
	if( !m_bConnected && m_bHttpsCommunication )
		freeCredentials();
	else if( m_bHttpsCommunication )
	{
		SecBuffer			ExtraData;
		bool				bOK	=	true;
		bOK	=	performClientHandshake( m_socket, m_strServerName, &ExtraData);
		
		if( bOK )
			bOK	=	verifyServerCertificate(m_strServerName, 0);
			

		if( !bOK )
		{
			Close();
			m_bConnected	=	false;
			bRet			=	false;
		}

	}
	return bRet;
}

inline bool CSecureEvtSyncSocket::internalWrite(WSABUF *pBuffers, int nCount, DWORD *pdwSize) 
{
	// if we aren't already connected we'll wait to see if the connect
	// event happens
	if (WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hEventConnect , m_dwSocketTimeout))
	{
		m_dwLastError = WSAENOTCONN;
		return false; // not connected
	}

	// make sure we aren't already writing
	if (WAIT_TIMEOUT == WaitForSingleObject((HANDLE)m_hEventWrite, 0))
	{
		m_dwLastError = WSAEINPROGRESS;
		return false; // another write on is blocking this socket
	}

	bool bRet = true;
	*pdwSize = 0;
	WSAOVERLAPPED o;
	m_csWrite.Lock();
	o.hEvent = m_hEventWrite;
	WSAResetEvent(o.hEvent);
	if (WSASend(m_socket, pBuffers, nCount, pdwSize, 0, &o, 0))
	{	
		DWORD dwLastError = WSAGetLastError();
		if (dwLastError != WSA_IO_PENDING)
		{
			m_dwLastError = dwLastError;
			bRet = false;
		}
	}
	
	// wait for write to complete
	if (bRet && WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)m_hEventWrite, m_dwSocketTimeout))
	{
		DWORD dwFlags = 0;
		if (WSAGetOverlappedResult(m_socket, &o, pdwSize, FALSE, &dwFlags))
			bRet = true;
		else
		{
			m_dwLastError = ::GetLastError();
			bRet = false;
		}
	}
	
	m_csWrite.Unlock();
	return bRet;

}

inline bool CSecureEvtSyncSocket::Write(const unsigned char *pBuffIn, DWORD *pdwSize) 
{
	WSABUF buff;
	buff.buf = (char*)pBuffIn;
	buff.len = *pdwSize;
	return Write(&buff, 1, pdwSize);
}

inline bool CSecureEvtSyncSocket::internalRead(const unsigned char *pBuff, DWORD *pdwSize) 
{
	// if we aren't already connected we'll wait to see if the connect
	// event happens
	if (WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hEventConnect , m_dwSocketTimeout))
	{
		m_dwLastError = WSAENOTCONN;
		return false; // not connected
	}

	if (WAIT_ABANDONED == WaitForSingleObject((HANDLE)m_hEventRead, 0))
	{
		m_dwLastError = WSAEINPROGRESS;
		return false; // another write on is blocking this socket
	}

	bool bRet = true;
	WSABUF buff;
	buff.buf = (char*)pBuff;
	buff.len = *pdwSize;
	*pdwSize = 0;
	DWORD dwFlags = 0;
	WSAOVERLAPPED o;
	ZeroMemory(&o, sizeof(o));

	// protect against re-entrency
	m_csRead.Lock();
	o.hEvent = m_hEventRead;
	WSAResetEvent(o.hEvent);
	if (WSARecv(m_socket, &buff, 1, pdwSize, &dwFlags, &o, 0))
	{
		DWORD dwLastError = WSAGetLastError();
		if (dwLastError != WSA_IO_PENDING)
		{
			m_dwLastError = dwLastError;
			bRet = false;
		}
	}

	// wait for the read to complete
	if (bRet && WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)o.hEvent, m_dwSocketTimeout))
	{
		dwFlags = 0;
		if (WSAGetOverlappedResult(m_socket, &o, pdwSize, FALSE, &dwFlags))
			bRet = true;
		else
		{
			m_dwLastError = ::GetLastError();
			bRet = false;
		}

	}

	m_csRead.Unlock();
	return bRet;
}

inline bool CSecureEvtSyncSocket::Init(SOCKET hSocket, void * /*pData=NULL*/) 
{
	ATLASSERT(hSocket != INVALID_SOCKET);

	if (hSocket == INVALID_SOCKET)
	{
		m_dwLastError = WSAENOTSOCK;
		return false;
	}

	m_socket = hSocket;
	
	// Allocate Events. On error, any open event handles will be closed
	// in the destructor
	if (NULL != (m_hEventRead = WSACreateEvent()))
		if (NULL != (m_hEventWrite = WSACreateEvent()))
			if (NULL != (m_hEventConnect = WSACreateEvent()))
	{
		if (!WSASetEvent(m_hEventWrite) || !WSASetEvent(m_hEventRead))
		{
			m_dwLastError = ::GetLastError();
			return false;
		}

		if (SOCKET_ERROR != WSAEventSelect(m_socket, m_hEventRead, FD_READ))
			if (SOCKET_ERROR != WSAEventSelect(m_socket, m_hEventWrite, FD_WRITE))
				if (SOCKET_ERROR != WSAEventSelect(m_socket, m_hEventConnect, FD_CONNECT))
					return true;
	}
	m_dwLastError = ::GetLastError();
	return false;
}

inline DWORD CSecureEvtSyncSocket::GetSocketTimeout()
{
	return m_dwSocketTimeout;
}

inline DWORD CSecureEvtSyncSocket::SetSocketTimeout(DWORD dwNewTimeout)
{
	DWORD dwOldTimeout = m_dwSocketTimeout;
	m_dwSocketTimeout = dwNewTimeout;
	return dwOldTimeout;
}


// HTTPS stuff
inline bool CSecureEvtSyncSocket::createCredentials()
{
    TimeStamp       tsExpiry;
    SECURITY_STATUS Status;

    DWORD           cSupportedAlgs = 0;
    ALG_ID          rgbSupportedAlgs[16];

    PCCERT_CONTEXT  pCertContext = NULL;

    // Open the "MY" certificate store, which is where Internet Explorer
    // stores its client certificates.
    if(m_hMyCertStore == NULL)
    {
        m_hMyCertStore = CertOpenSystemStore(0, _T("MY"));

        if(!m_hMyCertStore)
        {
			ATLTRACE("CSecureEvtSyncSocket::createCredentials 1 - SEC_E_NO_CREDENTIALS");
			return false;
        }
    }

    //
    // If a user name is specified, then attempt to find a client
    // certificate. Otherwise, just create a NULL credential.
    //

    if(m_pszUserName)
    {
        // Find client certificate. Note that in this sample we search for a 
        // certificate with a subject that matches the user name.

        pCertContext = CertFindCertificateInStore(m_hMyCertStore, 
                                                  X509_ASN_ENCODING, 
                                                  0,
                                                  CERT_FIND_SUBJECT_STR_A,
                                                  m_pszUserName,
                                                  NULL);
        if(pCertContext == NULL)
        {
            ATLTRACE("CSecureEvtSyncSocket::createCredentials 2 - SEC_E_NO_CREDENTIALS");
            return false;
        }
    }


    //
    // Build Schannel credential structure. 
    
    ZeroMemory(&m_SchannelCred, sizeof(m_SchannelCred));

    m_SchannelCred.dwVersion  = SCHANNEL_CRED_VERSION;
    if(pCertContext)
    {
        m_SchannelCred.cCreds     = 1;
        m_SchannelCred.paCred     = &pCertContext;
    }

    m_SchannelCred.grbitEnabledProtocols = m_dwProtocol;

    if(m_aiKeyExch)
    {
        rgbSupportedAlgs[cSupportedAlgs++] = m_aiKeyExch;
    }

    if(cSupportedAlgs)
    {
        m_SchannelCred.cSupportedAlgs    = cSupportedAlgs;
        m_SchannelCred.palgSupportedAlgs = rgbSupportedAlgs;
    }

    m_SchannelCred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;
    m_SchannelCred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;


    //
    // Create an SSPI credential.
    //

    Status = g_Sample_SecurityInit._SecurityFunc.AcquireCredentialsHandleA(
										NULL,                   // Name of principal    
										UNISP_NAME_A,           // Name of package
										SECPKG_CRED_OUTBOUND,   // Flags indicating use
										NULL,                   // Pointer to logon ID
										&m_SchannelCred,         // Package specific data
										NULL,                   // Pointer to GetKey() func
										NULL,                   // Value to pass to GetKey()
										&m_hClientCreds,         // (out) Cred Handle
										&tsExpiry);             // (out) Lifetime (optional)
    if(Status != SEC_E_OK)
    {
        ATLTRACE("**** Error 0x%x returned by AcquireCredentialsHandle\n");
        return false;
    }


    //
    // Free the certificate context. Schannel has already made its own copy.
    //

    if(pCertContext)
    {
        CertFreeCertificateContext(pCertContext);
    }


    return true;
}


inline bool CSecureEvtSyncSocket::freeCredentials()
{
    // Free security context.
    g_Sample_SecurityInit._SecurityFunc.DeleteSecurityContext(&m_hContext);

    // Free SSPI credentials handle.
    g_Sample_SecurityInit._SecurityFunc.FreeCredentialsHandle(&m_hClientCreds);


    // Close "MY" certificate store.
    if(m_hMyCertStore)
    {
        CertCloseStore(m_hMyCertStore, 0);
    }

	return true;
}



inline bool CSecureEvtSyncSocket::performClientHandshake(
							SOCKET          Socket,         // in
							LPCSTR          pszServerName,  // in
							SecBuffer *     pExtraData)     // out
{
    SecBufferDesc   OutBuffer;
    SecBuffer       OutBuffers[1];
    DWORD           dwSSPIFlags;
    DWORD           dwSSPIOutFlags;
    TimeStamp       tsExpiry;
    SECURITY_STATUS scRet;
    DWORD           cbData;

    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT   |
                  ISC_REQ_REPLAY_DETECT     |
                  ISC_REQ_CONFIDENTIALITY   |
                  ISC_RET_EXTENDED_ERROR    |
                  ISC_REQ_ALLOCATE_MEMORY   |
                  ISC_REQ_STREAM;

    //
    //  Initiate a ClientHello message and generate a token.
    //

    OutBuffers[0].pvBuffer   = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer   = 0;

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    scRet = g_Sample_SecurityInit._SecurityFunc.InitializeSecurityContextA(
                    &m_hClientCreds,
                    NULL,
                    (SEC_CHAR*)pszServerName,
                    dwSSPIFlags,
                    0,
                    SECURITY_NATIVE_DREP,
                    NULL,
                    0,
                    &m_hContext,
                    &OutBuffer,
                    &dwSSPIOutFlags,
                    &tsExpiry);

    if(scRet != SEC_I_CONTINUE_NEEDED)
    {
        CString	szError;
		szError.Format(_T("**** Error %d returned by InitializeSecurityContext (1)\n"), scRet);
		ATLTRACE( (LPCTSTR)szError );
        return false;
    }

    // Send response to server if there is one.
    if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
    {

		WSABUF	wsaBuff;
		wsaBuff.len	=  OutBuffers[0].cbBuffer;
		wsaBuff.buf = (char*)OutBuffers[0].pvBuffer;
		bool	bTmp = internalWrite( &wsaBuff, 1, &cbData);

		/*cbData = send(Socket,
                      (const char*)OutBuffers[0].pvBuffer,
                      OutBuffers[0].cbBuffer,
                      0);*/
        if(cbData == SOCKET_ERROR || cbData == 0)
        {
            CString		szError;
			szError.Format(_T("**** Error %d sending data to server (1)\n"), WSAGetLastError());
			ATLTRACE((LPCTSTR)szError);

            g_Sample_SecurityInit._SecurityFunc.FreeContextBuffer(OutBuffers[0].pvBuffer);
            g_Sample_SecurityInit._SecurityFunc.DeleteSecurityContext(&m_hContext);
            return false;
        }

        // Free output buffer.
        g_Sample_SecurityInit._SecurityFunc.FreeContextBuffer(OutBuffers[0].pvBuffer);
        OutBuffers[0].pvBuffer = NULL;
    }


    return clientHandshakeLoop(Socket, TRUE, pExtraData);
}

/*****************************************************************************/
inline bool CSecureEvtSyncSocket::clientHandshakeLoop(
    SOCKET          Socket,         // in
/*    PCredHandle     phCreds,        // in
    CtxtHandle *    phContext,      // in, out*/
    BOOL            fDoInitialRead, // in
    SecBuffer *     pExtraData)     // out
{
    SecBufferDesc   InBuffer;
    SecBuffer       InBuffers[2];
    SecBufferDesc   OutBuffer;
    SecBuffer       OutBuffers[1];
    DWORD           dwSSPIFlags;
    DWORD           dwSSPIOutFlags;
    TimeStamp       tsExpiry;
    SECURITY_STATUS scRet;
    DWORD           cbData;

    PUCHAR          IoBuffer;
    DWORD           cbIoBuffer;
    BOOL            fDoRead;
	bool			bRet = true;


    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT   |
                  ISC_REQ_REPLAY_DETECT     |
                  ISC_REQ_CONFIDENTIALITY   |
                  ISC_RET_EXTENDED_ERROR    |
                  ISC_REQ_ALLOCATE_MEMORY   |
                  ISC_REQ_STREAM;

    //
    // Allocate data buffer.
    //

    IoBuffer = (PUCHAR)LocalAlloc(LMEM_FIXED, HTTPS_IO_BUFFER_SIZE);
    if(IoBuffer == NULL)
    {
        ATLTRACE("**** Out of memory (1)\n");
        return false;
    }
    cbIoBuffer = 0;
    fDoRead = fDoInitialRead;


    // 
    // Loop until the handshake is finished or an error occurs.
    //

    scRet = SEC_I_CONTINUE_NEEDED;

    while(scRet == SEC_I_CONTINUE_NEEDED        ||
          scRet == SEC_E_INCOMPLETE_MESSAGE     ||
          scRet == SEC_I_INCOMPLETE_CREDENTIALS) 
	{

        //
        // Read data from server.
        //

        if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            if(fDoRead)
            {
				cbData	=	HTTPS_IO_BUFFER_SIZE - cbIoBuffer;
				bool	bTmp = internalRead( (const unsigned char*)IoBuffer + cbIoBuffer, &cbData);
				/*cbData = recv(Socket, 
                              (char*)IoBuffer + cbIoBuffer, 
                              HTTPS_IO_BUFFER_SIZE - cbIoBuffer, 
                              0);*/
                if(cbData == SOCKET_ERROR)
                {
                    CString		szError;
					szError.Format(_T("**** Error %d reading data from server\n"), WSAGetLastError());
					ATLTRACE((LPCTSTR)szError);
                    scRet = SEC_E_INTERNAL_ERROR;
					bRet	=	false;
                    break;
                }
                else if(cbData == 0)
                {
                    ATLTRACE("**** Server unexpectedly disconnected\n");
                    scRet = SEC_E_INTERNAL_ERROR;
					bRet	=	false;
                    break;
                }

                cbIoBuffer += cbData;
            }
            else
            {
                fDoRead = TRUE;
            }
        }


		if( !bRet )
			return bRet;
        //
        // Set up the input buffers. Buffer 0 is used to pass in data
        // received from the server. Schannel will consume some or all
        // of this. Leftover data (if any) will be placed in buffer 1 and
        // given a buffer type of SECBUFFER_EXTRA.
        //

        InBuffers[0].pvBuffer   = IoBuffer;
        InBuffers[0].cbBuffer   = cbIoBuffer;
        InBuffers[0].BufferType = SECBUFFER_TOKEN;

        InBuffers[1].pvBuffer   = NULL;
        InBuffers[1].cbBuffer   = 0;
        InBuffers[1].BufferType = SECBUFFER_EMPTY;

        InBuffer.cBuffers       = 2;
        InBuffer.pBuffers       = InBuffers;
        InBuffer.ulVersion      = SECBUFFER_VERSION;

        //
        // Set up the output buffers. These are initialized to NULL
        // so as to make it less likely we'll attempt to free random
        // garbage later.
        //

        OutBuffers[0].pvBuffer  = NULL;
        OutBuffers[0].BufferType= SECBUFFER_TOKEN;
        OutBuffers[0].cbBuffer  = 0;

        OutBuffer.cBuffers      = 1;
        OutBuffer.pBuffers      = OutBuffers;
        OutBuffer.ulVersion     = SECBUFFER_VERSION;

        //
        // Call InitializeSecurityContext.
        //

        scRet = g_Sample_SecurityInit._SecurityFunc.InitializeSecurityContextA(&m_hClientCreds,
                                          &m_hContext,
                                          NULL,
                                          dwSSPIFlags,
                                          0,
                                          SECURITY_NATIVE_DREP,
                                          &InBuffer,
                                          0,
                                          NULL,
                                          &OutBuffer,
                                          &dwSSPIOutFlags,
                                          &tsExpiry);

        //
        // If InitializeSecurityContext was successful (or if the error was 
        // one of the special extended ones), send the contends of the output
        // buffer to the server.
        //

        if(scRet == SEC_E_OK                ||
           scRet == SEC_I_CONTINUE_NEEDED   ||
           FAILED(scRet) && (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR))
        {
            if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
            {
				WSABUF	wsaBuff;
				wsaBuff.len	=  OutBuffers[0].cbBuffer;
				wsaBuff.buf = (char*)OutBuffers[0].pvBuffer;
				bool	bTmp = internalWrite( &wsaBuff, 1, &cbData);
                
				/*cbData = send(Socket,
                              (char*)OutBuffers[0].pvBuffer,
                              OutBuffers[0].cbBuffer,
                              0);*/
                if(cbData == SOCKET_ERROR || cbData == 0)
                {
                    CString		szError;
					szError.Format(_T("**** Error %d sending data to server (2)\n"), 
                        WSAGetLastError());
					ATLTRACE((LPCTSTR)szError);
                    g_Sample_SecurityInit._SecurityFunc.FreeContextBuffer(OutBuffers[0].pvBuffer);
                    g_Sample_SecurityInit._SecurityFunc.DeleteSecurityContext(&m_hContext);
                    // return SEC_E_INTERNAL_ERROR;
					return false;
                }

                // Free output buffer.
                g_Sample_SecurityInit._SecurityFunc.FreeContextBuffer(OutBuffers[0].pvBuffer);
                OutBuffers[0].pvBuffer = NULL;
            }
        }


        //
        // If InitializeSecurityContext returned SEC_E_INCOMPLETE_MESSAGE,
        // then we need to read more data from the server and try again.
        //

        if(scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            continue;
        }


        //
        // If InitializeSecurityContext returned SEC_E_OK, then the 
        // handshake completed successfully.
        //

        if(scRet == SEC_E_OK)
        {
            //
            // If the "extra" buffer contains data, this is encrypted application
            // protocol layer stuff. It needs to be saved. The application layer
            // will later decrypt it with DecryptMessage.
            //
            if(InBuffers[1].BufferType == SECBUFFER_EXTRA)
            {
                pExtraData->pvBuffer = LocalAlloc(LMEM_FIXED, 
                                                  InBuffers[1].cbBuffer);
                if(pExtraData->pvBuffer == NULL)
                {
                    ATLTRACE("**** Out of memory (2)\n");
                    return false;
					//return SEC_E_INTERNAL_ERROR;
                }

                memmove_s(pExtraData->pvBuffer, InBuffers[1].cbBuffer,
                           IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
                           InBuffers[1].cbBuffer);

                pExtraData->cbBuffer   = InBuffers[1].cbBuffer;
                pExtraData->BufferType = SECBUFFER_TOKEN;
            }
            else
            {
                pExtraData->pvBuffer   = NULL;
                pExtraData->cbBuffer   = 0;
                pExtraData->BufferType = SECBUFFER_EMPTY;
            }

            //
            // Bail out to quit
            //

            break;
        }


        //
        // Check for fatal error.
        //

        if(FAILED(scRet))
        {
            CString szError;
			szError.Format(_T("**** Error 0x%x returned by InitializeSecurityContext (2)\n"), scRet);
			ATLTRACE((LPCTSTR)szError);
			bRet	=	false;
            break;
        }


        //
        // If InitializeSecurityContext returned SEC_I_INCOMPLETE_CREDENTIALS,
        // then the server just requested client authentication. 
        //

        if(scRet == SEC_I_INCOMPLETE_CREDENTIALS)
        {
            //
            // Display trusted issuers info. 
            //

            getNewClientCredentials();


            // As this is currently written, Schannel will send a "no 
            // certificate" alert to the server in place of a certificate. 
            // The server might be cool with this, or it might drop the 
            // connection.
            // 
            // TODO : This would be a good time to prompt the user to select
            // a client certificate and obtain a new credential handle.

            // Go around again.
            fDoRead = FALSE;
            scRet = SEC_I_CONTINUE_NEEDED;
            continue;
        }


        //
        // Copy any leftover data from the "extra" buffer, and go around
        // again.
        //

        if ( InBuffers[1].BufferType == SECBUFFER_EXTRA )
        {
            memmove_s(IoBuffer, HTTPS_IO_BUFFER_SIZE,
                       IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
                       InBuffers[1].cbBuffer);

            cbIoBuffer = InBuffers[1].cbBuffer;
        }
        else
        {
            cbIoBuffer = 0;
        }
    }

    // Delete the security context in the case of a fatal error.
    if(FAILED(scRet))
    {
        bRet	=	false;
		g_Sample_SecurityInit._SecurityFunc.DeleteSecurityContext(&m_hContext);
    }

    LocalFree(IoBuffer);

    return bRet;
}


inline void CSecureEvtSyncSocket::getNewClientCredentials()
{
    CredHandle						hCreds;
    SecPkgContext_IssuerListInfoEx	IssuerListInfo;
    PCCERT_CHAIN_CONTEXT			pChainContext;
    CERT_CHAIN_FIND_BY_ISSUER_PARA	FindByIssuerPara;
    PCCERT_CONTEXT					pCertContext;
    TimeStamp						tsExpiry;
    SECURITY_STATUS					Status;

    //
    // Read list of trusted issuers from schannel.
    //

    Status = g_Sample_SecurityInit._SecurityFunc.QueryContextAttributesA(&m_hContext,
																	SECPKG_ATTR_ISSUER_LIST_EX,
																	(PVOID)&IssuerListInfo);
    if(Status != SEC_E_OK)
    {
        CString	szError;
		szError.Format(_T("Error 0x%x querying issuer list info\n"), Status);
		ATLTRACE((LPCTSTR)szError);
        return;
    }

    //
    // Enumerate the client certificates.
    //

    ZeroMemory(&FindByIssuerPara, sizeof(FindByIssuerPara));

    FindByIssuerPara.cbSize = sizeof(FindByIssuerPara);
    FindByIssuerPara.pszUsageIdentifier = szOID_PKIX_KP_CLIENT_AUTH;
    FindByIssuerPara.dwKeySpec = 0;
    FindByIssuerPara.cIssuer   = IssuerListInfo.cIssuers;
    FindByIssuerPara.rgIssuer  = IssuerListInfo.aIssuers;

    pChainContext = NULL;

    while(TRUE)
    {
        // Find a certificate chain.
        pChainContext = CertFindChainInStore(m_hMyCertStore,
                                             X509_ASN_ENCODING,
                                             0,
                                             CERT_CHAIN_FIND_BY_ISSUER,
                                             &FindByIssuerPara,
                                             pChainContext);
        if(pChainContext == NULL)
        {
            CString	szError;
			szError.Format(_T("Error 0x%x finding cert chain\n"), GetLastError());
			ATLTRACE((LPCTSTR)szError);
            break;
        }

        // Get pointer to leaf certificate context.
        pCertContext = pChainContext->rgpChain[0]->rgpElement[0]->pCertContext;

        // Create schannel credential.
        m_SchannelCred.cCreds = 1;
        m_SchannelCred.paCred = &pCertContext;

        Status = g_Sample_SecurityInit._SecurityFunc.AcquireCredentialsHandleA(
                            NULL,                   // Name of principal
                            UNISP_NAME_A,           // Name of package
                            SECPKG_CRED_OUTBOUND,   // Flags indicating use
                            NULL,                   // Pointer to logon ID
                            &m_SchannelCred,          // Package specific data
                            NULL,                   // Pointer to GetKey() func
                            NULL,                   // Value to pass to GetKey()
                            &hCreds,                // (out) Cred Handle
                            &tsExpiry);             // (out) Lifetime (optional)
        if(Status != SEC_E_OK)
        {
			CString	szError;
			szError.Format(_T("**** Error 0x%x returned by AcquireCredentialsHandle\n"), Status);
			ATLTRACE((LPCTSTR)szError);
            continue;
        }


        // Destroy the old credentials.
        g_Sample_SecurityInit._SecurityFunc.FreeCredentialsHandle(&m_hClientCreds);

        m_hClientCreds = hCreds;
        break;
    }
}


inline bool	CSecureEvtSyncSocket::verifyServerCertificate(
									LPCSTR          pszServerName,
									DWORD           dwCertFlags)
{
    HTTPSPolicyCallbackData  polHttps;
    CERT_CHAIN_POLICY_PARA   PolicyPara;
    CERT_CHAIN_POLICY_STATUS PolicyStatus;
    CERT_CHAIN_PARA          ChainPara;
    PCCERT_CHAIN_CONTEXT     pChainContext = NULL;
	PCCERT_CONTEXT				pServerCert = NULL;

    DWORD   Status;
    PWSTR   pwszServerName;
    DWORD   cchServerName;


    Status = g_Sample_SecurityInit._SecurityFunc.QueryContextAttributesA(&m_hContext,
														SECPKG_ATTR_REMOTE_CERT_CONTEXT,
														(PVOID)&pServerCert);
    if(Status != SEC_E_OK)
    {
        CString szError;
		szError.Format(_T("Error 0x%x querying remote certificate\n"), Status);
		ATLTRACE( szError );


        return false;
    }


    if(pServerCert == NULL)
    {
        //SEC_E_WRONG_PRINCIPAL;
		return false;
    }


    //
    // Convert server name to unicode.
    //

    if(pszServerName == NULL || strlen(pszServerName) == 0)
    {
        // SEC_E_WRONG_PRINCIPAL;
		return false;
    }

    cchServerName	= MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, NULL, 0);
    pwszServerName	= (PWSTR)LocalAlloc(LMEM_FIXED, cchServerName * sizeof(WCHAR));
    if(pwszServerName == NULL)
    {
        // SEC_E_INSUFFICIENT_MEMORY;
		return false;
    }

	bool	bRet	=	true;

	cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, pwszServerName, cchServerName);
    if(cchServerName == 0)
    {
        //SEC_E_WRONG_PRINCIPAL;
		bRet = false;
		goto cleanup;
    }


    //
    // Build certificate chain.
    //

    ZeroMemory(&ChainPara, sizeof(ChainPara));
    ChainPara.cbSize = sizeof(ChainPara);

    if(!CertGetCertificateChain(
                            NULL,
                            pServerCert,
                            NULL,
                            pServerCert->hCertStore,
                            &ChainPara,
                            0,
                            NULL,
                            &pChainContext))
    {
        CString		szError;
		Status = GetLastError();
        szError.Format(_T("Error 0x%x returned by CertGetCertificateChain!\n"), Status);
		ATLTRACE((LPCTSTR)szError);
        goto cleanup;
    }


    //
    // Validate certificate chain.
    // 

    ZeroMemory(&polHttps, sizeof(HTTPSPolicyCallbackData));
    polHttps.cbStruct           = sizeof(HTTPSPolicyCallbackData);
    polHttps.dwAuthType         = AUTHTYPE_SERVER;
    polHttps.fdwChecks          = dwCertFlags;
    polHttps.pwszServerName     = pwszServerName;

    memset(&PolicyPara, 0, sizeof(PolicyPara));
    PolicyPara.cbSize            = sizeof(PolicyPara);
    PolicyPara.pvExtraPolicyPara = &polHttps;

    memset(&PolicyStatus, 0, sizeof(PolicyStatus));
    PolicyStatus.cbSize = sizeof(PolicyStatus);

    if(!CertVerifyCertificateChainPolicy(
                            CERT_CHAIN_POLICY_SSL,
                            pChainContext,
                            &PolicyPara,
                            &PolicyStatus))
    {
        CString		szError;
		Status = GetLastError();
        szError.Format(_T("Error 0x%x returned by CertVerifyCertificateChainPolicy!\n"), Status);
		ATLTRACE((LPCTSTR)szError);
        goto cleanup;
    }



	if(PolicyStatus.dwError)
    {
		g_Sample_SecurityInit.traceVerifyTrustError( PolicyStatus.dwError );
		bRet	=	false;
        goto cleanup;
    }


    Status = SEC_E_OK;

cleanup:

    if(pChainContext)
    {
        CertFreeCertificateChain(pChainContext);
    }

	if(pwszServerName)
	{
		::LocalFree(pwszServerName);
	}

	return bRet;
}

// Overrides for Read, Write
inline bool CSecureEvtSyncSocket::Write(WSABUF *pBuffers, int nCount, DWORD *pdwSize) 
{
	bool bRet	=	true;
	if( !m_bHttpsCommunication	)
		return internalWrite( pBuffers, nCount, pdwSize);


	
	SecPkgContext_StreamSizes		Sizes;
    SECURITY_STATUS					scRet;
    DWORD							dwIoBufferLength;
	WSABUF							*arInternalBuffers = NULL;
	int								iIndex ;

	


    //
    // Read stream encryption properties.
    //

    scRet = g_Sample_SecurityInit._SecurityFunc.QueryContextAttributesA(&m_hContext,
					SECPKG_ATTR_STREAM_SIZES,
                    &Sizes);
    if(scRet != SEC_E_OK)
    {
        CString		szError;
		szError.Format(_T("**** Error 0x%x reading SECPKG_ATTR_STREAM_SIZES\n"), scRet);
		ATLTRACE((LPCTSTR)szError);
        return false;
    }
	arInternalBuffers	=	new WSABUF[nCount];
	memset(arInternalBuffers, 0, nCount*sizeof(WSABUF));

	dwIoBufferLength	=	Sizes.cbHeader + Sizes.cbMaximumMessage +   Sizes.cbTrailer;
	

	*pdwSize	=	0;
	for( iIndex = 0; iIndex < nCount; iIndex ++ )
	{
		PBYTE			pbIoBuffer = NULL;
		DWORD			dwMessageSize	=	pBuffers[iIndex].len;
		SecBufferDesc	Message;
		SecBuffer		Buffers[4];

		*pdwSize	+=	dwMessageSize;
		
		pbIoBuffer	=	new BYTE[dwIoBufferLength];

		ATLASSERT(pbIoBuffer != NULL);
		ATLASSERT( dwMessageSize < Sizes.cbMaximumMessage);

		// --------------------------------------------------------
		// lsu: changed sign compared to the MSDN source !!!
		// --------------------------------------------------------
		if (NULL == pbIoBuffer || dwMessageSize > Sizes.cbMaximumMessage)
		{
			bRet = false;
			if(pbIoBuffer)delete []pbIoBuffer;
			break;
		}

		memcpy_s( pbIoBuffer + Sizes.cbHeader, sizeof(pbIoBuffer) - Sizes.cbHeader, pBuffers[iIndex].buf, dwMessageSize );

		Buffers[0].pvBuffer     = pbIoBuffer;
		Buffers[0].cbBuffer     = Sizes.cbHeader;
		Buffers[0].BufferType   = SECBUFFER_STREAM_HEADER;

		Buffers[1].pvBuffer     = pbIoBuffer	+	Sizes.cbHeader;
		Buffers[1].cbBuffer     = dwMessageSize;
		Buffers[1].BufferType   = SECBUFFER_DATA;

		Buffers[2].pvBuffer     = pbIoBuffer	+	Sizes.cbHeader + dwMessageSize;
		Buffers[2].cbBuffer     = Sizes.cbTrailer;
		Buffers[2].BufferType   = SECBUFFER_STREAM_TRAILER;

		Buffers[3].BufferType   = SECBUFFER_EMPTY;

		Message.ulVersion       = SECBUFFER_VERSION;
		Message.cBuffers        = 4;
		Message.pBuffers        = Buffers;

		scRet = g_Sample_SecurityInit._SecurityFunc.EncryptMessage(&m_hContext, 0, &Message, 0);

		if(FAILED(scRet))
		{
			CString		szError;
			szError.Format(_T("**** Error 0x%x returned by EncryptMessage\n"), scRet);
			ATLTRACE((LPCTSTR)szError);
			bRet	=	false;
		}

		arInternalBuffers[iIndex].len	=	Buffers[0].cbBuffer+Buffers[1].cbBuffer+Buffers[2].cbBuffer;
		arInternalBuffers[iIndex].buf	=	(PCHAR)pbIoBuffer;

	}

	DWORD		dwWrittenEnc;
	if( bRet )
		bRet	=	internalWrite( arInternalBuffers, nCount, &dwWrittenEnc);

	for( iIndex = 0; iIndex < nCount; iIndex ++)
	{
		delete[] arInternalBuffers[iIndex].buf;
	}
	delete[]	arInternalBuffers;

	if( !bRet)
		*pdwSize	=	0;
		



	return bRet;

}

inline bool CSecureEvtSyncSocket::Read(const unsigned char *pBuff, DWORD *pdwSize) 
{
	bool bRet	=	true;
	if( !m_bHttpsCommunication	)
		return internalRead( pBuff, pdwSize);

	unsigned char	*pReadBuff = NULL;
	
	SecPkgContext_StreamSizes		Sizes;
    SECURITY_STATUS					scRet;
    DWORD							dwIoBufferLength;
	SecBuffer					    ExtraBuffer;
	DWORD							dwIoBuffer = 0;
	DWORD							dwActualReadSize = 0;


    //
    // Read stream encryption properties.
    //
    scRet = g_Sample_SecurityInit._SecurityFunc.QueryContextAttributesA(&m_hContext,
					SECPKG_ATTR_STREAM_SIZES,
                    &Sizes);
    if(scRet != SEC_E_OK)
    {
        CString		szError;
		szError.Format(_T("**** Error 0x%x reading SECPKG_ATTR_STREAM_SIZES\n"), scRet);
		ATLTRACE((LPCTSTR)szError);
        return false;
    }

	// This asserts that the size of the answer is smaller than maximum encrypted message size and ATL_READ_BUFF_SIZE
	dwIoBufferLength	=	Sizes.cbHeader + min(ATL_READ_BUFF_SIZE, Sizes.cbMaximumMessage) +   Sizes.cbTrailer;
	pReadBuff			=	new unsigned char[dwIoBufferLength];


	while(TRUE)
    {

		SecBufferDesc	Message;
		SecBuffer		Buffers[4];
        
		//
        // Read some data.
        //
        if(0 == dwIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
			 DWORD	dwRead = dwIoBufferLength - dwIoBuffer;
			bool	bReadRet;

			bReadRet	=	internalRead( (unsigned char*)pReadBuff + dwIoBuffer, &dwRead);
            if(!bReadRet)
            {
                CString	szError;
				szError.Format(_T("**** Error %d reading data from server\n"), WSAGetLastError());
                scRet = SEC_E_INTERNAL_ERROR;
                bRet	=	false;
				break;
            }
            else if(dwRead == 0)
            {
                // Server disconnected.
				bRet	=	false;
				break;
            }
            else
            {
                dwIoBuffer += dwRead;
            }
        }

        // 
        // Attempt to decrypt the received data.
        //

        Buffers[0].pvBuffer     = pReadBuff;
        Buffers[0].cbBuffer     = dwIoBuffer;
        Buffers[0].BufferType   = SECBUFFER_DATA;

        Buffers[1].BufferType   = SECBUFFER_EMPTY;
        Buffers[2].BufferType   = SECBUFFER_EMPTY;
        Buffers[3].BufferType   = SECBUFFER_EMPTY;

        Message.ulVersion       = SECBUFFER_VERSION;
        Message.cBuffers        = 4;
        Message.pBuffers        = Buffers;

        scRet = g_Sample_SecurityInit._SecurityFunc.DecryptMessage(&m_hContext, &Message, 0, NULL);

        if(scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            // The input buffer contains only a fragment of an
            // encrypted record. Loop around and read some more
            // data.
            continue;
        }

        // Server signalled end of session
        if(scRet == SEC_I_CONTEXT_EXPIRED)
		{
			bRet	=	false;
			break;
		}

        if( scRet != SEC_E_OK && 
            scRet != SEC_I_RENEGOTIATE && 
            scRet != SEC_I_CONTEXT_EXPIRED)
        {
            CString	szError;
			szError.Format(_T("**** Error 0x%x returned by DecryptMessage\n"), scRet);
			ATLTRACE( szError );
            bRet	=	false;
			break;
        }

        // Locate data and (optional) extra buffers.
		SecBuffer *     pDataBuffer;
		SecBuffer *     pExtraBuffer;

        pDataBuffer  = NULL;
        pExtraBuffer = NULL;
        for(int i = 1; i < 4; i++)
        {

            if(pDataBuffer == NULL && Buffers[i].BufferType == SECBUFFER_DATA)
            {
                pDataBuffer = &Buffers[i];
            }
            if(pExtraBuffer == NULL && Buffers[i].BufferType == SECBUFFER_EXTRA)
            {
                pExtraBuffer = &Buffers[i];
            }
        }

        // Display or otherwise process the decrypted data.
        if(pDataBuffer)
        {
            ATLASSERT(pDataBuffer->cbBuffer < (*pdwSize - dwActualReadSize));

			// --------------------------------------------------------
			// lsu: changed sign compared to the MSDN source !!!
			// --------------------------------------------------------
			if (pDataBuffer->cbBuffer > (*pdwSize - dwActualReadSize))
			{
				bRet	=	false;
				break;
			}

			memcpy_s( (void*)(pBuff + dwActualReadSize), pDataBuffer->cbBuffer, (PBYTE)pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);
			 //*pdwSize	=	pDataBuffer->cbBuffer;
			dwActualReadSize += pDataBuffer->cbBuffer;
        }

        // Move any "extra" data to the input buffer.
        if(pExtraBuffer)
        {
            memmove_s(pReadBuff, dwIoBufferLength, pExtraBuffer->pvBuffer, pExtraBuffer->cbBuffer);
            dwIoBuffer = pExtraBuffer->cbBuffer;
        }
        else
        {
            // That's all, no extra buffer, everything is read, so
			break;
			//dwIoBuffer = 0;

        }

        if(scRet == SEC_I_RENEGOTIATE)
        {
            // The server wants to perform another handshake
            // sequence.

            ATLTRACE("Server requested renegotiate!\n");

            scRet = clientHandshakeLoop(m_socket, 
                                        FALSE, 
                                        &ExtraBuffer);
            if(scRet != SEC_E_OK)
            {
                bRet	=	false;
				break;
            }
            // Move any "extra" data to the input buffer.
            if(ExtraBuffer.pvBuffer)
            {
                memmove_s(pReadBuff, dwIoBufferLength, ExtraBuffer.pvBuffer, ExtraBuffer.cbBuffer);
                dwIoBuffer = ExtraBuffer.cbBuffer;
            }
        }
		//bRet	=	true;
		//break;
    }


	delete[] pReadBuff;

	if( bRet)
			*pdwSize	=	dwActualReadSize;

	return bRet;
}


inline bool CSecureEvtSyncSocket::disconnectFromSecureServer()
{
    DWORD           dwType;
    char*			pbMessage;
    DWORD           cbMessage;
    DWORD           cbData;

    SecBufferDesc   OutBuffer;
    SecBuffer       OutBuffers[1];
    DWORD           dwSSPIFlags;
    DWORD           dwSSPIOutFlags;
    TimeStamp       tsExpiry;
    DWORD           Status;

    //
    // Notify schannel that we are about to close the connection.
    //

    dwType = SCHANNEL_SHUTDOWN;

    OutBuffers[0].pvBuffer   = &dwType;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer   = sizeof(dwType);

    OutBuffer.cBuffers  = 1;
    OutBuffer.pBuffers  = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    Status = g_Sample_SecurityInit._SecurityFunc.ApplyControlToken(&m_hContext, &OutBuffer);

    if(FAILED(Status)) 
    {
        CString	szError;
		szError.Format(_T("**** Error 0x%x returned by ApplyControlToken\n"), Status);
		ATLTRACE(szError);
        goto cleanup;
    }

    //
    // Build an SSL close notify message.
    //

    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT   |
                  ISC_REQ_REPLAY_DETECT     |
                  ISC_REQ_CONFIDENTIALITY   |
                  ISC_RET_EXTENDED_ERROR    |
                  ISC_REQ_ALLOCATE_MEMORY   |
                  ISC_REQ_STREAM;

    OutBuffers[0].pvBuffer   = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer   = 0;

    OutBuffer.cBuffers  = 1;
    OutBuffer.pBuffers  = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    Status = g_Sample_SecurityInit._SecurityFunc.InitializeSecurityContextA(
                    &m_hClientCreds,
                    &m_hContext,
                    NULL,
                    dwSSPIFlags,
                    0,
                    SECURITY_NATIVE_DREP,
                    NULL,
                    0,
                    &m_hContext,
                    &OutBuffer,
                    &dwSSPIOutFlags,
                    &tsExpiry);

    if(FAILED(Status)) 
    {
        CString szError;
		szError.Format(_T("**** Error 0x%x returned by InitializeSecurityContext\n"), Status);
		ATLTRACE(szError);
        goto cleanup;
    }

    pbMessage = (char*)OutBuffers[0].pvBuffer;
    cbMessage = OutBuffers[0].cbBuffer;


    //
    // Send the close notify message to the server.
    //

    if(pbMessage != NULL && cbMessage != 0)
    {
		WSABUF	wsaBuff;
		wsaBuff.len	=  cbMessage;
		wsaBuff.buf = pbMessage;
		bool	bTmp = internalWrite( &wsaBuff, 1, &cbData);
        
		// cbData = send(m_socket, pbMessage, cbMessage, 0);
        if(cbData == SOCKET_ERROR || cbData == 0)
        {
            Status = WSAGetLastError();
            CString	szError;
			szError.Format(_T("**** Error %d sending close notify\n"), Status);
            ATLTRACE(szError);
			goto cleanup;
        }

        ATLTRACE("Sending Close Notify\n");


        // Free output buffer.
		g_Sample_SecurityInit._SecurityFunc.FreeContextBuffer(pbMessage);
    }
    

cleanup:

    // Free the security context.
    g_Sample_SecurityInit._SecurityFunc.DeleteSecurityContext(&m_hContext);


    return true;
}

#endif // __SECURESOCKET_INL__

