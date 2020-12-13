
// MFCChat.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CMFCChatApp:
// Сведения о реализации этого класса: MFCChat.cpp
//

class CMFCChatApp : public CWinApp
{
public:
	CMFCChatApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CMFCChatApp theApp;
