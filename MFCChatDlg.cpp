﻿#include "pch.h"
#include "MFCChatDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMFCChatDlg::CMFCChatDlg(CWnd* pParent)
	: CDialogEx(IDD_MFCCHAT_DIALOG, pParent)
	, IPValue(0)
	, NiknameVal(_T("Введите имя"))
	, PortVal(_T("777"))
	, ServerCheck(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(Chat_Icon);
}

void CMFCChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, IPControl);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, IPValue);
	DDX_Control(pDX, IDC_B_Server, ButtonStartServerControl);
	DDX_Control(pDX, IDC_EDIT1, NiknameControl);
	DDX_Text(pDX, IDC_EDIT1, NiknameVal);
	DDX_Control(pDX, IDC_EDIT2, PortControl);
	DDX_Text(pDX, IDC_EDIT2, PortVal);
	DDX_Control(pDX, IDC_B_Client, ButtonClientControl);
	DDX_Control(pDX, IDC_B_Stop_Chat, ButtonStopChatControl);
	DDX_Control(pDX, IDC_People, CountPeopleControl);
	DDX_Control(pDX, IDC_B_Send_Mes, ButtonSendMesControl);
	DDX_Control(pDX, IDC_B_Send_File, ButtonSendFileControl);
	DDX_Control(pDX, IDC_EDIT3, SendWindowControl);
	DDX_Control(pDX, IDC_EDIT4, ChatWindowControl);
	DDX_Control(pDX, IDC_IP, IPv4);
}

BEGIN_MESSAGE_MAP(CMFCChatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_B_Client, &CMFCChatDlg::OnBnClickedBClient)
	ON_BN_CLICKED(IDC_B_Server, &CMFCChatDlg::OnBnClickedBServer)
	ON_BN_CLICKED(IDC_B_Send_File, &CMFCChatDlg::OnBnClickedBSendFile)
	ON_BN_CLICKED(IDC_B_Stop_Chat, &CMFCChatDlg::OnBnClickedBStopChat)
	ON_BN_CLICKED(IDC_B_Send_Mes, &CMFCChatDlg::OnBnClickedBSendMes)
END_MESSAGE_MAP()


// Обработчики сообщений CMFCChatDlg

BOOL CMFCChatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	BYTE p1 = 127, p2 = 0, p3 = 0, p4 = 1;
	IPControl.SetAddress(p1, p2, p3, p4);

	MainSocket.m_pParent = this;
		
	SendWindowControl.SetLimitText(200);

	NiknameControl.SetLimitText(12);

	IPv4.SetWindowText(L"Ваш IP" + FindIP());

	return TRUE;  
}

void CMFCChatDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CMFCChatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		short cxIcon = GetSystemMetrics(SM_CXICON);
		short cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		short x = (rect.Width() - cxIcon + 1) / 2;
		short y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMFCChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCChatDlg::OnOK() {}

