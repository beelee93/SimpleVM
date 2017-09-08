#include "simplevm_machine.h"
#include <stdio.h>
using namespace SimpleVM;


SimpleVM::VirtualMachine::VirtualMachine()
{
	reset();

	memory = new Memory(SVM_MEMORY_SIZE);
}

SimpleVM::VirtualMachine::~VirtualMachine()
{
	delete memory;
	memory = NULL;
}

void SimpleVM::VirtualMachine::reset()
{
	for (int i = 0; i < SVM_REGISTER_COUNT; i++)
		registers[i] = 0;

	pc = sp = 0;
	control = 0;
	interrupt = INT_NULL;
	halted = false;
}

VM_UINT32 SimpleVM::VirtualMachine::getPC()
{
	return pc;
}

VM_UINT32 SimpleVM::VirtualMachine::getSP()
{
	return sp;
}

VM_DWORD SimpleVM::VirtualMachine::getRegister(VM_UINT32 regIndex)
{
	if (regIndex >= SVM_REGISTER_COUNT)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS;
	return registers[regIndex];
}

void SimpleVM::VirtualMachine::loadProgram(VM_BYTE * program, VM_UINT32 programSize)
{
	memory->write(program, programSize, 0);
}

// === stack operations ===
void SimpleVM::VirtualMachine::pushByte(VM_BYTE x)
{
	sp = (sp - 1) % SVM_MEMORY_SIZE;
	memory->getByte(sp) = x;
}

void SimpleVM::VirtualMachine::pushWord(VM_WORD x)
{
	sp = (sp - 2) % SVM_MEMORY_SIZE;
	memory->getWord(sp) = x;
}

void SimpleVM::VirtualMachine::pushDword(VM_DWORD x)
{
	sp = (sp - 4) % SVM_MEMORY_SIZE;
	memory->getDword(sp) = x;
}

VM_BYTE SimpleVM::VirtualMachine::popByte()
{
	VM_BYTE x = memory->getByte(sp);
	sp = (sp + 1) % SVM_MEMORY_SIZE;
	return x;
}

VM_WORD SimpleVM::VirtualMachine::popWord()
{
	VM_WORD x = memory->getWord(sp);
	sp = (sp + 2) % SVM_MEMORY_SIZE;
	return x;
}

VM_DWORD SimpleVM::VirtualMachine::popDword()
{
	VM_DWORD x = memory->getDword(sp);
	sp = (sp + 4) % SVM_MEMORY_SIZE;
	return x;
}

void SimpleVM::VirtualMachine::decodeRegisters(VM_UINT8 regOperand, VM_DWORD ** reg1, VM_DWORD ** reg2)
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
	if (halted)
		return; // do nothing if halted

	// check for interrupt
	if (interrupt != INT_NULL) {
		interrupt = processInterrupt();
		return;
	}
	if (pc >= SVM_MEMORY_SIZE)
		throw SVM_EXCEPTION_CANNOT_OUT_OF_MEMORY;

	// get the opcode
	VM_BYTE opcode = memory->getByte(pc++);

	if (opcode == opNOP) return; // do nothing

	if (handleDataMovement(opcode))
		goto handled;
	if (handleArithmetic(opcode))
		goto handled;
	if (handleJumps(opcode))
		goto handled;
	if (handleOthers(opcode))
		goto handled;

	// upon reaching here, it has not been handled
	throw SVM_EXCEPTION_UNSUPPORTED_OPCODE;

handled:
	return;
}

void SimpleVM::VirtualMachine::run(ClockBase* clockInstance)
{
	clockInstance->runOn(this);
}

bool SimpleVM::VirtualMachine::isHalted()
{
	return halted;
}



