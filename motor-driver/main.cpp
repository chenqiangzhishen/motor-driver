#include "stdafx.h"
#include <windows.h>
#include "serial.h"
#include "motor.h"
#include "tchar.h"
#include <string>
using namespace std;


int main(int argc, _TCHAR* argv[])
{
	CSerial serial;
	int size = 0;
	int ret = 1;
	int rx_value = 0;
	int walking_step = 100;
	int delay_time = 5000;
	U8  direction = 1;

	serial.OpenSerialPort(_T("COM3:"), 115200, 8);  //打开串口后，自动接收数据
	//要进行两次复位，第一次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);
	//第二次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);

	//LightPathMdStop();      //马达先停
	LeftRightMdStop(serial);      //马达先停
    //FrontBackMdStop();
    //UpDownMdStop();


	/*
	printf("begin------------------------\n");
	while (1) {

		MotorWriteTest(serial);
		serial.ReceiveData();
		Sleep(1000);
		printf("------------------------end\n");
	}
	*/
/*
	while (1) {
		LeftRightMdMove(serial, 100, direction);
		//printf("%s, number is: %d \n", serial.m_rxdata, serial.ReceiveData());
		Sleep(100);
		LeftRightMdStop(serial);
		direction = (direction + 1) % 2;
		Sleep(1000);
		printf("------------------------end\n");
	}
	*/
	while (1) {
		//LeftRightMdMove(serial, 100, 0);

		MotorWriteTest(serial);
		serial.ReceiveData();
		Sleep(1000);
		printf("------------------------end\n");
	}


	return 0;
}
