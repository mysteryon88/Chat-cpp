
// MFCChatDlg.h: файл заголовка
//

#pragma once
#include "pch.h"
#include <afxwin.h>
#include <vector>
#include <fstream>
#include <string>
#include "framework.h"
#include "MFCChat.h"
#include "Sock.h"
#include "CAboutDlg.h"


struct SENDBUFFER
{
	SENDBUFFER()
	{
		typemessage = 0;
		countpeople = 0;
		stopchat = false;
		ZeroMemory(name, sizeof(TCHAR) * 14);
		ZeroMemory(buffer, sizeof(TCHAR) * 202);
	}
	
	uint8_t typemessage;
	uint8_t countpeople;
	bool stopchat;
	TCHAR name[14];
	TCHAR buffer[202];
};

// Диалоговое окно CMFCChatDlg
class CMFCChatDlg : public CDialogEx
{
// Создание
public:
	CMFCChatDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCHAT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
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
	DWORD IPValue;
	CEdit PortControl;
	CEdit NiknameControl;
	CEdit SendWindowControl;
	CEdit ChatWindowControl;
	CStatic CountPeopleControl;
	CSock MainSocket;
	std::vector<CSock*> VecSockets;
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
