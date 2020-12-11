
// ChatTPDlg.h: файл заголовка
//

#pragma once


// Диалоговое окно CChatTPDlg
class CChatTPDlg : public CDialogEx
{
// Создание
public:
	CChatTPDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATTP_DIALOG };
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
	afx_msg void OnBnClickedBSendFile();
	void OnOK();
	void OnCancel();
	// никнейм
	CString Nikname;
	// Порт
	CString Port;
	// IP адрес
	DWORD IPAddress;
	// Управление элементом ip
	CIPAddressCtrl IPAddControl;
};
