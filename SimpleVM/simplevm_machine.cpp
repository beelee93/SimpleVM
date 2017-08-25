#include "simplevm_machine.h"

using namespace SimpleVM;


VirtualMachine::VirtualMachine()
{
	reset();

	memory = new Memory(SVM_MEMORY_SIZE);
}

VirtualMachine::~VirtualMachine()
{
	delete memory;
	memory = NULL;
}

void VirtualMachine::reset()
{
	for (int i = 0; i < SVM_REGISTER_COUNT; i++)
		registers[i] = 0;

	pc = sp = 0;
	control = 0;
}

UINT32 VirtualMachine::getPC()
{
	return pc;
}

UINT32 VirtualMachine::getSP()
{
	return sp;
}

DWORD VirtualMachine::getRegister(UINT32 regIndex)
{
	if (regIndex >= SVM_REGISTER_COUNT)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS;
	return registers[regIndex];
}

void SimpleVM::VirtualMachine::loadProgram(BYTE * program, UINT32 programSize)
{
}

void SimpleVM::VirtualMachine::tick()
{
}

// === stack operations ===
void SimpleVM::VirtualMachine::pushByte(BYTE x)
{
	sp = (sp - 1) % SVM_MEMORY_SIZE;
	memory->getByte(sp) = x;
}

void SimpleVM::VirtualMachine::pushWord(WORD x)
{
	sp = (sp - 2) % SVM_MEMORY_SIZE;
	memory->getWord(sp) = x;
}

void SimpleVM::VirtualMachine::pushDword(DWORD x)
{
	sp = (sp - 4) % SVM_MEMORY_SIZE;
	memory->getDword(sp) = x;
}

BYTE SimpleVM::VirtualMachine::popByte()
{
	BYTE x = memory->getByte(sp);
	sp = (sp + 1) % SVM_MEMORY_SIZE;
	return x;
}

WORD SimpleVM::VirtualMachine::popWord()
{
	WORD x = memory->getWord(sp);
	sp = (sp + 2) % SVM_MEMORY_SIZE;
	return x;
}

DWORD SimpleVM::VirtualMachine::popDword()
{
	DWORD x = memory->getDword(sp);
	sp = (sp + 4) % SVM_MEMORY_SIZE;
	return x;
}