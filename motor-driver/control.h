#pragma once
#include "CnComm.h"

void MotorPowerOn(CnComm *serial, unsigned char data);
void Power12V(CnComm *serial, unsigned char data);
void Power5V(CnComm *serial, unsigned char data);
