#pragma once
#include <afxsock.h>

class Service 
{
public:
	Service();
	CString FindIP();
	virtual ~Service() {}
	void Close();
	bool Verify();
	CString OpenFile();
	void DisabledControl(bool server);
	void EnabledControl(void);
	CString GetIP();
	CWnd* m_pParent;
};