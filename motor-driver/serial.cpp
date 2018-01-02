#include "StdAfx.h"
#include "serial.h"
#include <process.h>

#define _CRT_SECURE_NO_WARNINGS

typedef unsigned(__stdcall *PTHREEA_START) (void *);

CSerial::CSerial(void)
{
	m_hComm = INVALID_HANDLE_VALUE;
}

CSerial::~CSerial(void)
{
	if (m_hComm != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hComm);
	}
}

/*********************************************************************************************
* 功能 ： 读串口线程回调函数
* 描述 ： 收到数据后，简单的显示出来
********************************************************************************************/
DWORD WINAPI CommProc(LPVOID lpParam) {

	CSerial* pSerial = (CSerial*)lpParam;

	//清空串口
	PurgeComm(pSerial->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);

	char buf[1];
	char handlebuf[30];
	char *find = NULL;
	DWORD dwRead;
	//====will delete following two vars.
	char str[30];
	int size = 0;
	int i = 0;
	//===
	DWORD read, written;
	DCB port;
	HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);

	while (pSerial->m_hComm != INVALID_HANDLE_VALUE) {
		BOOL bReadOK = ReadFile(pSerial->m_hComm, buf, sizeof buf, &dwRead, NULL);
		//printf("[in CommProc function]: COM1 buf=%s \n", buf);
		//if (bReadOK && (dwRead > 0)) {
		if (bReadOK && (dwRead > 0)) {
		//if (dwRead) {
			//buf[dwRead] = '\0';
			/*
			if (strstr(buf, "<event #1>") != NULL) {
				//strcpy_s(pSerial->m_rxdata, buf);

				printf("<event #1> happend \n");


				size = sprintf_s(str, "<send 0x%02x 0x%02x>", 0x80, 0x00);
				pSerial->SendData(str, size);
				//TODO: 根据返回的数据信息，分别处理
			}
			else
			*/
			//	strcpy_s(pSerial->m_rxdata, buf);
			//WriteFile(screen, buf, dwRead, &written, NULL);

			if (buf[0] == '<') {
				i = 0;
				pSerial->m_rxdata[i++] = '<';
			}
			else if (buf[0] == '>') {
				pSerial->m_rxdata[i++] = '>';
				pSerial->m_rxdata[i] = '\0';
			}
			else {
				pSerial->m_rxdata[i++] = buf[0];
			}
			/*
			if (strstr(pSerial->m_rxdata, "<event #1>") != NULL) {
				//strcpy_s(pSerial->m_rxdata, buf);

				printf("<event #1> happend \n");


				size = sprintf_s(str, "<send 0x%02x 0x%02x>", 0x80, 0x00);
				pSerial->SendData(str, size);
				//TODO: 根据返回的数据信息，分别处理
			}
			*/

		}
		/*
		if (strchr(buf,'<') && strchr(buf,'>')) {
				strcpy_s(pSerial->m_rxdata, buf);
		}
		else if (strchr(buf, '<')) {
			memset(handlebuf, 0, sizeof handlebuf);
			strcpy_s(handlebuf, buf);
		}
		else if (strchr(buf, '>')) {
			strcat_s(handlebuf, buf);
			strcpy_s(pSerial->m_rxdata, handlebuf);
		}
		else {
			strcat_s(handlebuf, buf);
		}

		if (strstr(handlebuf, "<event #1>") != NULL) {
			//TODO: 处理各种开关事件
			//1、清零地址0x00
			//2、读取各开关的值，查看是哪个开关事件
			printf("has <enent #1>\n");
		}
		printf("handlebuf=%s\n", handlebuf);
		printf("[in CommProc function]: COM1 pSerial->m_rxdata=%s \n", pSerial->m_rxdata);
		*/
	}
	return 0;
}
/*
DWORD WINAPI CommProc(LPVOID lpParam) {

CSerial* pSerial = (CSerial*)lpParam;

//清空串口
PurgeComm(pSerial->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);

char buf[512];
DWORD dwRead;
while (pSerial->m_hComm != INVALID_HANDLE_VALUE) {
BOOL bReadOK  = ReadFile(pSerial->m_hComm, buf, 512, &dwRead, NULL);
if (bReadOK  && (dwRead > 0)) {
buf[dwRead] = '\0';
MessageBoxA(NULL, buf, "串口收到数据", MB_OK);
}
}
return 0;
}
*/

