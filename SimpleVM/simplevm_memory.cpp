#include "simplevm_memory.h"
#include <stdlib.h>
#include <string.h>

using namespace SimpleVM;

Memory::Memory(UINT32 size) {
	this->size = size;

	// allocate size-number of bytes
	memory = (BYTE*)malloc(size);

	if (!memory) 
		throw SVM_EXCEPTION_CANNOT_ALLOCATE_MEMORY;
}

Memory::~Memory() {
	if (memory) {
		free(memory);
		memory = NULL;
	}
}

UINT32 Memory::getSize() {
	return size;
}

BYTE & SimpleVM::Memory::getByte(UINT32 offset)
{
	if (offset >= size)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS;
	return *(memory + offset);
}

WORD & SimpleVM::Memory::getWord(UINT32 offset)
{
	if (offset >= size-1)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS;
	return *((WORD*)(memory + offset));
}

DWORD & SimpleVM::Memory::getDword(UINT32 offset)
{
	if (offset >= size - 3)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS;
	return *((DWORD*)(memory + offset));
}

void SimpleVM::Memory::write(BYTE * raw, UINT32 length, UINT32 startingOffsetInMemory)
{
	if (startingOffsetInMemory >= size)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS;

	for (int i = startingOffsetInMemory; 
		i < size && length>0; 
		i++, length--) {
		memory[i] = *(raw++);
	}
}
