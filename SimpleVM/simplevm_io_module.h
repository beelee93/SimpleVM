#pragma once
#include "simplevm_types.h"

namespace SimpleVM {
	class VirtualMachine;

	typedef struct {
		VM_UINT32 baseOffset;
		VM_UINT32 length;
	} IOMemoryRequest;

	class IOModule {
		VM_UINT8* _base;
		VM_UINT32 _length;
		VirtualMachine* host;

	public:
		IOModule(VirtualMachine* host);
		virtual ~IOModule();

	protected:
		// gets the memory mapping of this io module
		virtual IOMemoryRequest getMemoryMapping();

		VM_UINT8* getPtr();
		VM_UINT32 getMemorySize();
	
	};
}