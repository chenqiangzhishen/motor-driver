#pragma once

#include <windows.h>
#include <string>

class CSerial
{
public:
	CSerial(void);
	~CSerial(void);

	//�򿪴���
	BOOL OpenSerialPort(TCHAR* port, UINT baud_rate, BYTE date_bits, BYTE stop_bit = ONESTOPBIT, BYTE parity = NOPARITY);

	//��������
	BOOL SendData(char* data, int len);
	unsigned char ReceiveData();
public:
	HANDLE m_hComm;
	char m_rxdata[50];
};

#pragma once
