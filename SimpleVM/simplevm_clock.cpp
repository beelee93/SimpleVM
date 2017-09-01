#include "simplevm_clock.h"
#include "simplevm_machine.h"

#include <stdio.h>

SimpleVM::ClockBase::ClockBase()
{
	debugReg = 0;
	tickCount = 0;
	showDebugMessages = 1;
}

void SimpleVM::ClockBase::tick()
{
	tickCount++;
	mac->tick();
	if (showDebugMessages) {
		printf("** Tick %d **\n", tickCount);
		mac->debugPrintRegisters(debugReg);
	}
}

void SimpleVM::ClockBase::setDebugRegisters(UINT8 whichReg)
{
	debugReg = whichReg;
}

void SimpleVM::ClockBase::setShowDebug(int show)
{
	showDebugMessages = show;
}

SimpleVM::TimedClock::TimedClock(clock_t tickDuration) {
	this->tickDuration = tickDuration;
}

void SimpleVM::TimedClock::runOn(VirtualMachine* mac) {
	this->mac = mac;
	clock_t last = clock();
	clock_t cur;
	bool running = true;
	while (running) {
		cur = clock();
		if (cur - last >= tickDuration) {
			last = cur;
			if (!mac->isHalted()) {
				try {
					tick();
				}
				catch (UINT32 ex) {
					printf("Exception occurred: %d\n", ex);
					running = false;
				}
			}
			else {
				printf("Halted\n");
				running = false;
			}
		}
	}
}

void SimpleVM::KeyClock::runOn(VirtualMachine* mac) {
	this->mac = mac;
	while (!mac->isHalted()) {
		try {
			tick();
		}
		catch (UINT32 ex) {
			printf("Exception occurred: %d\n", ex);
			break;
		}
		getchar();
	}
	printf("Halted\n");
}