/*******************************************************************************************
* 功能     :   打开串口
* port     :   串口号, 如_T("COM1:")
* baud_rate:   波特率
* date_bits:   数据位（有效范围4~8）
* stop_bit :   停止位
* parity   :   奇偶校验。默认为无校验。NOPARITY 0； ODDPARITY 1；EVENPARITY 2；MARKPARITY 3；SPACEPARITY 4
********************************************************************************************/
BOOL CSerial::OpenSerialPort(TCHAR* port, UINT baud_rate, BYTE date_bits, BYTE stop_bit, BYTE parity)
{
	//打开串口
	m_hComm = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);//独占方式打开串口

	TCHAR err[512];

	if (m_hComm == INVALID_HANDLE_VALUE) {
		_stprintf_s(err, _T("打开串口%s 失败，请查看该串口是否已被占用"), port);
		MessageBox(NULL, err, _T("提示"), MB_OK);
		return FALSE;
	}

	//MessageBox(NULL,_T("打开成功"),_T("提示"),MB_OK);

	//获取串口默认配置
	DCB dcb;
	if (!GetCommState(m_hComm, &dcb)) {
		MessageBox(NULL, _T("获取串口当前属性参数失败"), _T("提示"), MB_OK);
	}

	//配置串口参数
	dcb.BaudRate = baud_rate;  //波特率
	dcb.fBinary = TRUE;            //二进制模式。必须为TRUE
	dcb.ByteSize = date_bits;  //数据位。范围4-8
	dcb.StopBits = stop_bit; //停止位

	if (parity == NOPARITY) {
		dcb.fParity = FALSE;   //奇偶校验。无奇偶校验
		dcb.Parity = parity;   //校验模式。无奇偶校验
	}
	else {
		dcb.fParity = TRUE;        //奇偶校验
		dcb.Parity = parity;   //校验模式。无奇偶校验
	}

	dcb.fOutxCtsFlow = FALSE;  //CTS线上的硬件握手
	dcb.fOutxDsrFlow = FALSE;  //DST线上的硬件握手
	dcb.fDtrControl = DTR_CONTROL_ENABLE;//DTR控制
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = FALSE;         //是否使用XON/XOFF协议
	dcb.fInX = FALSE;          //是否使用XON/XOFF协议
	dcb.fErrorChar = FALSE;        //是否使用发送错误协议
	dcb.fNull = FALSE;         //停用null stripping
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fAbortOnError = FALSE; //串口发送错误，并不终止串口读写

							   //设置串口参数
	if (!SetCommState(m_hComm, &dcb)) {
		MessageBox(NULL, _T("设置串口参数失败"), _T("提示"), MB_OK);
		return FALSE;
	}

	//设置串口事件
	SetCommMask(m_hComm, EV_RXCHAR);//在缓存中有字符时产生事件
	SetupComm(m_hComm, 16384, 16384);

	//设置串口读写时间
	COMMTIMEOUTS CommTimeOuts;
	GetCommTimeouts(m_hComm, &CommTimeOuts);
	CommTimeOuts.ReadIntervalTimeout = 1;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 1;
	CommTimeOuts.ReadTotalTimeoutConstant = 1;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 1;
	CommTimeOuts.WriteTotalTimeoutConstant = 1;

	if (!SetCommTimeouts(m_hComm, &CommTimeOuts)) {
		MessageBox(NULL, _T("设置串口时间失败"), _T("提示"), MB_OK);
		return FALSE;
	}

	//创建线程，读取数据
	HANDLE hReadCommThread = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommProc, (LPVOID)this, 0, NULL);
	return TRUE;
}

/********************************************************************************************
* 功能    ：    通过串口发送一条数据
********************************************************************************************/
BOOL CSerial::SendData(char* data, int len) {
	if (m_hComm == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, _T("串口未打开"), _T("提示"), MB_OK);
		return FALSE;
	}

	//清空串口
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);

	//写串口
	DWORD dwWrite = 0;
	DWORD dwRet = WriteFile(m_hComm, data, len, &dwWrite, NULL);

	//清空串口
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);

	if (!dwRet) {
		MessageBox(NULL, _T("发送数据失败"), _T("提示"), MB_OK);
		return FALSE;
	}
	return TRUE;
}
// 接收数据
unsigned char CSerial::ReceiveData() {

	int rx_value = 0;
	unsigned char ret;

	std::string rxdata = m_rxdata;

	printf("COM1 serial.m_rxdata=%s \n", m_rxdata);
	std::size_t found_begin = rxdata.find('<');
	std::size_t found = rxdata.find_last_of('>');
	/*
	if (found_begin != std::string::npos && found != std::string::npos) {
		if (found - 4 >= 0) {
			ret = (unsigned char)std::stoul(rxdata.substr(found - 4, 4).c_str(), nullptr, 16);
			printf("COM1 (unsigned char)serial.m_rxdata=%d \n", ret);
			return ret;
		}
	}
	*/

	return 0;
}
