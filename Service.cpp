#include "pch.h"
#include "Service.h"
#include "MFCChatDlg.h"

Service::Service()
	: m_pParent(NULL)
{
}

CString Service::FindIP()
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

void Service::Close()
{
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	if (pDlg->MessageBox(L"Закрыть программу?", L"Chat", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		pDlg->StopChat();
		pDlg->EndDialog(0);
	}
}

bool Service::Verify()
{
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	CString strName;
	pDlg->NiknameControl.GetWindowText(strName);
	if (strName == pDlg->NiknameVal || strName.IsEmpty())
	{
		pDlg->MessageBox(L"Введите свое имя для чата!", L"Chat", MB_OK | MB_ICONWARNING);
		pDlg->StopChat();
		return false;
	}
	return true;
}

CString Service::OpenFile()
{
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK) return  dlg.m_ofn.lpstrFile;
	return L"";
}


void Service::DisabledControl(bool server)
{
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	
	pDlg->IPControl.EnableWindow(FALSE);
	pDlg->PortControl.EnableWindow(FALSE);
	pDlg->ButtonSendMesControl.EnableWindow(FALSE);
	pDlg->ButtonSendFileControl.EnableWindow(FALSE);

	if (server) pDlg->ButtonClientControl.EnableWindow(FALSE);
	else pDlg->ButtonStartServerControl.EnableWindow(FALSE);

	pDlg->ButtonStopChatControl.EnableWindow(TRUE);
}

void Service::EnabledControl(void)
{
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	
	pDlg->IPControl.EnableWindow(TRUE);
	pDlg->PortControl.EnableWindow(TRUE);
	pDlg->ButtonClientControl.EnableWindow(TRUE);
	pDlg->ButtonStartServerControl.EnableWindow(TRUE);

	pDlg->ButtonStopChatControl.EnableWindow(FALSE);
	pDlg->ButtonSendFileControl.EnableWindow(FALSE);
	pDlg->ButtonSendMesControl.EnableWindow(FALSE);
}

CString Service::GetIP()
{
	CMFCChatDlg* pDlg = (CMFCChatDlg*)m_pParent;
	CString temp, IPAdressServer;
	BYTE p1, p2, p3, p4;
	pDlg->IPControl.GetAddress(p1, p2, p3, p4);
	temp.Format(L"%d.", p1);
	IPAdressServer = temp;
	temp.Format(L"%d.", p2);
	IPAdressServer += temp;
	temp.Format(L"%d.", p3);
	IPAdressServer += temp;
	temp.Format(L"%d", p4);
	IPAdressServer += temp;

	return IPAdressServer;
}

