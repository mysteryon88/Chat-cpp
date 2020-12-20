#include "pch.h"
#include "CAboutDlg.h"

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CAboutDlg::OnNMClickSyslink1)
END_MESSAGE_MAP()


void CAboutDlg::OnNMClickSyslink1(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	ShellExecute(NULL, L"open", L"https://github.com/mysteryon88", NULL, NULL, SW_SHOW);
}
