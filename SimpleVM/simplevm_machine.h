#pragma once
#include "simplevm_clock.h"
#include "simplevm_memory.h"

#define SVM_REGISTER_COUNT 8
#define SVM_MEMORY_SIZE 65536

namespace SimpleVM {
	class IOModule;
	
	// Encapsulates a fully functioning virtual machine
	class VirtualMachine {
		Memory* memory;

		VM_UINT32 pc; // program counter
		VM_UINT32 sp; // stack pointer
		VM_UINT16 control; // control flags
		VM_UINT8 interrupt; // the current interrupt signal
		VM_DWORD registers[SVM_REGISTER_COUNT];

		bool halted;

		// stack operations
		void pushByte(VM_BYTE x);
		void pushWord(VM_WORD x);
		void pushDword(VM_DWORD x);
		VM_BYTE popByte();
		VM_WORD popWord();
		VM_DWORD popDword();


		int handleDataMovement(VM_UINT8 opcode);
		int handleArithmetic(VM_UINT8 opcode);
		int handleOthers(VM_UINT8 opcode);
		int handleJumps(VM_UINT8 opcode);

		void decodeRegisters(VM_UINT8 regOperand, VM_DWORD** reg1, VM_DWORD** reg2);

		void setInterrupt(VM_UINT8 intId);
		VM_UINT8 processInterrupt();

	public:
		VirtualMachine();
		~VirtualMachine();

		// resets the PC, SP, and registers
		void reset();

		VM_UINT32 getPC();
		VM_UINT32 getSP();
		VM_DWORD getRegister(VM_UINT32 regIndex);

		// normal operation
		void loadProgram(VM_BYTE* program, VM_UINT32 programSize);

		// performs 1 CPU tick
		void tick();

		// runs this machine until halt
		void run(ClockBase* clockInstance);

		bool isHalted();

		void debugPrintRegisters(int whichReg);
		void debugPrintMemory(VM_UINT32 offset);

		// IO Module operations
		VM_UINT8* attachIO(IOModule* module, VM_UINT32 base, VM_UINT32 memLength);
		void detachIO(IOModule* module);
	};
}