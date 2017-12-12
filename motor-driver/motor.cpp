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

	//	size = sprintf_s(str, "<send 0x%02x 0x%02x>", 0x80 | address, data);
	serial.SendData("<version>", 10);
	Sleep(20);
}

unsigned char MotorRead(CSerial &serial, unsigned char address) {
	/*
	CSerial serial;
	std::string cmd_prefix = std::string("<send ");
	std::string addr = std::to_string(0x80 | address);
	std::string space = std::string(" ");
	std::string data_s = std::to_string(data);
	std::string cmd_subfix = std::string(">");
	std::string state = cmd_prefix + addr + space + data_s + cmd_subfix;
	char* str = new char[state.length() + 1];
	strcpy_s(str, state.c_str());
	serial.SendData(str, strlen(str));
	*/

	char str[30];
	int size = 0;
	unsigned char ret_val = 0;

	size = sprintf_s(str, "<send 0x%02x 0x%02x,0x%02x 0x%02x>", address, 0xff, 0xff, 0xff);
	serial.SendData(str, size);
	Sleep(20);
	ret_val = serial.ReceiveData();
	return ret_val;
	//return 0;


	/*
	MotorBoard_ADDR = (0x7f & address); \
	MotorBoard_DATA = (0x00);           \
	MotorWaitWriteFinish(); \
	Delay(1);   \
	MotorBoard_ADDR = (0x7f & address); \
	MotorBoard_DATA = (0x00);           \
	MotorWaitWriteFinish(); \
	Delay(1);   \
	*data = MotorBoard_DATA;

	*/
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
// ���Ʋ������ת��
// ����num��ת������
// ����de��ת������(0,��ת 1,��ת)
void LeftRightMdMove(CSerial &serial, int num, U8 de)
{
	unsigned char controlReg = 0;
	//�޸ĵ������ʱӦ�ȹر�ʹ�����޸Ĳ���������
	//Ŀǰ���ִ���һ���ֽڣ���Ҫ��ֹͣ����������
	LeftRightMdStop(serial);

	if (de>0) de = 1;

	MotorWrite(serial, 0x0c, 0x7f & (num >> 8) | (de << 7));
	MotorWrite(serial, 0x0d, (U8)num & 0xff);
	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg | 0x02);
	//Delay(3);
}