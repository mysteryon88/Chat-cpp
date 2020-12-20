#pragma once
#include <afxsock.h>

class CSock : public  CAsyncSocket
{
public:
	CSock();
	virtual ~CSock() {}

	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnAccept(int nErrorCode);

	CWnd* m_pParent;
};