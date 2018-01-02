#include "StdAfx.h"
#include "serial.h"
#include <process.h>

#define _CRT_SECURE_NO_WARNINGS

typedef unsigned(__stdcall *PTHREEA_START) (void *);

CSerial::CSerial(void)
{
	m_hComm = INVALID_HANDLE_VALUE;
	memset(m_rxdata, 0, sizeof m_rxdata);
}

CSerial::~CSerial(void)
{
	if (m_hComm != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hComm);
	}
}

/*********************************************************************************************
* ���� �� �������̻߳ص�����
* ���� �� �յ����ݺ󣬼򵥵���ʾ����
********************************************************************************************/
DWORD WINAPI CommProc(LPVOID lpParam) {

	CSerial* pSerial = (CSerial*)lpParam;

	//��մ���
	PurgeComm(pSerial->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);

	char buf[50];
	char handlebuf[50];
	DWORD dwRead;

	while (pSerial->m_hComm != INVALID_HANDLE_VALUE) {
		BOOL bReadOK = ReadFile(pSerial->m_hComm, buf, sizeof buf, &dwRead, NULL);
		if (bReadOK && (dwRead > 0)) {
			buf[dwRead] = '\0';
			//strcpy_s(pSerial->m_rxdata, buf);
			//printf(">>>>>>>>pSerial->m_rxdata=%s\n", buf);
			if (strchr(buf, '<') && strchr(buf, '>')) {
				memset(pSerial->m_rxdata, 0, sizeof pSerial->m_rxdata);
				strcpy_s(pSerial->m_rxdata, buf);
				pSerial->m_ready = true;
				//printf("111+++++++++++pSerial->m_rxdata=%s\n", pSerial->m_rxdata);
			}
			else if (strchr(buf, '<')) {
				memset(handlebuf, 0, sizeof handlebuf);
				strcpy_s(handlebuf, buf);
			}
			else if (strchr(buf, '>')) {
				strcat_s(handlebuf, buf);
				strcpy_s(pSerial->m_rxdata, handlebuf);
				pSerial->m_ready = true;
				//printf("222+++++++++++pSerial->m_rxdata=%s\n", pSerial->m_rxdata);
			}
			else {
				strcat_s(handlebuf, buf);
			}

			if (pSerial->m_ready) {
				pSerial->ReceiveData();
				pSerial->m_ready = false;
			}
		}
	}
	return 0;
}
/*
DWORD WINAPI CommProc(LPVOID lpParam) {

CSerial* pSerial = (CSerial*)lpParam;

//��մ���
PurgeComm(pSerial->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);

char buf[512];
DWORD dwRead;
while (pSerial->m_hComm != INVALID_HANDLE_VALUE) {
BOOL bReadOK  = ReadFile(pSerial->m_hComm, buf, 512, &dwRead, NULL);
if (bReadOK  && (dwRead > 0)) {
buf[dwRead] = '\0';
MessageBoxA(NULL, buf, "�����յ�����", MB_OK);
}
}
return 0;
}
*/

