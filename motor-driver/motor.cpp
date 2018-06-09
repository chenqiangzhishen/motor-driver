#pragma once
#include "stdafx.h"
#include "motor.h"


void MotorWrite(CnComm *serial, unsigned char address, unsigned char data) {
	char str[20];
	int size = 0;

	size = sprintf_s(str, "<send 0x%02x 0x%02x>", 0x80 | address, data);
	printf("send to motor=%s \n", str);
	serial->Write(str);
	//NOTE: no less than 2ms, or motor will be abnormal when running
	//2ms should send 23 byte
	//Sleep(200);
	Sleep(30);
}

void MotorWriteTest(CnComm *serial) {
	char str[30];
	int size = 0;

	size = sprintf_s(str, "<%s>", "version");
	serial->Write(str, size);
	Sleep(200);
	printf("test command is %s \n", str);
}

/*
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
*/

// 0x11 write 0x00,0x01...0x10,0x11..,0x20,0x21,...,0xff
void MotorReadWriteTest1(CnComm *serial) {
	int cnt = 0;
	int ret = 0;

	while(1){
		printf("--------[%d]begin------------- \n", cnt);
		MotorWrite(serial, 0x11, cnt);
		ret=MotorRead(serial, 0x11);
		printf("ret=0x%02x\n", ret);
		/*
		if( cnt != ret ){
			printf("cnt != ret");
			break;
		}
		*/
		printf("--------[%d]end------------- \n", cnt);
		++cnt;
		if (cnt > 0xff) {
			break;
		}
	}
}

void MotorReadWriteTest2(CnComm *serial) {
	int cnt = 0xff;

	while (1) {
		printf("--------[%d]begin------------- \n", cnt);
		MotorWrite(serial, 0x12, cnt);
		MotorRead(serial, 0x12);
		printf("--------[%d]end------------- \n", cnt);
		++cnt;
		if (cnt > 0xffff) {
			break;
		}
	}
}
unsigned char MotorRead(CnComm *serial, unsigned char address) {
	char str[50];
	unsigned char ret = 0;

	//27 bytes
	sprintf_s(str, "<send 0x%02x 0x%02x,0x%02x 0x%02x>", address, 0xff, 0xff, 0xff);
	printf("[in read] send to motor=%s \n", str);
	serial->Write(str);
	//Sleep(300);
	Sleep(50);
	serial->ReadString(str, sizeof str);

	if (strstr(str, "<EXOR RTOS V1.0>") != NULL) {
		printf("==\n");
		
		//nothing to do.
	}
	else if (strstr(str, "<event #1>") != NULL) {
		//m_lightpath_swith = true;
	    //size = sprintf_s(str, "<send 0x%02x 0x%02x>", 0x80, 0x00);
		//SendData(str, size);
	}
	else if (strchr(str, ',')) {
		std::string buf(str);
		std::size_t found_begin = buf.find('<');
		std::size_t found_end = buf.find_last_of('>');
		if (found_begin != std::string::npos && found_end != std::string::npos) {
			if (found_end - 4 >= 0) {
				ret = (unsigned char)std::stoul(buf.substr(found_end - 4, 4).c_str(), nullptr, 16);
				printf("read motor=0x%02x\n", ret);
				return ret;
			}
		}
	}

	return 0;
}
extern U8 g_light_path_sign_on;
void LightPathMdInitPos(CnComm *serial) {
	int count = 0;
	unsigned char lightPathSwitchReg = 0;
	unsigned char lightPathSwithSign = 0;
	int step = 5;
	/*
	lightPathSwitchReg = MotorRead(serial, 0x03);
	if (lightPathSwitchReg & 0x01) {
		// 用于标志马达复位时，光路的光电管是否被档
		lightPathSwithSign = 1;
	}
	else {
		lightPathSwithSign = 0;
	}
	*/
	if (g_light_path_sign_on) { // 挡
		while (g_light_path_sign_on) { // 走直到不挡
			LightPathMdMove(serial, step, 0);
		}
		while (!g_light_path_sign_on) { // 走直到挡
			LightPathMdMove(serial, step, 1);
		}
		while (g_light_path_sign_on) { // 走直到不挡
			LightPathMdMove(serial, step, 0);
		}
		while (!g_light_path_sign_on) { // 走直到挡
			LightPathMdMove(serial, step, 1);
		}
	}
	else { // 不挡
		while (!g_light_path_sign_on) { //走直到挡
			LightPathMdMove(serial, step, 1);
		}
		while (g_light_path_sign_on) { //走直到不挡
			LightPathMdMove(serial, step, 0);
		}
		while (!g_light_path_sign_on) { //走直到挡
			LightPathMdMove(serial, step, 1);
		}
		while (g_light_path_sign_on) { // 走直到不挡
			LightPathMdMove(serial, step, 0);
		}
	}
	LightPathMdStop(serial);//马达置0.拉低.
}

