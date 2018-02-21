#pragma once

#include <windows.h>
#include <string>
#include <process.h>
typedef unsigned(__stdcall *PTHREEA_START) (void *);

class CSerialPort
{
public:
	CSerialPort(void);
	~CSerialPort(void);

	// port initialisation
	BOOL   InitPort(UINT portnr = 1, UINT baud = 115200, char parity = 'N', UINT databits = 8, UINT stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR | EV_TXEMPTY, UINT nBufferSize = 512);

	//·¢ËÍÊý¾Ý
	BOOL SendData(char* data, int len);
	unsigned char ReceiveData();
	void        ProcessErrorMessage(char* ErrorText);

	// start/stop comm watching
	BOOL        StartMonitoring();
	BOOL        RestartMonitoring();
	BOOL        StopMonitoring();
	static void ReceiveChar(CSerialPort* port, COMSTAT comstat);
	static void WriteChar(CSerialPort* port);
	void        WriteToPort(char* string);
	DWORD       GetWriteBufferSize();
	DWORD       GetCommEvents();
	DCB         GetDCB();

public:

	char m_rxdata[50];
	bool m_lightpath_swith;
	bool m_ready;

	// synchronisation objects
	CRITICAL_SECTION    m_csCommunicationSync;
	BOOL                m_bThreadAlive;

	// handles
	HANDLE              m_hThread;
	HANDLE              m_hComm;
	HANDLE              m_hShutdownEvent;
	HANDLE              m_hWriteEvent;

	// Event array.
	HANDLE              m_hEventArray[3];

	// structures
	OVERLAPPED          m_ov;
	COMMTIMEOUTS        m_CommTimeouts;
	DCB                 m_dcb;

	// misc
	UINT                m_nPortNr;
	char*               m_szWriteBuffer;
	DWORD               m_dwCommEvents;
	DWORD               m_nWriteBufferSize;
};
