#pragma once
#include <windows.h>
#include <string>
#include "stdafx.h"
#include "motor.h"
#include "Printer.h"
#include "video.h"
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <future>
typedef unsigned(__stdcall *PTHREEA_START) (void *);
using namespace std;

//光电开关开启表示没有挡住，关闭说明被挡住了
U8 g_light_path_sign_on = 1;

DWORD WINAPI CommLRThread(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("-----------LR-------------begin\n");
	U8  direction = 1;
	CnComm *serial = (CnComm *)lpParam;
	while (1) {
		LeftRightMdMove(serial, 100, direction);
		//Sleep(100);
		direction = (direction + 1) % 2;

		Sleep(250);

		LeftRightMdStop(serial);      //马达先停
	}

	printf("-----------LR-------------end\n");

}
DWORD WINAPI CommFBThread(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("-----------FB-------------begin\n");
	U8  direction = 1;
	CnComm *serial = (CnComm *)lpParam;
	while (1) {
		FrontBackMdMove(serial, 100, direction);
		direction = (direction + 1) % 2;

		Sleep(350);

		FrontBackMdStop(serial);
	}
	printf("-----------FB-------------end\n");
}

DWORD WINAPI CommMotorParallelMoveThread(LPVOID lpParam) {
	// test 3. motor run in parallel mode
	printf("-----------parallel mode-------------begin\n");
	U8  direction = 1;
	char read_buf[30];
	CnComm *serial = (CnComm *)lpParam;
	while (g_light_path_sign_on) {
		srand(time(NULL));
		//MotorParallelMove(serial, rand() % 200, rand() % 2, rand() % 50, rand() % 2, rand() % 100, rand() % 2);
		MotorParallelMove(serial, 200, direction, 50, direction, 100, direction);
		direction = (direction + 1) % 2;
		Sleep(150);
		MotorParallelStop(serial);
	}
	printf("-----------parallel mode-------------end\n");
	return 0;
}

DWORD WINAPI Camera0Thread(LPVOID lpParam) {
	// test 3. motor run in parallel mode
	printf("-----------camera 0 mode-------------begin\n");
	if (OpenCamera0())
		return -1;
	else
		return 0;
	printf("-----------camera 0 mode-------------end\n");
}

DWORD WINAPI Camera1Thread(LPVOID lpParam) {
	// test 3. motor run in parallel mode
	printf("-----------camera 1 mode-------------begin\n");
	if (OpenCamera1())
		return -1;
	else
		return 0;
	printf("-----------camera 1 mode-------------end\n");
}

DWORD WINAPI CheckSwitchSign(LPVOID lpParam) {
	// test 3. motor run in parallel mode
	printf("-----------parallel mode-------------begin\n");
	U8  direction = 1;
	char read_buf[30];
	CnComm *serial = (CnComm *)lpParam;
	while (g_light_path_sign_on) {
		//serial->Read(str, sizeof str);
		serial->ReadString(read_buf, sizeof read_buf-1);
		Sleep(150);
		printf("******read string=%s, sizeof=%d \n", read_buf, sizeof read_buf);
		if (strstr(read_buf, "<event #1>")) {
			printf("==\n");
			g_light_path_sign_on = 0;
			MotorWrite(serial, 0x00, 0x00);
			MotorParallelStop(serial);
			break;
		}
		else {
			printf("!=\n");
		}
	}
	printf("-----------parallel mode-------------end\n");
	return 0;
}
DWORD WINAPI MotorReadWriteTestThread1(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("----------test1--------------begin\n");

	CnComm *serial = (CnComm *)lpParam;
	while (1) {
		MotorReadWriteTest(serial,1);
		Sleep(350);
	}
	printf("-----------test1-------------end\n");
}


DWORD WINAPI MotorReadWriteTestThread2(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("----------test2--------------begin\n");

	CnComm *serial = (CnComm *)lpParam;
	while (1) {
		MotorReadWriteTest(serial,2);

		Sleep(350);
	}
	printf("-----------test2-------------end\n");
}

int main(int argc, _TCHAR* argv[])
{
	int size = 0;
	int ret = 1;
	int rx_value = 0;
	int walking_step = 100;
	int delay_time = 5000;
	U8  direction = 1;

	//printer test
	//PrintTest();

	//DisplayVideo();
	//OpenCamera();

	//while (1);

	CnComm *serial = new CnComm();

	serial->Open(1, 115200);
	//要进行两次复位，第一次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);
	//第二次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);

	//LightPathMdStop();      //马达先停
	LeftRightMdStop(serial);      //马达先停
	FrontBackMdStop(serial);
	printf("initialization finished------------------------\n");

	HANDLE m_hCamera0 = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)Camera0Thread, NULL, 0, NULL);
	HANDLE m_hCamera1 = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)Camera1Thread, NULL, 0, NULL);
	HANDLE m_hCheckSwitchSign = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CheckSwitchSign, (LPVOID)serial, 0, NULL);
	HANDLE m_hThreadMotorParallelMove = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommMotorParallelMoveThread, (LPVOID)serial, 0, NULL);
	//HANDLE m_hThreadLR = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommLRThread, (LPVOID)serial, 0, NULL);
	//HANDLE m_hThreadFB = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommFBThread, (LPVOID)serial, 0, NULL);
	//HANDLE m_hThreadTest1 = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)MotorReadWriteTestThread1, (LPVOID)serial, 0, NULL);
	//HANDLE m_hThreadTest2 = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)MotorReadWriteTestThread2, (LPVOID)serial, 0, NULL);

	while (1);
	return 0;
}
