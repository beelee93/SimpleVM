#pragma once
#include "simplevm_io_module.h"
#include <Windows.h>

namespace SimpleVM {

	class IOSevenSegment : public IOModule {
		HWND handle;

	public:
		static const VM_UINT32 BASE_OFFSET = 4096;
		static const VM_UINT32 MEM_LENGTH = 2;

		IOSevenSegment(VirtualMachine* host);

	protected:
		virtual IOMemoryRequest getMemoryMapping() override;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};
}