/*******************************************************************************************
* ����     :   �򿪴���
* port     :   ���ں�, ��_T("COM1:")
* baud_rate:   ������
* date_bits:   ����λ����Ч��Χ4~8��
* stop_bit :   ֹͣλ
* parity   :   ��żУ�顣Ĭ��Ϊ��У�顣NOPARITY 0�� ODDPARITY 1��EVENPARITY 2��MARKPARITY 3��SPACEPARITY 4
********************************************************************************************/
BOOL CSerial::OpenSerialPort(TCHAR* port, UINT baud_rate, BYTE date_bits, BYTE stop_bit, BYTE parity)
{
	//�򿪴���
	m_hComm = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//��ռ��ʽ�򿪴���

	TCHAR err[512];

	if (m_hComm == INVALID_HANDLE_VALUE) {
		_stprintf_s(err, _T("�򿪴���%s ʧ�ܣ���鿴�ô����Ƿ��ѱ�ռ��"), port);
		MessageBox(NULL, err, _T("��ʾ"), MB_OK);
		return FALSE;
	}

	//MessageBox(NULL,_T("�򿪳ɹ�"),_T("��ʾ"),MB_OK);

	//��ȡ����Ĭ������
	DCB dcb;
	if (!GetCommState(m_hComm, &dcb)) {
		MessageBox(NULL, _T("��ȡ���ڵ�ǰ���Բ���ʧ��"), _T("��ʾ"), MB_OK);
	}

	//���ô��ڲ���
	dcb.BaudRate = baud_rate;  //������
	dcb.fBinary = TRUE;            //������ģʽ������ΪTRUE
	dcb.ByteSize = date_bits;  //����λ����Χ4-8
	dcb.StopBits = stop_bit; //ֹͣλ

	if (parity == NOPARITY) {
		dcb.fParity = FALSE;   //��żУ�顣����żУ��
		dcb.Parity = parity;   //У��ģʽ������żУ��
	}
	else {
		dcb.fParity = TRUE;        //��żУ��
		dcb.Parity = parity;   //У��ģʽ������żУ��
	}

	dcb.fOutxCtsFlow = FALSE;  //CTS���ϵ�Ӳ������
	dcb.fOutxDsrFlow = FALSE;  //DST���ϵ�Ӳ������
	dcb.fDtrControl = DTR_CONTROL_ENABLE;//DTR����
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = FALSE;         //�Ƿ�ʹ��XON/XOFFЭ��
	dcb.fInX = FALSE;          //�Ƿ�ʹ��XON/XOFFЭ��
	dcb.fErrorChar = FALSE;        //�Ƿ�ʹ�÷��ʹ���Э��
	dcb.fNull = FALSE;         //ͣ��null stripping
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fAbortOnError = FALSE; //���ڷ��ʹ��󣬲�����ֹ���ڶ�д

							   //���ô��ڲ���
	if (!SetCommState(m_hComm, &dcb)) {
		MessageBox(NULL, _T("���ô��ڲ���ʧ��"), _T("��ʾ"), MB_OK);
		return FALSE;
	}

	//���ô����¼�
	SetCommMask(m_hComm, EV_RXCHAR);//�ڻ��������ַ�ʱ�����¼�
	SetupComm(m_hComm, 16384, 16384);

	//���ô��ڶ�дʱ��
	COMMTIMEOUTS CommTimeOuts;
	GetCommTimeouts(m_hComm, &CommTimeOuts);
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 1;
	CommTimeOuts.WriteTotalTimeoutConstant = 1;

	if (!SetCommTimeouts(m_hComm, &CommTimeOuts)) {
		MessageBox(NULL, _T("���ô���ʱ��ʧ��"), _T("��ʾ"), MB_OK);
		return FALSE;
	}

	//�����̣߳���ȡ����
	HANDLE hReadCommThread = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommProc, (LPVOID)this, 0, NULL);
	return TRUE;
}

/********************************************************************************************
* ����    ��    ͨ�����ڷ���һ������
********************************************************************************************/
BOOL CSerial::SendData(char* data, int len) {
	if (m_hComm == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, _T("����δ��"), _T("��ʾ"), MB_OK);
		return FALSE;
	}

	//��մ���
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);

	//д����
	DWORD dwWrite = 0;
	DWORD dwRet = WriteFile(m_hComm, data, len, &dwWrite, NULL);

	//��մ���
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);

	if (!dwRet) {
		MessageBox(NULL, _T("��������ʧ��"), _T("��ʾ"), MB_OK);
		return FALSE;
	}
	return TRUE;
}
// ��������
unsigned char CSerial::ReceiveData() {

	int rx_value = 0;
	unsigned char ret;
	char str[30];
	int size = 0;

	std::string rxdata = m_rxdata;

	printf("COM1 serial.m_rxdata=%s \n", m_rxdata);
	if (strstr(m_rxdata, "<EXOR RTOS V1.0>") != NULL) {
		//nothing to do.
	}
	else if (strstr(m_rxdata, "<event #1>") != NULL) {
		//TODO: ������ֿ����¼�
		//1�������ַ0x00
		//2����ȡ�����ص�ֵ���鿴���ĸ������¼�
		size = sprintf_s(str, "<send 0x%02x 0x%02x>", 0x80, 0x00);
		SendData(str, size);
	}
	else {
		std::size_t found_begin = rxdata.find('<');
		std::size_t found = rxdata.find_last_of('>');
		if (found_begin != std::string::npos && found != std::string::npos) {
			if (found - 4 >= 0) {
				ret = (unsigned char)std::stoul(rxdata.substr(found - 4, 4).c_str(), nullptr, 16);
				printf("COM1 (unsigned char)serial.m_rxdata=%d \n", ret);
				return ret;
			}
		}
    }

	return 0;
}
