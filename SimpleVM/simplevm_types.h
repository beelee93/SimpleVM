#pragma once

// Type definitions

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;

typedef UINT8 BYTE;
typedef UINT16 WORD;
typedef UINT32 DWORD;

#ifndef NULL
#define NULL 0
#endif

#define CONTROL_ZERO 0x01
#define CONTROL_SIGN 0x02

#define SET(x, cnFlag) (x = (x | cnFlag))
#define UNSET(x, cnFlag) (x = (x & (~cnFlag)))
#define ISSET(x, cnFlag) ((x & cnFlag) == cnFlag)
// Exceptions

#define SVM_EXCEPTION_GENERIC 1000
#define SVM_EXCEPTION_CANNOT_ALLOCATE_MEMORY 1001
#define SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS 1002
#define SVM_EXCEPTION_CANNOT_OUT_OF_MEMORY 1003

#define SVM_EXCEPTION_UNSUPPORTED_OPCODE 2001

// Opcodes

// Format
//		opOPCODE_XY
//		X = type of operand 1
//		Y = type of operand 2
//
// Types of operand
//		R = Register
//		N = Register Indirect
//		I = Immediate

// no-operation
#define opNOP		0x00

#define opLOAD_I	0x01

#define opMOV_RR	0x02	
#define opMOV_NR	0x82
#define opMOV_RN	0x42

#define opMOVB_NR	0x83
#define opMOVB_RN	0x43

#define opMOVW_NR	0x84
#define opMOVW_RN	0x44

#define opADD_RR	0x05
#define opSUB_RR	0x06
#define opMUL_RR	0x07
#define opDIV_RR	0x08
#define opMOD_RR	0x09

#define opAND_RR	0x0A
#define opOR_RR		0x0B
#define opNOT_R		0x0C
#define opXOR_RR	0x0D

#define opPUSH_R	0x0E
#define opPUSHB_R	0x4E
#define opPUSHW_R	0xCE

#define opPOP_R		0x0F
#define opPOPB_R	0x4F
#define opPOPW_R	0xCF

#define opCALL_I	0x10
#define opRET		0xD0
#define opHALT		0x90
#define opSYS_I		0x50

#define opCMP_RR	0x11
#define opJMP_I		0x12
#define opJEQ_I		0x13
#define opJNEQ_I	0x53

// Interrupts

#define INT_NULL		0
#define INT_HALT		0x10
#define INT_OUTCHAR		0x21