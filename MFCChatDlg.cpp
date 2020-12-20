#include "pch.h"
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

	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		

	BYTE p1 = 127, p2 = 0, p3 = 0, p4 = 1;
	IPControl.SetAddress(p1, p2, p3, p4);

	MainSocket.m_pParent = this;

	myServ.m_pParent = this;

	SendWindowControl.SetLimitText(200);

	NiknameControl.SetLimitText(12);

	IPv4.SetWindowText(L"Ваш IP" + myServ.FindIP());

	return TRUE;  
}

void CMFCChatDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else CDialogEx::OnSysCommand(nID, lParam);	
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
	else CDialogEx::OnPaint();
}

HCURSOR CMFCChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCChatDlg::OnCancel()
{
	myServ.Close();
}

void CMFCChatDlg::OnBnClickedBClient()
{
	ServerCheck = false;
	if (MainSocket.m_hSocket != INVALID_SOCKET) return;

	if (!myServ.Verify()) return;

	if (MainSocket.Create())
	{
		CString temp, IPAdressServer, strPort;
		IPAdressServer = myServ.GetIP();
		PortControl.GetWindowText(strPort);
		if (MainSocket.Connect(IPAdressServer, _ttoi(strPort)) == FALSE)
		{
			if (GetLastError() == WSAEWOULDBLOCK) myServ.DisabledControl(false);
			else StopChat();
		}
	}
}

void CMFCChatDlg::OnBnClickedBServer()
{
	ServerCheck = true;
	if (MainSocket.m_hSocket != INVALID_SOCKET) return;

	if (!myServ.Verify()) return;

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
			myServ.DisabledControl(true);
			SetWindowText(L"Сервер запущен, сервер ждёт подключения!");
			CountPeopleControl.SetWindowText(L"В чате 1 человек");
		}
	}
}

void CMFCChatDlg::OnBnClickedBSendFile()
{
	CString filename;
	static uint8_t numderfile = 0;

	CString path = myServ.OpenFile();
	if (path.IsEmpty()) return;

	filename.Format(L"%d-", numderfile);
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

void CMFCChatDlg::OnBnClickedBSendMes()
{
	CString StrChat;
	SendWindowControl.GetWindowText(StrChat);
	SendToChat(StrChat);
}

void CMFCChatDlg::OnBnClickedBStopChat()
{
	StopChat();
}

void CMFCChatDlg::StopChat()
{
	SendDisconnect();

	MainSocket.Close();
	for (uint8_t i = 0; i < (uint8_t)VecSockets.size(); i++)
	{
		VecSockets[i]->Close();
		delete [] VecSockets[i];
	}
	VecSockets.clear();
	myServ.EnabledControl();
	CountPeopleControl.SetWindowText(L"В чате 0 человек");
	SetWindowText(L"Нет сети!");
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
	if (Error)
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

	if (MainSocket.Accept(*pSock) == TRUE)
	{
		VecSockets.push_back(pSock);
		ButtonSendMesControl.EnableWindow(TRUE);
		ButtonSendFileControl.EnableWindow(TRUE);
		SendCountPeople();
		SetWindowText(L"Сеть работает!");
	}
	else delete pSock;
}

void CMFCChatDlg::SendCountPeople(void)
{
	uint8_t countpeople = 1 + (uint8_t)VecSockets.size();
	CString temp;
	temp.Format(L"%d", countpeople);
	CountPeopleControl.SetWindowText(L"В чате " + temp + L" человек");

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

	if (ServerCheck) sb.stopchat = true;

	SendBuffer(sb, true);
}

void CMFCChatDlg::OnReceive(void)
{
	SENDBUFFER sb;
	if (ServerCheck)
	{
		for (uint8_t index = 0; index < (uint8_t)VecSockets.size(); index++)
		{
			VecSockets[index]->Receive(&sb, sizeof(SENDBUFFER));

			if (sb.typemessage == m_TypeMessage::tmDisconnect)
			{
				VecSockets[index]->Close();
				delete [] VecSockets[index];
				VecSockets.erase(VecSockets.begin() + index);

				SendCountPeople();
				SendBuffer(sb, false);
				break;
			}
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
	else if (!ServerCheck)	MainSocket.Receive(&sb, sizeof(SENDBUFFER));

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

		if (sb.stopchat)
		{
			StopChat();
			strChat +=  L"    " + CString(sb.name) + L": Чат остановлен!" + L"\r\n";
		}
		else strChat += L"    " + CString(sb.name) + L" - покинул(а) чат!" + L"\r\n";

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