#include "simplevm_machine.h"

#include <stdio.h>

using namespace SimpleVM;

int main(int argc, char** argv) {
	VirtualMachine mac;
	UINT8 program[] = { 
		0x01, 1,0,0,0,		// 0: LOAD 1 (to R0)
		0x02, 0x10,			// 5: MOV R1,R0 (R1 = R0)
		0x01, 37,0,0,0,		// 7: Load 37 (to R0)
		0x02, 0x20,			// 12: MOV R2,R0 (R2 = R0)
		0x0D, 0x33,			// 14: XOR R3,R3 (zero out R3)
		0x43, 0x42,			// (loop) 16: MOVB R4, [R2]
		0x11, 0x43,			// 18: CMP R4, R3
		0x13, 36,0,0,0,		// 20: JEQ 36 (jump if R4==R3 to finish)
		0x02, 0x04,			// 25: MOV R0,R4 (R0 = R4)
		0x50, 0x21,			// 27: SYS INT_OUTCHAR (putchar)
		0x05, 0x21,			// 29: ADD R2,R2 (R2=R2+R1)
		0x12, 16,0,0,0,		// 31: JMP 16 (jump to loop)
		0x90, 				// (finish) 36: HALT
		'H','e','l','l','o',' ', // 37
		'w','o','r','l','d','\n',
		0
	};

	mac.loadProgram(program, sizeof(program));
	TimedClock clock(50);
	clock.setShowDebug(0);
	clock.setDebugRegisters(0x1F);
	mac.run(&clock);
	return 0;
}