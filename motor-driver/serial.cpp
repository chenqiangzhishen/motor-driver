#include "StdAfx.h"
#include <assert.h>
#include "serial.h"

CSerialPort::CSerialPort(void)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_lightpath_swith = false;
	m_hComm = NULL;

	// initialize overlapped structure members to zero
	m_ov.Offset = 0;
	m_ov.OffsetHigh = 0;

	// create events
	m_ov.hEvent = NULL;
	m_hWriteEvent = NULL;
	m_hShutdownEvent = NULL;

	m_szWriteBuffer = NULL;

	m_bThreadAlive = FALSE;
}

CSerialPort::~CSerialPort(void)
{
	if (m_hComm != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hComm);
	}
	do
	{
		CloseHandle(m_hThread);
		SetEvent(m_hShutdownEvent);
	} while (m_bThreadAlive);

	printf("Thread ended\n");

	delete[] m_szWriteBuffer;
}

//
// Initialize the port. This can be port 1 to 4.
//
BOOL CSerialPort::InitPort(
	UINT  portnr,        // portnumber (1..4)
	UINT  baud,          // baudrate
	char  parity,        // parity
	UINT  databits,      // databits
	UINT  stopbits,      // stopbits
	DWORD dwCommEvents,  // EV_RXCHAR, EV_CTS etc
	UINT  writebuffersize)   // size to the writebuffer
{
	// if the thread is alive: Kill
	if (m_bThreadAlive)
	{
		do
		{
			SetEvent(m_hShutdownEvent);
		} while (m_bThreadAlive);
		printf("Thread ended\n");
	}

	// create events
	if (m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// initialize the event objects
	m_hEventArray[0] = m_hShutdownEvent;    // highest priority
	m_hEventArray[1] = m_ov.hEvent;
	m_hEventArray[2] = m_hWriteEvent;

	// initialize critical section
	InitializeCriticalSection(&m_csCommunicationSync);
	if (m_szWriteBuffer != NULL)
		delete[] m_szWriteBuffer;
	m_szWriteBuffer = new char[writebuffersize];

	m_nPortNr = portnr;

	m_nWriteBufferSize = writebuffersize;
	m_dwCommEvents = dwCommEvents | EV_TXEMPTY | EV_RXCHAR; // make sure tx empty set and rx char

	BOOL bResult = FALSE;
	char *szPort = new char[50];
	char *szBaud = new char[50];

	// now it critical!
	EnterCriticalSection(&m_csCommunicationSync);

	// if the port is already opened: close it
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}

	// prepare port strings
	sprintf(szPort, "COM%d", portnr);
	sprintf(szBaud, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopbits);

	// get a handle to the port
	m_hComm = CreateFile(szPort,       // communication port string (COMX)
		GENERIC_READ | GENERIC_WRITE,  // read/write types
		0,                             // comm devices must be opened with exclusive access
		NULL,                          // no security attributes
		OPEN_EXISTING,                 // comm devices must use OPEN_EXISTING
		FILE_FLAG_OVERLAPPED,          // Async I/O
		0);                            // template must be 0 for comm devices

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// port not found
		delete[] szPort;
		delete[] szBaud;

		return FALSE;
	}

	// set the timeout values
	m_CommTimeouts.ReadIntervalTimeout = 1000;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;

	// configure
	if (SetCommTimeouts(m_hComm, &m_CommTimeouts))
	{
		if (SetCommMask(m_hComm, dwCommEvents))
		{
			if (GetCommState(m_hComm, &m_dcb))
			{
				m_dcb.fRtsControl = RTS_CONTROL_ENABLE;     // set RTS bit high!
				if (BuildCommDCB(szBaud, &m_dcb))
				{
					if (SetCommState(m_hComm, &m_dcb))
						; // normal operation... continue
					else
						ProcessErrorMessage("SetCommState()");
				}
				else
					ProcessErrorMessage("BuildCommDCB()");
			}
			else
				ProcessErrorMessage("GetCommState()");
		}
		else
			ProcessErrorMessage("SetCommMask()");
	}
	else
		ProcessErrorMessage("SetCommTimeouts()");

	delete[] szPort;
	delete[] szBaud;

	// flush the port
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// release critical section
	LeaveCriticalSection(&m_csCommunicationSync);

	printf("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\n", portnr);

	return TRUE;
}

//
// Create a thread to monitor all kinds of events. And handle with different methods
//

