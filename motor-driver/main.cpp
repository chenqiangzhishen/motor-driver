#include <windows.h>
#include <string>
#include "stdafx.h"
#include "serial.h"
#include "motor.h"
#include "tchar.h"
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

	printf("initialization------------------------\n");
	serial.OpenSerialPort(_T("COM1:"), 115200, 8);  //打开串口后，自动接收数据
	//要进行两次复位，第一次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);
	//第二次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);

	//LightPathMdStop();      //马达先停
	LeftRightMdStop(serial);      //马达先停
	printf("initialization finished------------------------\n");
	//FrontBackMdStop();
	//UpDownMdStop();

	while (1) {
		printf("------------------------begin\n");

		// test 1. using <version>
		// MotorWriteTest(serial);

		// test 2. write & read.
		//MotorWrite(serial, 0x00, 0xaa);
		//MotorRead(serial, 0x00);

		// test 3. motor run left & right
		LeftRightMdMove(serial, 100, direction);
		direction = (direction + 1) % 2;

		Sleep(500);

		printf("------------------------end\n");
	}

	return 0;
}
