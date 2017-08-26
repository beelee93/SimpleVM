#include "simplevm_machine.h"

#include <stdio.h>

using namespace SimpleVM;

int main(int argc, char** argv) {
	VirtualMachine mac;
	UINT8 program[] = { 
		0x00, // NOP
		0x01, 0xFF, 0x00, 0x00, 0x00, // Load 255 into R0
		0x02, 0x10, // MOV R1, R0
		0x01, 0x1,0,0,0, // Load 0x1 into R0
		0x0D, 0x01
	};

	mac.loadProgram(program, sizeof(program));
	mac.tick(); 
	mac.tick(); mac.debugPrintRegisters();
	mac.tick(); mac.debugPrintRegisters();
	mac.tick(); mac.debugPrintRegisters(); 
	mac.tick(); mac.debugPrintRegisters(); 

	return 0;
}