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
	char str[30];
	int size = 0;
	int ret = 1;
	int rx_value = 0;
	char buffer[20];

	//serial.OpenSerialPort(_T("COM1:"), 115200, 8, 1.5);  //打开串口后，自动接收数据  
	//serial.OpenSerialPort(_T("COM2:"), 115200, 8, 1.5);  //打开串口后，自动接收数据  
	serial.OpenSerialPort(_T("COM3:"), 115200, 8, 1.5);  //打开串口后，自动接收数据  


														 /*
														 while (ret != IDNO) {
														 //MotorWrite(serial, 0x00, 0x00);
														 //rx_value=MotorRead(serial, 0x00);
														 //rx_value = serial.ReceiveData();
														 //_itoa_s(rx_value, buffer, 20, 10);
														 //MessageBoxA(NULL, buffer, "串口收到数据", MB_OK);
														 MotorWriteTest(serial);

														 Sleep(1000); //1s

														 printf("%s, number is: %d \n", serial.m_rxdata, serial.ReceiveData());
														 //ret = MessageBox(NULL, _T(""), _T("是否向串口发送数据"), MB_YESNO); //YES继续发送一条数据，NO不发送，退出
														 }
														 */



	int walking_step = 100;
	int delay_time = 5000;
	U8  direction = 1;
	//要进行两次复位，第一次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);
	//第二次上电初始化马达的全局控制寄存器
	MotorWrite(serial, 0x00, 0x00);

	//LightPathMdStop();      //马达先停
	LeftRightMdStop(serial);      //马达先停
								  //FrontBackMdStop();
								  //UpDownMdStop();



	while (1) {
		LeftRightMdMove(serial, 100, direction);
		//printf("%s, number is: %d \n", serial.m_rxdata, serial.ReceiveData());
		Sleep(1);
		LeftRightMdStop(serial);
		direction = (direction + 1) % 2;
		Sleep(1000);
		printf("------------------------end\n");
	}

	return 0;
}

