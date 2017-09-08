#pragma once
#include "simplevm_types.h"
#include <time.h>

namespace SimpleVM {
	class VirtualMachine;

	class ClockBase {
	private:
		VM_UINT8 debugReg;
		VM_UINT32 tickCount;
		int showDebugMessages;

	protected: 
		ClockBase();
		VirtualMachine* mac;
		void tick();

	public:
		void setDebugRegisters(VM_UINT8 whichReg);
		void setShowDebug(int show);
		virtual void runOn(VirtualMachine*) = 0;
	};

	// Timed, autonomous triggering of the tick function 
	class TimedClock : public ClockBase {
	private:
		clock_t tickDuration;

	public:
		TimedClock(clock_t tickDuration);
		virtual void runOn(VirtualMachine*);
	};

	// Allows the user to control the CPU cycle by keyboard
	class KeyClock : public ClockBase {
	public:
		virtual void runOn(VirtualMachine*);
	};
}