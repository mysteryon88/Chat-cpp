
// ChatTP.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CChatTPApp:
// Сведения о реализации этого класса: ChatTP.cpp
//

class CChatTPApp : public CWinApp
{
public:
	CChatTPApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CChatTPApp theApp;
