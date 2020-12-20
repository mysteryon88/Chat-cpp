#pragma once
#include "pch.h"
#include "framework.h"
#include "InfoBuf.h"

class CMFCChatDlg : public CDialogEx
{

public:
	CMFCChatDlg(CWnd* pParent = nullptr);	


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCHAT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBClient();	
	afx_msg void OnBnClickedBServer();
	afx_msg void OnBnClickedBSendFile();
	afx_msg	void OnBnClickedBStopChat();
	afx_msg void OnBnClickedBSendMes();
	void StopChat();
	void OnOK() {}
	void OnCancel();
	void SendToChat(CString strMessage);
	void SendBuffer(SENDBUFFER sb, bool toserver);
	void SendCountPeople(void);
	void SendDisconnect(void);

	CButton ButtonClientControl;
	CButton ButtonStopChatControl;
	CButton ButtonSendMesControl;
	CButton ButtonSendFileControl;
	CButton ButtonStartServerControl;
	CIPAddressCtrl IPControl;
	CEdit PortControl;
	CEdit NiknameControl;
	CEdit SendWindowControl;
	CEdit ChatWindowControl;
	CStatic CountPeopleControl;
	CStatic IPv4;
	CString NiknameVal;
	CString PortVal;
	DWORD IPValue;

	Service myServ;
	CSock MainSocket;
	std::vector<CSock*> VecSockets;

	enum m_TypeMessage { tmCountPeople = 1, tmChat, tmDisconnect, tmFile };

	bool ServerCheck;
	void OnReceive(void);
	void OnConnect(BOOL Error);
	void OnAccept(void);
};
