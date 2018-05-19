#include "control.h"

// <set power=0x##>
// bit0: motor vdd, 0: turn off, 1: turn on
// bit1: 5V, 0: turn off, 1: turn on
// bit2: 12V, 0: turn off, 1: turn on

void MotorPowerOn(CnComm *serial, unsigned char data) {
	char str[20];
	int size = 0;

	size = sprintf_s(str, "<set power=0x%02x>", data);
	serial->Write(str);
	//NOTE: no less than 0.2ms, or motor will be abnormal when running
	//0.2ms should send 34 byte
	Sleep(150);
}

/*
void Power12V(CnComm *serial, unsigned char data) {
	char str[20];
	int size = 0;

	size = sprintf_s(str, "<set power=0x%02x>", ~0x04 | data);
	serial->Write(str);
	//NOTE: no less than 0.2ms, or motor will be abnormal when running
	//0.2ms should send 34 byte
	Sleep(150);
}

void Power5V(CnComm *serial, unsigned char data) {
	char str[20];
	int size = 0;

	size = sprintf_s(str, "<set power=0x%02x>", ~0x02 | data);
	serial->Write(str);
	//NOTE: no less than 0.2ms, or motor will be abnormal when running
	//0.2ms should send 34 byte
	Sleep(150);
}
*/