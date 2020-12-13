#include "pch.h"
#include "MFCChatDlg.h"
#include "afxdialogex.h"

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

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

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

	if (QueryName() == false)
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
		//MessageBox(IPAdressServer);
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

	if (QueryName() == false)
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
		if (MainSocket.Listen() == FALSE)
		{
			StopChat();
		}
		else
		{
			// Если все в порядке.
			DisabledControl(true);
			SetWindowText(L"Сервер запущен, сервер ждёт подключения!");
			// В чате только сервер.
			CountPeopleControl.SetWindowText(L"В чате 1 человек");
		}
	}
}

void CMFCChatDlg::OnBnClickedBSendFile()
{

	CFileDialog dlg(TRUE);
	CString FullFilename, filename;
	int LenFullFilename = FullFilename.GetLength();

	if (dlg.DoModal() == IDOK) {
		FullFilename = dlg.m_ofn.lpstrFile;
	}
	//MessageBox(FullFilename);

	std::string::size_type found = FullFilename.ReverseFind(L'\\');
	filename = FullFilename.Right(LenFullFilename - found - 1);
	//MessageBox(filename);

	std::ifstream file(FullFilename, std::ios::binary);
	if (file.is_open())
	{
		file.seekg(0, std::ios::end);//указатель в файле на конец
		int length = file.tellg();//столько прочитаем из файла и передадим(размер файла узнаем)
		file.seekg(0, std::ios::beg);//указатель на начало файла

		SENDBUFFER sb;
		sb.typemessage = m_TypeMessage::tmFile;
		sb.countpeople = length;
		//передаем то, что сейчас будет файл
		for (int i = 0; i < (int)VecSockets.size(); i++)
		{
			// Всем клиентам рассылаем знак о том, что сейчас будет файл
			VecSockets[i]->Send(&sb, sizeof(SENDBUFFER));
		}

		SENDFILE sf(length);
		file.read(sf.buffer, length);//читаем в него
		sf.filename = filename;
		for (int i = 0; i < (int)VecSockets.size(); i++)
		{
			// Всем клиентам рассылаем файл
			VecSockets[i]->Send(&sf, sizeof(SENDBUFFER));
		}
		file.close();
	}
}

bool CMFCChatDlg::QueryName(void)
{
	CString strName;
	NiknameControl.GetWindowText(strName);
	if (strName == NiknameVal || strName.IsEmpty() == true) return false;

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
	for (int i = 0; i < (int)VecSockets.size(); i++)
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
	int len = strMessage.GetLength();
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
		for (int i = 0; i < (int)VecSockets.size(); i++)
		{
			int send = VecSockets[i]->Send(&sb, sizeof(SENDBUFFER));
			if (send == sizeof(SENDBUFFER))
			{
				SendWindowControl.SetWindowText(L"");
			}
		}

		// Если непосредственно отправку осуществляет сервер,
		// отобразим его сообщение в его же окне отправки,
		// флаг toserver необходим поскольку данная функция
		// может работать в режиме зеркала см. CChatCppDlg::OnReceive(void).
		if (toserver == true)
		{
			if (sb.typemessage == m_TypeMessage::tmChat)
			{
				CString strChat;
				ChatWindowControl.GetWindowText(strChat);
				strChat += L"\n    " + CString(sb.name) + L": " + CString(sb.buffer) + L"\r\n";
				ChatWindowControl.SetWindowText(strChat);
				int number_line = ChatWindowControl.GetLineCount();
				ChatWindowControl.LineScroll(number_line);
			}
			if (sb.typemessage == m_TypeMessage::tmDisconnect)
			{
				CString strChat;
				ChatWindowControl.GetWindowText(strChat);
				strChat += L"\n    " + CString(sb.name) + L": " + L"Чат остановлен!" + L"\r\n";
				ChatWindowControl.SetWindowText(strChat);
				int number_line = ChatWindowControl.GetLineCount();
				ChatWindowControl.LineScroll(number_line);
			}
		}

	}
	else if (ServerCheck == false)
	{
		int send = MainSocket.Send(&sb, sizeof(SENDBUFFER));
		if (send == sizeof(SENDBUFFER))
			SendWindowControl.SetWindowText(L"");
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
	short count = 0;
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
	int countpeople = 1/*сервер*/ + (int)VecSockets.size()/*клиенты*/;
	CString temp;
	temp.Format(L"%d", countpeople);
	CountPeopleControl.SetWindowText(L"В чате " + temp + L" человек");

	// Отправим сообщение о количествах людей в чате.
	SENDBUFFER sb;
	sb.typemessage = m_TypeMessage::tmCountPeople;
	sb.countpeople = countpeople;

	for (int i = 0; i < (int)VecSockets.size(); i++)
	{
		// Всем клиентам рассылаем количество людей в чате.
		VecSockets[i]->Send(&sb, sizeof(SENDBUFFER));
	}
}

void CMFCChatDlg::SendDisconnect(void)
{
	SENDBUFFER sb;
	CString s;
	NiknameControl.GetWindowText(s);
	int len = s.GetLength();
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
		for (int index = 0; index < (int)VecSockets.size(); index++)
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

				// Отсылка принятого севером сообщения в сеть другим клиентам, 
				// как зеркало, клиент может работать с другими клиентами в 
				// сети только через сервер.
				SendBuffer(sb, false);
				 
				break;
			}
			// Рассылаем сообщения клиента по сети чата.
			if (sb.typemessage == m_TypeMessage::tmChat)
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
		int number_line = ChatWindowControl.GetLineCount();
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
		int number_line = ChatWindowControl.GetLineCount();
		ChatWindowControl.LineScroll(number_line);
	}
	break;
	case m_TypeMessage::tmFile:
	{
		SENDFILE sf(sb.countpeople);
		MainSocket.Receive(&sf, sizeof(SENDBUFFER));
		std::ofstream file(sf.filename, std::ios::binary);
		MainSocket.Receive(&sf, sizeof(SENDBUFFER));
		file.write(sf.buffer, sb.countpeople);//пишем в файл, сколько прочитали
	    file.close();
		break;
	}
	default:
		AfxMessageBox(L"Неизвестное сообщение!");
		break;
	}
}

