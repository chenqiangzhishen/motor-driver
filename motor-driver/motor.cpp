#pragma once

#include <windows.h>
#include "stdafx.h"
#include "tchar.h"
#include "motor.h"
#include "Serial.h"

void MotorWrite(CSerialPort *serial, unsigned char address, unsigned char data) {
	char str[100];
	int size = 0;

	size = sprintf_s(str, "<send 0x%02x 0x%02x>", 0x80 | address, data);
	serial->WriteToPort(str);
	//NOTE: no less than 0.2ms, or motor will be abnormal when running
	Sleep(200);
}

void MotorWriteTest(CSerialPort *serial) {
	char str[30];
	int size = 0;

	size = sprintf_s(str, "<%s>", "version");
	serial->WriteToPort(str);
	Sleep(200);
	printf("test command is %s \n", str);
}

unsigned char MotorRead(CSerialPort *serial, unsigned char address) {
	char str[30];
	int size = 0;
	unsigned char ret_val = 0;

	size = sprintf_s(str, "<send 0x%02x 0x%02x,0x%02x 0x%02x>", address, 0xff, 0xff, 0xff);
	serial->WriteToPort(str);
	Sleep(100);
	ret_val = serial->ReceiveData();
	printf("MotoRead() : ret_val=%d\n", ret_val);
	//serial.m_ready = false;
	Sleep(100);

	return ret_val;
}

void LeftRightMdStop(CSerialPort *serial)
{
	unsigned char controlReg = 0;

	//controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg&~0x02);
}

void FrontBackMdStop(CSerialPort *serial)
{
	unsigned char controlReg = 0;

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg&~0x04);
}

//U8 MdMoveFlag = 1;
// 控制步进马达转动
// 参数num：转动步数
// 参数de：转动方向(0,正转 1,倒转)
void LeftRightMdMove(CSerialPort *serial, int num, U8 de)
{
	unsigned char controlReg = 0;
	//修改电机步数时应先关闭使能再修改步数及方向
	//目前发现大于一个字节，需要先停止，否则会出错。
	LeftRightMdStop(serial);

	if (de>0) de = 1;

	printf("motor number=0x%02x\n", num);
	U8 high = 0x7f & (num >> 8) | (de << 7);
	U8 low = (U8)num & 0xff;
	//MotorWrite(serial, 0x0c, 0x7f & (num >> 8) | (de << 7));
	//printf("motor number_high=0x%02x\n", high);
	MotorWrite(serial, 0x0c, high);
	//printf("motor number_low=0x%02x\n", low);
	MotorWrite(serial, 0x0d, low);
	//MotorWrite(serial, 0x0d, (U8)num & 0xff);

	//controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg | 0x02);
}

//U8 MdMoveFlag = 1;
// 控制步进马达转动
// 参数num：转动步数
// 参数de：转动方向(0,正转 1,倒转)
void FrontBackMdMove(CSerialPort *serial, int num, U8 de)
{
	unsigned char controlReg = 0;
	//修改电机步数时应先关闭使能再修改步数及方向
	//目前发现大于一个字节，需要先停止，否则会出错。
	FrontBackMdStop(serial);

	if (de>0) de = 1;

	printf("motor number=0x%02x\n", num);
	U8 high = 0x7f & (num >> 8) | (de << 7);
	U8 low = (U8)num & 0xff;
	//MotorWrite(serial, 0x0c, 0x7f & (num >> 8) | (de << 7));
	//printf("motor number_high=0x%02x\n", high);
	MotorWrite(serial, 0x0e, high);
	//printf("motor number_low=0x%02x\n", low);
	MotorWrite(serial, 0x0f, low);
	//MotorWrite(serial, 0x0d, (U8)num & 0xff);

	//controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg | 0x04);
}