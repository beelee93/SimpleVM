#include "simplevm_machine.h"
#include "simplevm_sevseg.h"

#include <stdio.h>

using namespace SimpleVM;

int main(int argc, char** argv) {
	VirtualMachine mac;
	VM_UINT8 program[] = { 
		0x01, 0,0x10,0,0, // LOAD 4096
		0x02, 0x10, // MOV R1,R0
		0x01, 0x06,0x5B,0,0, // LOAD 
		0x84, 0x10 // MOVW [R1],R0
	};

	mac.loadProgram(program, sizeof(program));
	IOSevenSegment* seg = new IOSevenSegment();
	seg->attach(&mac);

#ifdef TIMED
	TimedClock clock(50);
	clock.setShowDebug(0);
	clock.setDebugRegisters(0x1F);
#else
	KeyClock clock;
#endif
	mac.run(&clock);

	delete seg;
	seg = NULL;
	return 0;
}