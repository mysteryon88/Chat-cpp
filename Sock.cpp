#include "pch.h"
#include "Sock.h"
#include "MFCChatDlg.h"

CSock::CSock()
	: m_pParent(NULL)
{
}
void CSock::OnConnect(int nErrorCode)
{
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	nErrorCode == 0 ? pDlg->OnConnect(FALSE) : pDlg->OnConnect(TRUE);
	CAsyncSocket::OnConnect(nErrorCode);
}

void CSock::OnClose(int nErrorCode)
{
	Beep(2000, 300);
	CAsyncSocket::OnClose(nErrorCode);
}

void CSock::OnReceive(int nErrorCode)
{
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	pDlg->OnReceive();

	CAsyncSocket::OnReceive(nErrorCode);
}

void CSock::OnAccept(int nErrorCode)
{
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	pDlg->OnAccept();

	CAsyncSocket::OnAccept(nErrorCode);
}
