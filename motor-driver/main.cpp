#pragma once
#include <windows.h>
#include <string>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <future>
#include "stdafx.h"
#include "motor.h"
#include "Printer.h"
#include "video.h"
#include "control.h"
#include "selective-search.h"

typedef unsigned(__stdcall *PTHREEA_START) (void *);
using namespace std;

//光电开关开启表示没有挡住，关闭说明被挡住了
U8 g_light_path_sign_on = 1;
extern int faceDetect(int argc, const char** argv);

DWORD WINAPI CommFBThread(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("-----------FB-------------begin\n");
	U8  direction = 1;
	CnComm *serial = (CnComm *)lpParam;
	while (g_light_path_sign_on) {
		FrontBackMdMove(serial, 100, direction);
		direction = (direction + 1) % 2;

		Sleep(150);

		FrontBackMdStop(serial);
	}
	printf("-----------FB-------------end\n");
	return 0;
}

DWORD WINAPI CommLRThread(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("-----------LR-------------begin\n");
	U8  direction = 1;
	CnComm *serial = (CnComm *)lpParam;
	while (g_light_path_sign_on) {
		LeftRightMdMove(serial, 100, direction);
		direction = (direction + 1) % 2;
		//Sleep(250);
		LeftRightMdStop(serial);      //马达先停
	}

	printf("-----------LR-------------end\n");
	return 0;
}

DWORD WINAPI CommUDThread(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("-----------UD-------------begin\n");
	U8  direction = 1;
	CnComm *serial = (CnComm *)lpParam;
	while (g_light_path_sign_on) {
		UpDownMdMove(serial, 100, direction);
		direction = (direction + 1) % 2;
		Sleep(350);
		UpDownMdStop(serial);
	}
	printf("-----------UD-------------end\n");
	return 0;
}

DWORD WINAPI LightPathMdThread(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("-----------UD-------------begin\n");
	U8  direction = 1;
	CnComm *serial = (CnComm *)lpParam;
	while (g_light_path_sign_on) {
		LightPathMdMove(serial, 100, direction);
		direction = (direction + 1) % 2;
		Sleep(350);
		LightPathMdStop(serial);
	}
	printf("-----------UD-------------end\n");
	return 0;
}

DWORD WINAPI CommMotorParallelMoveThread(LPVOID lpParam) {
	// test 3. motor run in parallel mode
	printf("-----------parallel mode-------------begin\n");
	U8  direction = 1;
	CnComm *serial = (CnComm *)lpParam;
	while (g_light_path_sign_on) {
		//MotorParallelMove(serial, 20, 1, 0, 0, 0, 1);
		MotorParallelMove(serial, 100, direction, 100, direction, 100, direction, 100, direction);
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

DWORD WINAPI WriteCamera0Thread(LPVOID lpParam) {
	// test 3. motor run in parallel mode
	printf("-----------write camera 0 mode-------------begin\n");
	if (WriteVideo())
		return -1;
	else
		return 0;
	printf("-----------camera 0 mode-------------end\n");
}

DWORD WINAPI WriteCamera1Thread(LPVOID lpParam) {
	// test 3. motor run in parallel mode
	printf("-----------write camera 1 mode-------------begin\n");
	if (WriteVideo1())
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
		serial->ReadString(read_buf, sizeof read_buf-1);
		Sleep(150);
		//MotorRead(serial, 0x03);
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
	MotorReadWriteTest1(serial);
	printf("-----------test1-------------end\n");
	return 0;
}


DWORD WINAPI MotorReadWriteTestThread2(LPVOID lpParam) {
	// test 3. motor run left & right
	printf("----------test2--------------begin\n");
	CnComm *serial = (CnComm *)lpParam;
	MotorReadWriteTest2(serial);
	printf("-----------test2-------------end\n");
	return 0;
}


int main(int argc, const char* argv[])
{
	int size = 0;
	int ret = 1;
	int rx_value = 0;
	int walking_step = 100;
	int delay_time = 5000;
	U8  direction = 1;

	//printer test
	//PrintTest();
	//SelectiveSearch(argc, argv);
	//DisplayImage();
	//OpenCamera();
	//WriteVideo();
	//WriteVideo1();
	//faceDetect(argc, argv);
	//while (1);
	//touchTest();

	CnComm *serial = new CnComm();

	serial->Open(1, 115200);

	//要进行两次复位，第一次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);
	//第二次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);

	LightPathMdStop(serial);      //马达先停
	FrontBackMdStop(serial);
	LeftRightMdStop(serial);      //马达先停
	UpDownMdStop(serial);      //马达先停

	//set motor power on
	MotorPowerOn(serial, 7);
	printf("-----start to LightPathMdInitPos()\n");
	//LightPathMdInitPos(serial);
	printf("-----end to LightPathMdInitPos()\n");
	//

	//set power 12v & 5v on
	//Power12V(serial, 4);

	//set power 5v on
	//Power5V(serial, 2);

	HANDLE m_hCamera0 = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)Camera0Thread, NULL, 0, NULL);
	//HANDLE m_hCamera1 = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)Camera1Thread, NULL, 0, NULL);
	//HANDLE m_hWriteCamera0 = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)WriteCamera0Thread, NULL, 0, NULL);
	//HANDLE m_hWriteCamera1 = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)WriteCamera1Thread, NULL, 0, NULL);
	//HANDLE m_hCheckSwitchSign = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CheckSwitchSign, (LPVOID)serial, 0, NULL);

	// if want run motor in parallel, enable this thread.
	//HANDLE m_hThreadMotorParallelMove = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommMotorParallelMoveThread, (LPVOID)serial, 0, NULL);

	//LEFT&RIGHT
	//HANDLE m_hThreadLR = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommLRThread, (LPVOID)serial, 0, NULL);

	//FRONT&BACK
	//HANDLE m_hThreadFB = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommFBThread, (LPVOID)serial, 0, NULL);

	//UP&DOWN
	//HANDLE m_hThreadUD = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)CommUDThread, (LPVOID)serial, 0, NULL);

	//lightpath
	//HANDLE m_hThreadLightPath = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)LightPathMdThread, (LPVOID)serial, 0, NULL);

	//HANDLE m_hThreadTest1 = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)MotorReadWriteTestThread1, (LPVOID)serial, 0, NULL);
	//HANDLE m_hThreadTest2 = (HANDLE)_beginthreadex(NULL, 0, (PTHREEA_START)MotorReadWriteTestThread2, (LPVOID)serial, 0, NULL);
	//LightPathMdInitPos(serial);
	while (1);
	return 0;
}