void CMFCChatDlg::OnCancel()
{
	if (MessageBox(L"Закрыть программу?", L"Chat", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		StopChat();
		EndDialog(0);
	}
}

void CMFCChatDlg::OnBnClickedBClient()
{
	ServerCheck = false;
	if (MainSocket.m_hSocket != INVALID_SOCKET) return;

	if (!QueryName())
	{
		MessageBox(L"Введите свое имя для чата!", L"Chat", MB_OK | MB_ICONWARNING);
		StopChat();
		return;
	}

	if (MainSocket.Create() == TRUE)
	{
		BYTE p1, p2, p3, p4;
		CString temp, IPAdressServer, strPort;
		IPControl.GetAddress(p1, p2, p3, p4);
		temp.Format(L"%d.", p1);
		IPAdressServer = temp;
		temp.Format(L"%d.", p2);
		IPAdressServer += temp;
		temp.Format(L"%d.", p3);
		IPAdressServer += temp;
		temp.Format(L"%d", p4);
		IPAdressServer += temp;
		PortControl.GetWindowText(strPort);// _ttoi(strPort)
		if (MainSocket.Connect(IPAdressServer, _ttoi(strPort)) == FALSE)
		{
			// В ассинхронном режиме код этой ошибки
			// считается как ожидание события подключения,
			// т.е. практически успешный возврат.
			if (GetLastError() == WSAEWOULDBLOCK)
			{
				DisabledControl(false);
			}
			else
			{
				StopChat();
			}
		}
	}
}

void CMFCChatDlg::OnBnClickedBServer()
{
	ServerCheck = true;
	if (MainSocket.m_hSocket != INVALID_SOCKET) return;

	if (!QueryName())
	{
		MessageBox(L"Введите свое имя для чата!", L"Chat", MB_OK | MB_ICONWARNING);
		StopChat();
		return;
	}

	CString Port;
	PortControl.GetWindowText(Port);

	if (MainSocket.Create(_ttoi(Port)) == FALSE)
	{
		MessageBox(L"Ошибка создания сервера!\nВозможно данный порт уже используется!", L"Chat", MB_ICONWARNING);
		StopChat();
	}
	else
	{
		if (MainSocket.Listen() == FALSE) StopChat();
		else
		{
			DisabledControl(true);
			SetWindowText(L"Сервер запущен, сервер ждёт подключения!");
			CountPeopleControl.SetWindowText(L"В чате 1 человек");
		}
	}
}

void CMFCChatDlg::OnBnClickedBSendFile()
{
	CFileDialog dlg(TRUE);
	CString path, filename;
	static uint8_t numderfile = 0;

	if (dlg.DoModal() == IDOK) path = dlg.m_ofn.lpstrFile;
	
	
	filename.Format(L"%d", numderfile);
	uint16_t lenpath = path.GetLength();
	std::string::size_type found = path.ReverseFind(L'\\');
	filename += path.Right(lenpath - found - 1);

	std::ifstream file(path, std::ios::binary);
	if (file.is_open())
	{
		SENDBUFFER sb;

		uint16_t len = filename.GetLength();
		memcpy(sb.filename, filename.GetBuffer(), sizeof(TCHAR) * len);

		sb.typemessage = m_TypeMessage::tmFile;

		file.seekg(0, std::ios::end);
		uint32_t length = file.tellg();
		file.seekg(0, std::ios::beg);

		uint32_t ostatok = length % PACK;
		uint32_t end = ceil(length / PACK);

		for (uint32_t i = 0; i <= end; i++) {
			file.read(sb.filebuffer, PACK);
			i == end ? sb.filebuffersize = ostatok : sb.filebuffersize = PACK;
			SendBuffer(sb, true);
		}
		file.close();
		SendToChat(L"Файл "+ CString(sb.filename) + L" отправлен!");
		numderfile++;
	}
}

bool CMFCChatDlg::QueryName(void)
{
	CString strName;
	NiknameControl.GetWindowText(strName);
	if (strName == NiknameVal || strName.IsEmpty())return false;

	return true;
}

void CMFCChatDlg::DisabledControl(bool server)
{
	// Запреты.
	IPControl.EnableWindow(FALSE);
	PortControl.EnableWindow(FALSE);
	ButtonSendMesControl.EnableWindow(FALSE);
	ButtonSendFileControl.EnableWindow(FALSE);

	if (server) ButtonClientControl.EnableWindow(FALSE);
	else ButtonStartServerControl.EnableWindow(FALSE);
	
	// Разрешения.
	// Разрешить возможность выхода из чата.
	ButtonStopChatControl.EnableWindow(TRUE);
}

void CMFCChatDlg::EnabledControl(void)
{
	// Разрешения.
	IPControl.EnableWindow(TRUE);
	PortControl.EnableWindow(TRUE);
	ButtonClientControl.EnableWindow(TRUE);
	ButtonStartServerControl.EnableWindow(TRUE);

	// Запреты.
	ButtonStopChatControl.EnableWindow(FALSE);
	ButtonSendFileControl.EnableWindow(FALSE);
	ButtonSendMesControl.EnableWindow(FALSE);
}

void CMFCChatDlg::OnBnClickedBStopChat()
{
	StopChat();
}

void CMFCChatDlg::StopChat()
{
	// Отсылаем сигнал об отключении от чата.
	SendDisconnect();

	MainSocket.Close();
	for (uint8_t i = 0; i < (uint8_t)VecSockets.size(); i++)
	{
		VecSockets[i]->Close();
		delete [] VecSockets[i];
	}
	// Очистим вектор от ненужных элементов.
	VecSockets.clear();

	ButtonStartServerControl.SetCheck(BST_UNCHECKED);
	ButtonClientControl.SetCheck(BST_UNCHECKED);

	// Разрешим доступ к управлению для повторных попыток.
	EnabledControl();

	// В чате нет никого.
	CountPeopleControl.SetWindowText(L"В чате 0 человек");
	SetWindowText(L"Нет сети!");
}

void CMFCChatDlg::OnBnClickedBSendMes()
{
	CString StrChat;
	SendWindowControl.GetWindowText(StrChat);
	SendToChat(StrChat);
}

void CMFCChatDlg::SendToChat(CString strMessage)
{
	SENDBUFFER sb;
	short len = strMessage.GetLength();
	memcpy(sb.buffer, strMessage.GetBuffer(), sizeof(TCHAR) * len);
	NiknameControl.GetWindowText(strMessage);
	len = strMessage.GetLength();
	memcpy(sb.name, strMessage.GetBuffer(), sizeof(TCHAR) * len);
	sb.typemessage = m_TypeMessage::tmChat;
	SendBuffer(sb, true);
}

void CMFCChatDlg::SendBuffer(SENDBUFFER sb, bool toserver)
{
	// Если сокет не создан, нечего делать в этой функции.
	if (MainSocket.m_hSocket == INVALID_SOCKET) return;

	if (ServerCheck)
	{
		for (uint8_t i = 0; i < (uint8_t)VecSockets.size(); i++)
		{
			unsigned int send = VecSockets[i]->Send(&sb, sizeof(SENDBUFFER));
			if (send == sizeof(SENDBUFFER))	SendWindowControl.SetWindowText(L"");
		}

		// Если непосредственно отправку осуществляет сервер,
		// отобразим его сообщение в его же окне отправки,
		// флаг toserver необходим поскольку данная функция
		// может работать в режиме зеркала см. CChatCppDlg::OnReceive(void).
		if (toserver)
		{
			if (sb.typemessage == m_TypeMessage::tmChat)
			{
				CString strChat;
				ChatWindowControl.GetWindowText(strChat);
				strChat += L"\n    " + CString(sb.name) + L": " + CString(sb.buffer) + L"\r\n";
				ChatWindowControl.SetWindowText(strChat);
				unsigned short number_line = ChatWindowControl.GetLineCount();
				ChatWindowControl.LineScroll(number_line);
			}
			if (sb.typemessage == m_TypeMessage::tmDisconnect)
			{
				CString strChat;
				ChatWindowControl.GetWindowText(strChat);
				strChat += L"\n    " + CString(sb.name) + L": " + L"Чат остановлен!" + L"\r\n";
				ChatWindowControl.SetWindowText(strChat);
				unsigned short number_line = ChatWindowControl.GetLineCount();
				ChatWindowControl.LineScroll(number_line);
			}
		}

	}
	else if (ServerCheck == false)
	{
	    unsigned int send = MainSocket.Send(&sb, sizeof(SENDBUFFER));
		if (send == sizeof(SENDBUFFER)) SendWindowControl.SetWindowText(L"");
	}
}

void CMFCChatDlg::OnConnect(BOOL Error)
{
	if (Error == TRUE)
	{
		AfxMessageBox(L"Попытка подключения была отвергнута!\nВозможно сервер еще не создан!");
		StopChat();
	}
	else
	{
		ButtonSendMesControl.EnableWindow(TRUE);
		ButtonSendFileControl.EnableWindow(TRUE);
		SetWindowText(L"Сеть работает!");
	}
}

void CMFCChatDlg::OnAccept(void)
{

	CSock* pSock = new CSock;
	pSock->m_pParent = this;

	// Если все в порядке добавим рабочий сокет в список 
	// подключенных рабочих сокетов.
	if (MainSocket.Accept(*pSock) == TRUE)
	{
		VecSockets.push_back(pSock);
		ButtonSendMesControl.EnableWindow(TRUE);
		ButtonSendFileControl.EnableWindow(TRUE);
		SendCountPeople();

		SetWindowText(L"Сеть работает!");
	}
	else
	{
		delete pSock;
	}
}

CString CMFCChatDlg::FindIP()
{
	CString IP;
	std::string line;
	int8_t count = 0;
	system("ipconfig>ip.txt");
	std::ifstream IPfile(L"ip.txt");
	if (IPfile.is_open())
	{		
		while (getline(IPfile, line))
		{
			if (line.find("IPv4") != -1)
			{
				count++;
				IP = line.c_str();
				IP = IP.Right(IP.GetLength() - 37);
				//обычно 2 раза попадается в файлике на винде
				if (count == 2)	break; 
			}
		}
	}
	IPfile.close();
	std::remove("ip.txt");
	return IP;
}

void CMFCChatDlg::SendCountPeople(void)
{
	uint8_t countpeople = 1 + (uint8_t)VecSockets.size();
	CString temp;
	temp.Format(L"%d", countpeople);
	CountPeopleControl.SetWindowText(L"В чате " + temp + L" человек");

	// Отправим сообщение о количествах людей в чате.
	SENDBUFFER sb;
	sb.typemessage = m_TypeMessage::tmCountPeople;
	sb.countpeople = countpeople;

	for (uint8_t i = 0; i < (uint8_t)VecSockets.size(); i++)
		VecSockets[i]->Send(&sb, sizeof(SENDBUFFER));
	
}

void CMFCChatDlg::SendDisconnect(void)
{
	SENDBUFFER sb;
	CString s;
	NiknameControl.GetWindowText(s);
	uint8_t len = s.GetLength();
	memcpy(sb.name, s.GetBuffer(), sizeof(TCHAR) * len);
	sb.typemessage = m_TypeMessage::tmDisconnect;

	// Отсоединение сервера останавливает чат.
	if (ServerCheck)
		sb.stopchat = true;

	SendBuffer(sb, true);
}

void CMFCChatDlg::OnReceive(void)
{
	SENDBUFFER sb;
	if (ServerCheck)
	{
		// Сервер несет большую нагрузку при получении сообщений.
		for (uint8_t index = 0; index < (uint8_t)VecSockets.size(); index++)
		{
			VecSockets[index]->Receive(&sb, sizeof(SENDBUFFER));
			// Если кто-то отключился, удаляем этого клиента 
			// из списка доступных клиентов.
			if (sb.typemessage == m_TypeMessage::tmDisconnect)
			{
				VecSockets[index]->Close();
				delete [] VecSockets[index];
				VecSockets.erase(VecSockets.begin() + index);

				SendCountPeople();

				SendBuffer(sb, false);
			
				break;
			}
			// Рассылаем сообщения клиента по сети чата.
			if (sb.typemessage == m_TypeMessage::tmChat)
			{
				SendBuffer(sb, false);
				break;
			}
			if (sb.typemessage == m_TypeMessage::tmFile)
			{
				SendBuffer(sb, false);
				break;
			}
		}

	}
	else if (ServerCheck == false)
	{
		MainSocket.Receive(&sb, sizeof(SENDBUFFER));
	}

	// Обработка принятого сообщения на основе его типа.
	switch (sb.typemessage)
	{
	case m_TypeMessage::tmCountPeople:
	{
		CString temp;
		temp.Format(L"%d", sb.countpeople);
		CountPeopleControl.SetWindowText(L"В чате " + temp + L" человек");
	}
	break;
	case m_TypeMessage::tmChat:
	{
		CString strChat;
		ChatWindowControl.GetWindowText(strChat);
		strChat += L"    " + CString(sb.name) + L": " + CString(sb.buffer) + L"\r\n";
		ChatWindowControl.SetWindowText(strChat);
		uint16_t number_line = ChatWindowControl.GetLineCount();
		ChatWindowControl.LineScroll(number_line);
	}
	break;
	case m_TypeMessage::tmDisconnect:
	{
		CString strChat;
		ChatWindowControl.GetWindowText(strChat);

		// Если принято сообщение об остановки чата(отключении сервера),
		// изменим содержимое сообщения.
		if (sb.stopchat == true)
		{
			StopChat();
			strChat +=  L"    " + CString(sb.name) + L": Чат остановлен!" + L"\r\n";
		}
		else
		{
			strChat += L"    " + CString(sb.name) + L" - покинул(а) чат!" + L"\r\n";
		}
		ChatWindowControl.SetWindowText(strChat);
		uint16_t number_line = ChatWindowControl.GetLineCount();
		ChatWindowControl.LineScroll(number_line);
	}
	break;
	case m_TypeMessage::tmFile:
	{ 

		std::ofstream fout( CString(sb.filename), std::ios::binary | std::ios::app);

		if (fout.is_open())	fout.write(sb.filebuffer, sb.filebuffersize);
		
		fout.close();
	}
	break;
	default:
		AfxMessageBox(L"Неизвестное сообщение!");
		break;
	}
}

