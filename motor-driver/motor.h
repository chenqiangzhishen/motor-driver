#pragma once
#include "stdafx.h"
#include <windows.h>
#include "CnComm.h"
#include <string>

#define U8 unsigned char
extern CRITICAL_SECTION    m_csCommunicationSyncTest;


unsigned char MotorRead(CnComm *serial, unsigned char address);
void MotorWrite(CnComm *serial, unsigned char address, unsigned char data);
void MotorWriteTest(CnComm *serial);
//void MotorReadWriteTest(CnComm *serial, int number);
void MotorReadWriteTest1(CnComm *serial);
void MotorReadWriteTest2(CnComm *serial);

void LightPathMdStop(CnComm *serial);
void LightPathMdMove(CnComm *serial, int num, U8 de);
void LightPathMdInitPos(CnComm *serial);

void FrontBackMdStop(CnComm *serial);
void FrontBackMdMove(CnComm *serial, int num, U8 de);

void LeftRightMdStop(CnComm *serial);
void LeftRightMdMove(CnComm *serial, int num, U8 de);

void UpDownMdStop(CnComm *serial);
void UpDownMdMove(CnComm *serial, int num, U8 de);

void MotorParallelStop(CnComm *serial);
void MotorParallelMove(CnComm *serial, int lrNum, U8 lrD, int fbNum, U8 fbD, int udNum, U8 udD, int lpNum, U8 lpD);