void MotorParallelStop(CnComm *serial)
{
	unsigned char controlReg = 0;

	// TODO: if open this it will abort and exit the program. why?? some error?
	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg&~0x1e);
}

//lr=leftrightmotor
//fb=frontbackmotor
//ud=updownmotor
//lp=lightpathmotor
void MotorParallelMove(CnComm *serial, int lrNum, U8 lrD, int fbNum, U8 fbD, int udNum, U8 udD, int lpNum, U8 lpD){
	unsigned char controlReg = 0;
	//修改电机步数时应先关闭使能再修改步数及方向
	//目前发现大于一个字节，需要先停止，否则会出错。

	MotorParallelStop(serial);
	// Direction: 1 -> left, 0 -> right
	if (lrD>0) lrD = 1;
	// Direction: 1 -> front, 0 -> back
	if (fbD>0) fbD = 1;
	// Direction: 1 -> down, 0 -> up
	if (udD>0) udD = 1;
	// Direction: 1 -> down, 0 -> up
	if (lpD>0) lpD = 1;

	if (lrNum) {
		MotorWrite(serial, 0x0e, 0x7f & (lrNum >> 8) | (lrD << 7));
		MotorWrite(serial, 0x0f, (U8)lrNum & 0xff);
	}

	if (fbNum) {
		MotorWrite(serial, 0x0c, 0x7f & (fbNum >> 8) | (fbD << 7));
		MotorWrite(serial, 0x0d, (U8)fbNum & 0xff);
	}

	if (udNum) {
		MotorWrite(serial, 0x10, 0x7f & (udNum >> 8) | (udD << 7));
		MotorWrite(serial, 0x11, (U8)udNum & 0xff);
	}

	if (lpNum) {
		MotorWrite(serial, 0x12, 0x7f & (udNum >> 8) | (udD << 7));
		MotorWrite(serial, 0x13, (U8)udNum & 0xff);
	}

	MotorWrite(serial, 0x00, controlReg | 0x1e);
}

void LeftRightMdStop(CnComm *serial)
{
	unsigned char controlReg = 0;

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg&~0x04);
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
	MotorWrite(serial, 0x00, controlReg | 0x04);
}

void FrontBackMdStop(CnComm *serial)
{
	unsigned char controlReg = 0;

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg&~0x02);
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
	MotorWrite(serial, 0x00, controlReg | 0x02);
}

void UpDownMdStop(CnComm *serial)
{
	unsigned char controlReg = 0;

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg&~0x08);
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
	MotorWrite(serial, 0x00, controlReg | 0x08);
}

void LightPathMdStop(CnComm *serial)
{
	unsigned char controlReg = 0;

	controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg&~0x10);
}

//U8 MdMoveFlag = 1;
// 控制步进马达转动
// 参数num：转动步数
// 参数de：转动方向(0,正转 1,倒转)
void LightPathMdMove(CnComm *serial, int num, U8 de)
{
	unsigned char controlReg = 0;
	//修改电机步数时应先关闭使能再修改步数及方向
	//目前发现大于一个字节，需要先停止，否则会出错。
	LightPathMdStop(serial);
	if (de>0) de = 1;

	MotorWrite(serial, 0x12, 0x7f & (num >> 8) | (de << 7));
	MotorWrite(serial, 0x13, (U8)num & 0xff);

	//controlReg = MotorRead(serial, 0x00);
	MotorWrite(serial, 0x00, controlReg | 0x10);
}
