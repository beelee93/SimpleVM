#include "simplevm_machine.h"
#include <stdio.h>
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
	memory->write(program, programSize, 0);
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

void SimpleVM::VirtualMachine::decodeRegisters(UINT8 regOperand, DWORD ** reg1, DWORD ** reg2)
{
	int r1, r2;
	r1 = (regOperand & 0xF0) >> 4;
	r2 = regOperand & 0x0F;

	(*reg1) = &registers[r1];
	(*reg2) = &registers[r2];
}
// ==== the heart of the vm ====
void SimpleVM::VirtualMachine::tick()
{
	if (pc >= SVM_MEMORY_SIZE)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_MEMORY;

	// get the opcode
	BYTE opcode = memory->getByte(pc++);

	if (opcode == opNOP) return; // do nothing

	if (handleDataMovement(opcode))
		goto handled;
	if (handleArithmetic(opcode))
		goto handled;
	if (handleOthers(opcode))
		goto handled;

	// upon reaching here, it has not been handled
	throw SVM_EXCEPTION_UNSUPPORTED_OPCODE;

handled:
	return;
}



int SimpleVM::VirtualMachine::handleDataMovement(UINT8 opcode) {
	if (opcode == opLOAD_I) {
		registers[0] = memory->getDword(pc);
		pc += 4;
	}
	else {
		int advancePC = 1;
		DWORD *reg1, *reg2;
		UINT8 regOperand = memory->getByte(pc);
		UINT16 sign;
		decodeRegisters(regOperand, &reg1, &reg2);

		switch (opcode) {
		case opMOV_RR:
			*reg1 = *reg2;
			break;

		case opMOV_NR:
			memory->getDword(*reg1) = *reg2;
			break;
		case opMOV_RN:
			*reg1 = memory->getDword(*reg2);
			break;

		case opMOVB_NR:
			memory->getByte(*reg1) = (*reg2) & 0xFF;
			break;

		case opMOVB_RN:
			*reg1 = memory->getByte(*reg2);

			// sign extension
			sign = ((*reg1) & 0x80);
			if (sign)
				*reg1 = (*reg1) | 0xFFFFFF00;
			else
				*reg1 = (*reg1) & 0x000000FF;

			break;

		case opMOVW_NR:
			memory->getWord(*reg1) = (*reg2) & 0xFFFF;
			break;

		case opMOVW_RN:
			*reg1 = memory->getWord(*reg2);

			// sign extension
			sign = ((*reg1) & 0x8000);
			if (sign)
				*reg1 = (*reg1) | 0xFFFF0000;
			else
				*reg1 = (*reg1) & 0x0000FFFF;

			break;
		default:
			advancePC = 0;
			return 0;
		}
		pc += advancePC;
	}

	return 1;
}

int SimpleVM::VirtualMachine::handleArithmetic(UINT8 opcode) {
	DWORD *reg1, *reg2;
	UINT8 regOperand = memory->getByte(pc);
	decodeRegisters(regOperand, &reg1, &reg2);

	unsigned long long int mul;

	switch (opcode) {
	case opADD_RR:
		*reg1 = *reg1 + *reg2;
		break;
	case opSUB_RR:
		*reg1 = *reg1 - *reg2;
		break;
	case opMUL_RR:
		mul = (*reg1) * (*reg2);
		*reg1 = mul & (0xFFFFFFFF);
		registers[7] = (mul & 0xFFFFFFFF00000000) >> 32;
		break;
	case opDIV_RR:
		*reg1 = *reg1 / *reg2;
		break;
	case opMOD_RR:
		*reg1 = *reg1 % *reg2;
		break;
	case opAND_RR:
		*reg1 = *reg1 & *reg2;
		break;
	case opOR_RR:
		*reg1 = *reg1 | *reg2;
		break;
	case opXOR_RR:
		*reg1 = *reg1 ^ *reg2;
		break;
	case opNOT_R:
		*reg1 = ~(*reg1);
		break;
	default:
		return 0;
	}

	pc++;
	return 1;
}

int SimpleVM::VirtualMachine::handleOthers(UINT8 opcode) {
	int pcAdd = 1;
	DWORD *reg1, *reg2;
	UINT8 regOperand = memory->getByte(pc);
	UINT16 sign;
	decodeRegisters(regOperand, &reg1, &reg2);

	switch (opcode) {
	case opPUSH_R:
		pushDword(*reg1);
		break;
	case opPUSHB_R:
		pushByte(*reg1);
		break;
	case opPUSHW_R:
		pushWord(*reg1);
		break;
	case opPOP_R:
		*reg1 = popDword();
		break;
	case opPOPB_R:
		*reg1 = popByte();
		// sign extension
		sign = ((*reg1) & 0x80);
		if (sign)
			*reg1 = (*reg1) | 0xFFFFFF00;
		else
			*reg1 = (*reg1) & 0x000000FF;
		break;

	case opPOPW_R:
		*reg1 = popWord();
		// sign extension
		sign = ((*reg1) & 0x8000);
		if (sign)
			*reg1 = (*reg1) | 0xFFFF0000;
		else
			*reg1 = (*reg1) & 0x0000FFFF;

		break;
	case opCALL_I:
		pcAdd = 0;
		pushDword(pc+4);
		pc = memory->getDword(pc);
	break;
	case opRET:
		pcAdd = 0;
		pc = popDword();
		break;
	default:
		return 0;
	}

	pc += pcAdd;
	return 1;
}


void SimpleVM::VirtualMachine::debugPrintRegisters()
{
	printf("======== PC[%x]\n", pc);
	for (int i = 0; i < SVM_REGISTER_COUNT; i++)
		printf("Register %d: %X\n", i, registers[i]);
	printf("========\n");
}

void SimpleVM::VirtualMachine::debugPrintMemory(UINT32 offset)
{
	printf("========\n");
	printf("[%x] = %x\n", offset, memory->getDword(offset));
	printf("========\n");
}