#pragma once

#include "simplevm_types.h"


namespace SimpleVM {

	// Encapsulates the RAM of the virtual machine
	// The RAM is byte-addressible
	class Memory {
	public:
		Memory(UINT32 size);
		~Memory();

		UINT32 getSize();

		// accessing memory
		BYTE& getByte(UINT32 offset);
		WORD& getWord(UINT32 offset);
		DWORD& getDword(UINT32 offset);

		// copying to memory
		void write(BYTE* raw, UINT32 length, UINT32 startingOffsetInMemory);

	private:
		UINT32 size;
		BYTE* memory;
	};
}