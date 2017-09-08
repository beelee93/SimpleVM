#include "simplevm_memory.h"
#include <stdlib.h>
#include <string.h>

using namespace SimpleVM;

Memory::Memory(VM_UINT32 size) {
	this->size = size;

	// allocate size-number of bytes
	memory = (VM_BYTE*)malloc(size);

	if (!memory) 
		throw SVM_EXCEPTION_CANNOT_ALLOCATE_MEMORY;
}

Memory::~Memory() {
	if (memory) {
		free(memory);
		memory = NULL;
	}
}

VM_UINT32 Memory::getSize() {
	return size;
}

VM_BYTE & SimpleVM::Memory::getByte(VM_UINT32 offset)
{
	if (offset >= size)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS;
	return *(memory + offset);
}

VM_WORD & SimpleVM::Memory::getWord(VM_UINT32 offset)
{
	if (offset >= size-1)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS;
	return *((VM_WORD*)(memory + offset));
}

VM_DWORD & SimpleVM::Memory::getDword(VM_UINT32 offset)
{
	if (offset >= size - 3)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS;
	return *((VM_DWORD*)(memory + offset));
}

void SimpleVM::Memory::write(VM_BYTE * raw, VM_UINT32 length, VM_UINT32 startingOffsetInMemory)
{
	if (startingOffsetInMemory >= size)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS;

	for (int i = startingOffsetInMemory; 
		i < size && length>0; 
		i++, length--) {
		memory[i] = *(raw++);
	}
}
