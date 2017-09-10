#pragma once
#include "simplevm_io_module.h"
#include <Windows.h>

namespace SimpleVM {
	class IOSevenSegment;

	typedef struct {
		HINSTANCE hInst;
		HWND window;
		HANDLE thread;
		IOSevenSegment* pObj;
		int createSignal;
		int stopSignal;
	} CreateWindowThreadData;

	class IOSevenSegment : public IOModule {
		CreateWindowThreadData threadData;

	public:
		static const VM_UINT32 BASE_OFFSET = 4096;
		static const VM_UINT32 MEM_LENGTH = 2;

		IOSevenSegment();
		virtual ~IOSevenSegment();

		virtual void attach(VirtualMachine* host) override;

	protected:
		virtual IOMemoryRequest getMemoryMapping();

		void onPaint(HWND hwnd);

		static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
		static DWORD WINAPI windowThread(LPVOID param);
	};
}