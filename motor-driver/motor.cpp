#pragma once
#include "stdafx.h"
#include "motor.h"


void MotorWrite(CnComm *serial, unsigned char address, unsigned char data) {
	char str[20];
	int size = 0;

	size = sprintf_s(str, "<send 0x%02x 0x%02x>", 0x80 | address, data);
	//printf("send to motor=%s \n", str);
	serial->Write(str);
	//NOTE: no less than 0.2ms, or motor will be abnormal when running
	//0.2ms should send 34 byte
	Sleep(150);
}

void MotorWriteTest(CnComm *serial) {
	char str[30];
	int size = 0;

	size = sprintf_s(str, "<%s>", "version");
	serial->Write(str, size);
	Sleep(200);
	printf("test command is %s \n", str);
}

void MotorParallelMove(CnComm *serial, int fbNum, U8 fbD, int lrNum, U8 lrD, int udNum, U8 udD){
	unsigned char controlReg = 0;
	//修改电机步数时应先关闭使能再修改步数及方向
	//目前发现大于一个字节，需要先停止，否则会出错。

	MotorParallelStop(serial);
	// Direction: 1
	if (fbD>0) fbD = 1;
	// Direction: 1 -> left, 0 -> right
	if (lrD>0) lrD = 1;
	// Direction: 1 -> down, 0 -> up
	if (udD>0) udD = 1;

	if (fbNum) {
		MotorWrite(serial, 0x0c, 0x7f & (fbNum >> 8) | (fbD << 7));
		MotorWrite(serial, 0x0d, (U8)fbNum & 0xff);
	}

	if(lrNum){
		MotorWrite(serial, 0x0e, 0x7f & (lrNum >> 8) | (lrD << 7));
		MotorWrite(serial, 0x0f, (U8)lrNum & 0xff);
	}
	if (udNum) {
		MotorWrite(serial, 0x10, 0x7f & (udNum >> 8) | (udD << 7));
		MotorWrite(serial, 0x11, (U8)udNum & 0xff);
	}

	MotorWrite(serial, 0x00, controlReg | 0x0e);
}

CRITICAL_SECTION m_csCommunicationSyncTest;
void MotorReadWriteTest(CnComm *serial, int number) {
	char str[30];
	char read_buf[50];
	int size = 0;

	// initialize critical section
	InitializeCriticalSection(&m_csCommunicationSyncTest);
	// now it critical!
	EnterCriticalSection(&m_csCommunicationSyncTest);
	printf("--------[%d]begin------------- \n", number);

	size = sprintf_s(str, "<%s>", "version");
	printf("[%d]test command is %s \n", number, str);
	serial->Write(str, size);
	Sleep(200);
	serial->Read(read_buf, sizeof read_buf);
	printf("[%d]read_buf=%s \n", number, read_buf);
	printf("--------[%d]end------------- \n", number);
	LeaveCriticalSection(&m_csCommunicationSyncTest);
}

unsigned char MotorRead(CnComm *serial, unsigned char address) {
	char str[30];
	int size = 0;
	unsigned char ret_val = 0;

	size = sprintf_s(str, "<send 0x%02x 0x%02x,0x%02x 0x%02x>", address, 0xff, 0xff, 0xff);
	serial->Write(str);
	Sleep(100);
	ret_val = serial->Read(str, sizeof str);
	//printf("MotoRead() : ret_val=%d\n", ret_val);
	Sleep(100);

	return ret_val;
}

void MotorParallelStop(CnComm *serial)
{
	unsigned char controlReg = 0;

	// TODO: if open this it will abort and exit the program. why?? some error?
	//controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, 0x00);
}



void FrontBackMdStop(CnComm *serial)
{
	unsigned char controlReg = 0;

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg&~0x04);
}

void LeftRightMdStop(CnComm *serial)
{
	unsigned char controlReg = 0;

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg&~0x02);
}

void UpDownMdStop(CnComm *serial)
{
	unsigned char controlReg = 0;

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg&~0x06);
}

//U8 MdMoveFlag = 1;
// 控制步进马达转动
// 参数num：转动步数
// 参数de：转动方向(0,正转 1,倒转)
void FrontBackMdMove(CnComm *serial, int num, U8 de)
{
	unsigned char controlReg = 0;
	//修改电机步数时应先关闭使能再修改步数及方向
	//目前发现大于一个字节，需要先停止，否则会出错。
	FrontBackMdStop(serial);
	if (de>0) de = 1;

	MotorWrite(serial, 0x0c, 0x7f & (num >> 8) | (de << 7));
	MotorWrite(serial, 0x0d, (U8)num & 0xff);

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg | 0x04);
}

//U8 MdMoveFlag = 1;
// 控制步进马达转动
// 参数num：转动步数
// 参数de：转动方向(0,正转 1,倒转)
void LeftRightMdMove(CnComm *serial, int num, U8 de)
{
	unsigned char controlReg = 0;
	//修改电机步数时应先关闭使能再修改步数及方向
	//目前发现大于一个字节，需要先停止，否则会出错。

	LeftRightMdStop(serial);

	if (de>0) de = 1;

	MotorWrite(serial, 0x0e, 0x7f & (num >> 8) | (de << 7));
	MotorWrite(serial, 0x0f, (U8)num & 0xff);

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg | 0x02);
}

//U8 MdMoveFlag = 1;
// 控制步进马达转动
// 参数num：转动步数
// 参数de：转动方向(0,正转 1,倒转)
void UpDownMdMove(CnComm *serial, int num, U8 de)
{
	unsigned char controlReg = 0;
	//修改电机步数时应先关闭使能再修改步数及方向
	//目前发现大于一个字节，需要先停止，否则会出错。

	UpDownMdStop(serial);

	if (de>0) de = 1;

	MotorWrite(serial, 0x10, 0x7f & (num >> 8) | (de << 7));

	MotorWrite(serial, 0x11, (U8)num & 0xff);

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg | 0x06);
}
