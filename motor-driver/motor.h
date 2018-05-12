#pragma once
#include "stdafx.h"
#include <windows.h>
#include "CnComm.h"

#define U8 unsigned char

void MotorWrite(CnComm *serial, unsigned char address, unsigned char data);
void MotorWriteTest(CnComm *serial);
void MotorReadWriteTest(CnComm *serial, int number);

void MotorParallelStop(CnComm *serial);
void MotorParallelMove(CnComm *serial, int fbNum, U8 fbD, int lrNum, U8 lrD, int udNum, U8 udD);

extern CRITICAL_SECTION    m_csCommunicationSyncTest;
unsigned char MotorRead(CnComm *serial, unsigned char address);
void FrontBackMdStop(CnComm *serial);
void FrontBackMdMove(CnComm *serial, int num, U8 de);
void LeftRightMdStop(CnComm *serial);
void LeftRightMdMove(CnComm *serial, int num, U8 de);
void UpDownMdStop(CnComm *serial);
void UpDownMdMove(CnComm *serial, int num, U8 de);