DWORD WINAPI CommThread(LPVOID lpParam) {

	CSerialPort *port = (CSerialPort *)lpParam;

	port->m_bThreadAlive = TRUE;
	DWORD BytesTransfered = 0;
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
	COMSTAT comstat;
	BOOL  bResult = TRUE;

	// Clear comm buffers at startup
	if (port->m_hComm) // check if the port is opened
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	// begin forever loop.  This loop will run as long as the thread is alive.
	for (;;)
	{
		bResult = WaitCommEvent(port->m_hComm, &Event, &port->m_ov);
		if (!bResult)
		{
			// If WaitCommEvent() returns FALSE, process the last error to determin
			// the reason..
			switch (dwError = GetLastError())
			{
			case ERROR_IO_PENDING:
			{
				// This is a normal return value if there are no bytes
				// to read at the port.
				// Do nothing and continue
				break;
			}
			case 87:
			{
                //TODO
				// I have not investigated why, but it is also a valid reply
				// Also do nothing and continue.
				break;
			}
			default:
			{
				// All other error codes indicate a serious error has
				// occured.  Process this error.
				port->ProcessErrorMessage("WaitCommEvent()");
				break;
			}
			}
		}
		else
		{
			bResult = ClearCommError(port->m_hComm, &dwError, &comstat);
			if (comstat.cbInQue == 0)
				continue;
		} // end if bResult
		Event = WaitForMultipleObjects(3, port->m_hEventArray, FALSE, INFINITE);
		switch (Event)
		{
		case 0:
		{
			port->m_bThreadAlive = FALSE;
			// Kill this thread.  break is not needed, but makes me feel better.
			//TODO
			//AfxEndThread(100);
			break;
		}
		case 1: // read event
		{
			GetCommMask(port->m_hComm, &CommEvent);
			if (CommEvent & EV_RXCHAR)
				// Receive character event from port.
				port->ReceiveChar(port, comstat);
			break;
		}
		case 2: // write event
		{
			// Write character event from port
			port->WriteChar(port);
			break;
		}
		} // end switch
	} // close forever loop
	return 0;
}

//
// start comm watching
//
BOOL CSerialPort::StartMonitoring()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommThread, (LPVOID)this, 0, NULL);
	//TODO, if failed??
	printf("Thread started\n");
	return TRUE;
}

//
// Restart the comm thread
//
BOOL CSerialPort::RestartMonitoring()
{
	printf("Thread resumed\n");
	ResumeThread(m_hThread);
	return TRUE;
}

//
// Suspend the comm thread
//
BOOL CSerialPort::StopMonitoring()
{
	printf("Thread suspended\n");
	SuspendThread(m_hThread);
	return TRUE;
}

//
// Write a character.
//
void CSerialPort::WriteChar(CSerialPort* port)
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;

	DWORD BytesSent = 0;

	ResetEvent(port->m_hWriteEvent);

	// Gain ownership of the critical section
	EnterCriticalSection(&port->m_csCommunicationSync);

	if (bWrite)
	{
		// Initailize variables
		port->m_ov.Offset = 0;
		port->m_ov.OffsetHigh = 0;

		// Clear buffer
		PurgeComm(port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

		bResult = WriteFile(port->m_hComm,          // Handle to COMM Port
			port->m_szWriteBuffer,                  // Pointer to message buffer in calling finction
			strlen((char*)port->m_szWriteBuffer),   // Length of message to send
			&BytesSent,                             // Where to store the number of bytes sent
			&port->m_ov);                           // Overlapped structure
		// deal with any error codes
		if (!bResult)
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
			case ERROR_IO_PENDING:
			{
				// continue to GetOverlappedResults()
				BytesSent = 0;
				bWrite = FALSE;
				break;
			}
			default:
			{
				// all other error codes
				port->ProcessErrorMessage("WriteFile()");
			}
			}
		}
		else
		{
			printf("write success\n");
			LeaveCriticalSection(&port->m_csCommunicationSync);
		}
	} // end if(bWrite)

	if (!bWrite)
	{
		bWrite = TRUE;

		bResult = GetOverlappedResult(port->m_hComm,    // Handle to COMM port
			&port->m_ov,      // Overlapped structure
			&BytesSent,       // Stores number of bytes sent
			TRUE);            // Wait flag

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// deal with the error code
		if (!bResult)
		{
			port->ProcessErrorMessage("GetOverlappedResults() in WriteFile()");
		}
	} // end if (!bWrite)

	  // Verify that the data size send equals what we tried to send
	if (BytesSent != strlen((char*)port->m_szWriteBuffer))
	{
		printf("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", BytesSent, strlen((char*)port->m_szWriteBuffer));
	}
}

