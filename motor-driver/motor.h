#pragma once
#pragma once
#include "Serial.h" 
/*
#define MotorWaitWriteFinish()      while(MotorBoard_Read_FLAG & 0x02);

#define MotorWrite(address, data)   do{MotorBoard_ADDR = (0x80 | address);  \
MotorBoard_DATA = (data);       \
MotorWaitWriteFinish();Delay(1);}while(0)

#define MotorRead(data, address)    do{MotorBoard_ADDR = (0x7f & address); \
MotorBoard_DATA = (0x00);           \
MotorWaitWriteFinish(); \
Delay(1);   \
MotorBoard_ADDR = (0x7f & address); \
MotorBoard_DATA = (0x00);           \
MotorWaitWriteFinish(); \
Delay(1);   \
*data = MotorBoard_DATA;}while(0)
*/

#define U8 unsigned char
#define MotorWaitWriteFinish()      while(MotorBoard_Read_FLAG & 0x02);


//void MotorWrite(unsigned char address, unsigned char data);  
void MotorWrite(CSerial &serial, unsigned char address, unsigned char data);
void MotorWriteTest(CSerial &serial);

unsigned char MotorRead(CSerial &serial, unsigned char address);
void LeftRightMdStop(CSerial &serial);
void LeftRightMdMove(CSerial &serial, int num, U8 de);