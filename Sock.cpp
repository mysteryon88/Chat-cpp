#include "pch.h"
#include "Sock.h"
#include "MFCChatDlg.h"

CSock::CSock()
	: m_pParent(NULL)
{
}

// Событие подключения на стороне клиентского приложения.
void CSock::OnConnect(int nErrorCode)
{
	//// Данные в родительское окно для индикации процесса соединения.
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	nErrorCode == 0 ? pDlg->OnConnect(FALSE) : pDlg->OnConnect(TRUE);
	CAsyncSocket::OnConnect(nErrorCode);
}

// Событие отключения от сети
void CSock::OnClose(int nErrorCode)
{
	Beep(2000, 300);

	CAsyncSocket::OnClose(nErrorCode);
}

// Событие получения сообщений.
void CSock::OnReceive(int nErrorCode)
{
	// Данные в родительское окно для визуальности приема сообщений.
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	pDlg->OnReceive();

	CAsyncSocket::OnReceive(nErrorCode);
}

// Запрос на подключение, направляемый клиентом серверу.
// Происходит на стороне серверного приложения.
void CSock::OnAccept(int nErrorCode)
{
	// Данные в родительское окно для индикации процесса соединения.
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	pDlg->OnAccept();

	CAsyncSocket::OnAccept(nErrorCode);
}