//
// Character received. Inform the owner
//
void CSerialPort::ReceiveChar(CSerialPort* port, COMSTAT comstat)
{
	BOOL  bRead = TRUE;
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	unsigned char RXBuff;

	for (;;)
	{
		// Gain ownership of the comm port critical section.
		// This process guarantees no other part of this program
		// is using the port object.

		EnterCriticalSection(&port->m_csCommunicationSync);

		// ClearCommError() will update the COMSTAT structure and
		// clear any other errors.

		bResult = ClearCommError(port->m_hComm, &dwError, &comstat);

		LeaveCriticalSection(&port->m_csCommunicationSync);
		if (comstat.cbInQue == 0)
		{
			// break out when all bytes have been read
			break;
		}

		EnterCriticalSection(&port->m_csCommunicationSync);

		if (bRead)
		{
			bResult = ReadFile(port->m_hComm,       // Handle to COMM port
				&RXBuff,             // RX Buffer Pointer
				1,                   // Read one byte
				&BytesRead,          // Stores number of bytes read
				&port->m_ov);        // pointer to the m_ov structure
									 // deal with the error code
			/*
			memset(port->m_rxdata, 0, sizeof port->m_rxdata);
			bResult = ReadFile(port->m_hComm,       // Handle to COMM port
				port->m_rxdata,             // RX Buffer Pointer
				sizeof port->m_rxdata,                   // Read one byte
				&BytesRead,          // Stores number of bytes read
				&port->m_ov);        // pointer to the m_ov structure
									 // deal with the error code
			*/
			if (!bResult)
			{
				switch (dwError = GetLastError())
				{
				case ERROR_IO_PENDING:
				{
					// asynchronous i/o is still in progress
					// Proceed on to GetOverlappedResults();
					bRead = FALSE;
					break;
				}
				default:
				{
					// Another error has occured.  Process this error.
					port->ProcessErrorMessage("ReadFile()");
					break;
				}
				}
			}
			else
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				/*
				if (BytesRead > 0) {
					port->m_rxdata[BytesRead] = '\0';
					//printf(">>>>>>>>port->m_rxbuf=%s\n", port->m_rxbuf);
				}
				*/
				bRead = TRUE;
			}
		}  // close if (bRead)

		if (!bRead)
		{
			bRead = TRUE;
			bResult = GetOverlappedResult(port->m_hComm,    // Handle to COMM port
				&port->m_ov,      // Overlapped structure
				&BytesRead,       // Stores number of bytes read
				TRUE);            // Wait flag

								  // deal with the error code
			if (!bResult)
			{
				port->ProcessErrorMessage("GetOverlappedResults() in ReadFile()");
			}
		}  // close if (!bRead)

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// notify parent that a byte was received
		//TODO
		//::SendMessage((port->m_pOwner)->m_hWnd, WM_COMM_RXCHAR, (WPARAM)RXBuff, (LPARAM)port->m_nPortNr);
	} // end forever loop

}

//
// Write a string to the port
//
void CSerialPort::WriteToPort(char* string)
{
	assert(m_hComm != 0);

	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
	strcpy(m_szWriteBuffer, string);

	// set event for write
	SetEvent(m_hWriteEvent);
}

//
// Return the device control block
//
DCB CSerialPort::GetDCB()
{
	return m_dcb;
}

//
// Return the communication event masks
//
DWORD CSerialPort::GetCommEvents()
{
	return m_dwCommEvents;
}

//
// Return the output buffer size
//
DWORD CSerialPort::GetWriteBufferSize()
{
	return m_nWriteBufferSize;
}


/*
DWORD WINAPI CommProc(LPVOID lpParam) {
	CSerial* pSerial = (CSerial*)lpParam;

	//清空串口
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
		}
	}
	return 0;
}
*/
//
// If there is a error, give the right message
//
void CSerialPort::ProcessErrorMessage(char* ErrorText)
{
	char *Temp = new char[200];

	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	sprintf(Temp, "WARNING:  %s Failed with the following error: \n%s\nPort: %d\n", (char*)ErrorText, lpMsgBuf, m_nPortNr);
	MessageBox(NULL, Temp, "Application Error", MB_ICONSTOP);

	LocalFree(lpMsgBuf);
	delete[] Temp;
}

/********************************************************************************************
* 功能    ：    通过串口发送一条数据
********************************************************************************************/
BOOL CSerialPort::SendData(char* data, int len) {
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
unsigned char CSerialPort::ReceiveData() {
	int rx_value = 0;
	unsigned char ret;
	char str[30];
	int size = 0;

	printf("COM1 serial.m_rxdata=%s \n", m_rxdata);
	if (strstr(m_rxdata, "<EXOR RTOS V1.0>") != NULL) {
		//nothing to do.
	}
	else if (strstr(m_rxdata, "<event #1>") != NULL) {
		//TODO: 处理各种开关事件
		//1、清零地址0x00
		//2、读取各开关的值，查看是哪个开关事件

		m_lightpath_swith = true;
		//size = sprintf_s(str, "<send 0x%02x 0x%02x>", 0x80, 0x00);
		//SendData(str, size);
	}
	else if (strchr(m_rxdata, ',')) {
		std::string rxdata = m_rxdata;
		std::size_t found_begin = rxdata.find('<');
		std::size_t found_end = rxdata.find_last_of('>');
		if (found_begin != std::string::npos && found_end != std::string::npos) {
			if (found_end - 4 >= 0) {
				ret = (unsigned char)std::stoul(rxdata.substr(found_end - 4, 4).c_str(), nullptr, 16);
				printf("COM1 (unsigned char)serial.m_rxdata=%d \n", ret);
				return ret;
			}
		}
	}

	return 0;
}
