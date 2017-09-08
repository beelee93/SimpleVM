#include "simplevm_sevseg.h"
#include "simplevm_machine.h"

using namespace SimpleVM;

IOSevenSegment::IOSevenSegment(VirtualMachine* host) 
	: IOModule(host) {
	
	// create window
	HINSTANCE hInst = GetModuleHandle(0);
	
	MSG msg;
	WNDCLASSEX Wc;

	Wc.cbSize = sizeof(WNDCLASSEX);
	Wc.style = 0;
	Wc.lpfnWndProc = WndProc;
	Wc.cbClsExtra = 0;
	Wc.cbWndExtra = 0;
	Wc.hInstance = hInst;
	Wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	Wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	Wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	Wc.lpszMenuName = NULL;
	Wc.lpszClassName = L"SevenSegment";
	Wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&Wc))
		throw SVM_EXCEPTION_IO;

	handle = CreateWindowEx(WS_EX_TOPMOST, L"SevenSegment", L"Seven Segment Display", 
		WS_OVERLAPPEDWINDOW, 0,0,640,480, 
		GetDesktopWindow(), NULL, hInst, NULL);

	if (!handle)
		throw SVM_EXCEPTION_IO;

	ShowWindow(handle, SW_SHOW);
	UpdateWindow(handle);
	
	
	do 
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while (msg.message != WM_QUIT);

	DestroyWindow(handle);
	handle = NULL;
}

IOMemoryRequest IOSevenSegment::getMemoryMapping()
{
	IOMemoryRequest req = { BASE_OFFSET, MEM_LENGTH };
	return req;
}

LRESULT IOSevenSegment::WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wparam, lparam);
}
