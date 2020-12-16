#pragma once
#include "pch.h"
#include "framework.h"

struct SENDBUFFER
{
	SENDBUFFER()
	{
		stopchat = false;
		typemessage = 0;
		countpeople = 0;
		filebuffersize = 0;
		ZeroMemory(name, sizeof(TCHAR) * 14);
		ZeroMemory(filename, sizeof(TCHAR) * 33);
		ZeroMemory(buffer, sizeof(TCHAR) * 202);
		ZeroMemory(filebuffer, sizeof(char) * 2048);
	}
	~SENDBUFFER() {}
	bool stopchat;
	uint8_t typemessage;
	uint8_t countpeople;
    unsigned short filebuffersize;
	TCHAR name[14];
	TCHAR filename[33];
	TCHAR buffer[202];
    char filebuffer[2048];
};


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
	//проверка имени
	bool QueryName(void);
	//Запреты 
	void DisabledControl(bool server);
	//Разрешения
	void EnabledControl(void);
	//Закрытие чата
	void StopChat();
	//Перегрузка для ОК
	void OnOK();
	//Перегрузка для выхода
	void OnCancel();
	//Послать строку-сообщение в чат
	void SendToChat(CString strMessage);
	// Послать буфер подготовленного сообщения в сеть
	void SendBuffer(SENDBUFFER sb, bool toserver);
	// Сервер отправляет клиентам количество людей в чате
	void SendCountPeople(void);
	// Формируем и отправляем сообщение об отключении от сети
	void SendDisconnect(void);
private:
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
	CSock MainSocket;
	std::vector<CSock*> VecSockets;
	DWORD IPValue;
	CString NiknameVal;
	CString PortVal;
	enum m_TypeMessage { tmCountPeople = 1, tmChat, tmDisconnect, tmFile };
	//что запущено сервер true, клиент false
	bool ServerCheck;
public:
	// Извлечение сообщений из сети чата
	void OnReceive(void);
	// Событие подключения, вызывается на стороне клиента
	void OnConnect(BOOL Error);
	// Принимаем запросы на подключения
	void OnAccept(void);
	//поиск своего IP
    CString	FindIP();
	CStatic IPv4;
};
