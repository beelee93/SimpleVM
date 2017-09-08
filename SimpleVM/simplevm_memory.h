#pragma once

#include "simplevm_types.h"


namespace SimpleVM {

	// Encapsulates the RAM of the virtual machine
	// The RAM is byte-addressible
	class Memory {
	public:
		Memory(VM_UINT32 size);
		~Memory();

		VM_UINT32 getSize();

		// accessing memory
		VM_BYTE& getByte(VM_UINT32 offset);
		VM_WORD& getWord(VM_UINT32 offset);
		VM_DWORD& getDword(VM_UINT32 offset);

		// copying to memory
		void write(VM_BYTE* raw, VM_UINT32 length, VM_UINT32 startingOffsetInMemory);

	private:
		VM_UINT32 size;
		VM_BYTE* memory;
	};
}