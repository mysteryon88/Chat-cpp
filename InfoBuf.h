#pragma once
#include "framework.h"

//размер пакета для передачи файла
constexpr auto PACK = 4096;

struct SENDBUFFER
{
	SENDBUFFER()
	{
		stopchat = false;
		typemessage = 0;
		countpeople = 0;
		filebuffersize = 0;
		ZeroMemory(name, sizeof(TCHAR) * 14);
		ZeroMemory(filename, sizeof(TCHAR) * 33);
		ZeroMemory(buffer, sizeof(TCHAR) * 202);
		ZeroMemory(filebuffer, sizeof(char) * PACK);
	}
	

	~SENDBUFFER() {}
	bool stopchat;
	uint8_t typemessage;
	uint8_t countpeople;
	uint16_t filebuffersize;
	TCHAR name[14];
	TCHAR filename[33];
	TCHAR buffer[202];
	char filebuffer[PACK];
};