#include "simplevm_io_module.h"
#include "simplevm_machine.h"

using namespace SimpleVM;

IOModule::IOModule(VirtualMachine* host) {
	// get the memory mapping
	IOMemoryRequest req = this->getMemoryMapping();
	_length = req.length;

	// attach this io module
	_base = host->attachIO(this, req.baseOffset, req.length);
	this->host = host;
}

IOModule::~IOModule() {
	host->detachIO(this);
	_base = NULL;
}

IOMemoryRequest IOModule::getMemoryMapping()
{
	return { 0,0 };
}

VM_UINT8 * IOModule::getPtr()
{
	return _base;
}

VM_UINT32 IOModule::getMemorySize()
{
	return _length;
}

