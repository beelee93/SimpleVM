#include "simplevm_machine.h"

#include <stdio.h>

using namespace SimpleVM;

int main(int argc, char** argv) {
	VirtualMachine mac;
	UINT8 program[] = { 
		0x01, 0xFF, 0x00, 0x00, 0x00, // LOAD 255
		0x02, 0x10, // MOV R1,R0
		0x10, 0x15, 0,0,0, // CALL 0x15
		0x01, 0x01, 0,0,0, // LOAD 1
		0x02, 0x20, // MOV R2,R0
		0, // NOP
		0, // NOP
		0x0E, 0x10, // 0x15: PUSH R1
		0x0D, 0x11, // XOR R1,R1
		0x0F, 0x10, // POP R1
		0xD0, // RET
		0
	};

	mac.loadProgram(program, sizeof(program));
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();
	mac.tick(); mac.debugPrintRegisters(); getchar();

	return 0;
}