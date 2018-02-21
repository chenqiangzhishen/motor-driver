#include <windows.h>
#include <string>
#include "stdafx.h"
#include "serial.h"
#include "motor.h"
#include "tchar.h"
using namespace std;

DWORD WINAPI CommLRThread(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("-----------LR-------------begin\n");
	U8  direction = 1;
	CSerialPort *serial = (CSerialPort *)lpParam;
	while (1) {
		LeftRightMdMove(serial, 100, direction);
		//Sleep(100);
		direction = (direction + 1) % 2;

		Sleep(250);
		if (serial->m_lightpath_swith) {
			MotorWrite(serial, 0x00, 0x00);
			printf("in switch \n");
			serial->m_lightpath_swith = false;
			//Sleep(1000);
		}
		LeftRightMdStop(serial);      //马达先停
	}

	printf("-----------LR-------------end\n");

}
DWORD WINAPI CommFBThread(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("-----------FB-------------begin\n");
	U8  direction = 1;
	CSerialPort *serial = (CSerialPort *)lpParam;
	while (1) {
		FrontBackMdMove(serial, 100, direction);
		direction = (direction + 1) % 2;

		Sleep(350);
		if (serial->m_lightpath_swith) {
			MotorWrite(serial, 0x00, 0x00);
			printf("in switch \n");
			serial->m_lightpath_swith = false;
			//Sleep(1000);
		}
		FrontBackMdStop(serial);
	}
	printf("-----------FB-------------end\n");
}
int main(int argc, _TCHAR* argv[])
{
	CSerialPort *serial = new CSerialPort();
	int size = 0;
	int ret = 1;
	int rx_value = 0;
	int walking_step = 100;
	int delay_time = 5000;
	U8  direction = 1;

	printf("initialization------------------------\n");
	//serial.OpenSerialPort(_T("COM1:"), 115200, 8);  //打开串口后，自动接收数据
	serial->InitPort();
	serial->StartMonitoring();
	//要进行两次复位，第一次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);
	//第二次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);

	//LightPathMdStop();      //马达先停
	LeftRightMdStop(serial);      //马达先停
	FrontBackMdStop(serial);
	printf("initialization finished------------------------\n");

	HANDLE m_hThreadLR = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommLRThread, (LPVOID)serial, 0, NULL);
	HANDLE m_hThreadFB = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommFBThread, (LPVOID)serial, 0, NULL);
	while (1);
	return 0;
}