int SimpleVM::VirtualMachine::handleDataMovement(VM_UINT8 opcode) {
	if (opcode == opLOAD_I) {
		registers[0] = memory->getDword(pc);
		pc += 4;
	}
	else {
		int advancePC = 1;
		VM_DWORD *reg1, *reg2;
		VM_UINT8 regOperand = memory->getByte(pc);
		VM_UINT16 sign;
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

int SimpleVM::VirtualMachine::handleArithmetic(VM_UINT8 opcode) {
	VM_DWORD *reg1, *reg2;
	VM_UINT8 regOperand = memory->getByte(pc);
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

int SimpleVM::VirtualMachine::handleOthers(VM_UINT8 opcode) {
	int pcAdd = 1;
	VM_DWORD *reg1, *reg2;
	VM_UINT8 regOperand = memory->getByte(pc);
	VM_UINT16 sign;
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
	case opHALT:
		pcAdd = 0;
		setInterrupt(INT_HALT);
		break;
	case opSYS_I:
		pcAdd = 1;
		setInterrupt(regOperand);
		break;
	default:
		return 0;
	}

	pc += pcAdd;
	return 1;
}

int SimpleVM::VirtualMachine::handleJumps(VM_UINT8 opcode)
{
	int pcAdd = 4;
	VM_DWORD *reg1, *reg2;
	VM_UINT32 offset;
	VM_UINT8 regOperand = memory->getByte(pc);
	decodeRegisters(regOperand, &reg1, &reg2);

	switch (opcode) {
	case opCMP_RR:
		pcAdd = 1;
		regOperand = memory->getByte(pc);
		decodeRegisters(regOperand, &reg1, &reg2);
		if (*reg1 > *reg2) {
			UNSET(control, CONTROL_SIGN);
			UNSET(control, CONTROL_ZERO);
		}
		else if (*reg1 < *reg2) {
			SET(control, CONTROL_SIGN);
			UNSET(control, CONTROL_ZERO);
		}
		else {
			UNSET(control, CONTROL_SIGN);
			SET(control, CONTROL_ZERO);
		}
		break;

	case opJEQ_I:
		offset = memory->getDword(pc);
		if (ISSET(control, CONTROL_ZERO)) {
			pc = offset;
			pcAdd = 0;
		}
		break;
	case opJNEQ_I:
		offset = memory->getDword(pc);
		if (!ISSET(control, CONTROL_ZERO)) {
			pc = offset;
			pcAdd = 0;
		}
		break;
	case opJMP_I:
		offset = memory->getDword(pc);
		pc = offset;
		pcAdd = 0;
		break;
	default:
		return 0;
	}

	pc += pcAdd;
	return 1;
}

void SimpleVM::VirtualMachine::setInterrupt(VM_UINT8 intId) {
	interrupt = intId;
}

VM_UINT8 SimpleVM::VirtualMachine::processInterrupt() {
	switch (interrupt) {
	case INT_HALT:
		halted = true;
		break;
	case INT_OUTCHAR:
		printf("%c", registers[0] & 0xFF);
		break;
	}
	return INT_NULL;
}

void SimpleVM::VirtualMachine::debugPrintRegisters(int whichReg)
{
	if (!whichReg) return;

	printf("======== PC[%d]\n", pc);
	for (int i = 0; i < SVM_REGISTER_COUNT; i++) {
		if(whichReg & (1<<i))
			printf("Register %d: %d[%X]\n", i, registers[i], registers[i]);
	}
	printf("========\n");
}

void SimpleVM::VirtualMachine::debugPrintMemory(VM_UINT32 offset)
{
	printf("========\n");
	printf("[%x] = %x\n", offset, memory->getDword(offset));
	printf("========\n");
}

VM_UINT8* VirtualMachine::attachIO(IOModule* module, VM_UINT32 base, VM_UINT32 memLength)
{
	if (base < memory->getSize() &&
		base + memLength <= memory->getSize()) {
		return &(memory->getByte(base));
	}
	else
		return NULL;
}

void VirtualMachine::detachIO(IOModule * module)
{
	// detach module
	// currently nothing to do here
}
