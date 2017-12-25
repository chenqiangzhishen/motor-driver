#pragma once
#include "stdafx.h"
#include "motor.h"
#include "Serial.h"
#include <windows.h>
#include "tchar.h"

void MotorWrite(CSerial &serial, unsigned char address, unsigned char data) {

	char str[30];
	int size = 0;

	size = sprintf_s(str, "<send 0x%02x 0x%02x>", 0x80 | address, data);
	serial.SendData(str, size);
	Sleep(20);
}

void MotorWriteTest(CSerial &serial) {

	char str[30];
	int size = 0;

	size = sprintf_s(str, "<%s>", "version");
	serial.SendData(str, size);
	printf("test version is %s \n", str);
	Sleep(200);
}

unsigned char MotorRead(CSerial &serial, unsigned char address) {

	char str[30];
	int size = 0;
	unsigned char ret_val = 0;

	size = sprintf_s(str, "<send 0x%02x 0x%02x,0x%02x 0x%02x>", address, 0xff, 0xff, 0xff);
	serial.SendData(str, size);
	Sleep(50);
	ret_val = serial.ReceiveData();
	return ret_val;
}

void LeftRightMdStop(CSerial &serial)
{
	//TBD:  MotorWaitWriteFinish();
	unsigned char controlReg = 0;

	controlReg = MotorRead(serial, 0x00);
	//Delay(3);
	MotorWrite(serial, 0x00, controlReg&~0x02);
}

//U8 MdMoveFlag = 1;
// 控制步进马达转动
// 参数num：转动步数
// 参数de：转动方向(0,正转 1,倒转)
void LeftRightMdMove(CSerial &serial, int num, U8 de)
{
	unsigned char controlReg = 0;
	//修改电机步数时应先关闭使能再修改步数及方向
	//目前发现大于一个字节，需要先停止，否则会出错。
	LeftRightMdStop(serial);

	if (de>0) de = 1;

	MotorWrite(serial, 0x0c, 0x7f & (num >> 8) | (de << 7));
	MotorWrite(serial, 0x0d, (U8)num & 0xff);
	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg | 0x02);
	//Delay(3);
}
