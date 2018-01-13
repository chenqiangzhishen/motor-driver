#pragma once

#include "Serial.h"

#define U8 unsigned char

void MotorWrite(CSerial &serial, unsigned char address, unsigned char data);
void MotorWriteTest(CSerial &serial);

unsigned char MotorRead(CSerial &serial, unsigned char address);
void LeftRightMdStop(CSerial &serial);
void LeftRightMdMove(CSerial &serial, int num, U8 de);
void FrontBackMdStop(CSerial &serial);
void FrontBackMdMove(CSerial &serial, int num, U8 de);