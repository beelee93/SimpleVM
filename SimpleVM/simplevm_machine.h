#pragma once
#include "simplevm_memory.h"

#define SVM_REGISTER_COUNT 8
#define SVM_MEMORY_SIZE 65536

namespace SimpleVM {

	// Encapsulates a fully functioning virtual machine
	class VirtualMachine {
		Memory* memory;

		UINT32 pc; // program counter
		UINT32 sp; // stack pointer
		UINT16 control; // control flags

		DWORD registers[SVM_REGISTER_COUNT];

		// stack operations
		void pushByte(BYTE x);
		void pushWord(WORD x);
		void pushDword(DWORD x);
		BYTE popByte();
		WORD popWord();
		DWORD popDword();


		int handleDataMovement(UINT8 opcode);
		int handleArithmetic(UINT8 opcode);
		int handleOthers(UINT8 opcode);

		void decodeRegisters(UINT8 regOperand, DWORD** reg1, DWORD** reg2);
	public:
		VirtualMachine();
		~VirtualMachine();

		// resets the PC, SP, and registers
		void reset();

		UINT32 getPC();
		UINT32 getSP();
		DWORD getRegister(UINT32 regIndex);

		// normal operation
		void loadProgram(BYTE* program, UINT32 programSize);
		void tick();

		void debugPrintRegisters();
		void debugPrintMemory(UINT32 offset);
	};
}