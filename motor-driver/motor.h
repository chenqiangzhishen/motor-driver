#pragma once

#include "Serial.h"

#define U8 unsigned char

void MotorWrite(CSerialPort *serial, unsigned char address, unsigned char data);
void MotorWriteTest(CSerialPort *serial);

unsigned char MotorRead(CSerialPort *serial, unsigned char address);
void LeftRightMdStop(CSerialPort *serial);
void LeftRightMdMove(CSerialPort *serial, int num, U8 de);
void FrontBackMdStop(CSerialPort *serial);
void FrontBackMdMove(CSerialPort *serial, int num, U8 de);