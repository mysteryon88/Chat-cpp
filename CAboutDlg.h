#pragma once
#include "framework.h"
#include "MFCChat.h"

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ������ ����������� ����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
};