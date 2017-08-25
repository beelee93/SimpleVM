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


// Exceptions

#define SVM_EXCEPTION_GENERIC 1000
#define SVM_EXCEPTION_CANNOT_ALLOCATE_MEMORY 1001
#define SVM_EXCEPTION_CANNOT_OUT_OF_BOUNDS 1002