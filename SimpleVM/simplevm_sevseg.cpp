#include "simplevm_sevseg.h"
#include "simplevm_machine.h"

using namespace SimpleVM;

#define SEV_SHORT 16
#define SEV_LONG 48
#define SEV_GAP 3

#define SEV_COL_0 0
#define SEV_COL_1 (SEV_SHORT+SEV_GAP)
#define SEV_COL_2 (SEV_COL_1+SEV_LONG+SEV_GAP)

#define SEV_ROW_0 0
#define SEV_ROW_1 (SEV_SHORT + SEV_GAP)
#define SEV_ROW_2 (SEV_ROW_1 + SEV_LONG + SEV_GAP)
#define SEV_ROW_3 (SEV_ROW_2 + SEV_SHORT + SEV_GAP)
#define SEV_ROW_4 (SEV_ROW_3 + SEV_LONG + SEV_GAP)

#define SEV_WIDTH (SEV_COL_2 + SEV_SHORT)

#define IDT_TIMER_SEV_SEG 1001

static int sevenSegDimensions[] = {
	SEV_COL_1,SEV_ROW_0,SEV_LONG,SEV_SHORT,
	SEV_COL_2,SEV_ROW_1,SEV_SHORT,SEV_LONG,
	SEV_COL_2,SEV_ROW_3,SEV_SHORT,SEV_LONG,
	SEV_COL_1,SEV_ROW_4,SEV_LONG,SEV_SHORT,
	SEV_COL_0,SEV_ROW_3,SEV_SHORT,SEV_LONG,
	SEV_COL_0,SEV_ROW_1,SEV_SHORT,SEV_LONG,
	SEV_COL_1,SEV_ROW_2,SEV_LONG,SEV_SHORT
};

IOSevenSegment::IOSevenSegment() {
	
	// create window
	HINSTANCE hInst = GetModuleHandle(0);

	threadData.hInst = hInst;
	threadData.window = NULL;
	threadData.thread = NULL;
	threadData.pObj = this;
	threadData.createSignal = 0;
	threadData.stopSignal = 0;

	threadData.thread = CreateThread(0, 0, windowThread, &threadData, CREATE_SUSPENDED, 0);
}

void SimpleVM::IOSevenSegment::attach(VirtualMachine* host)
{
	__super::attach(host);

	ResumeThread(threadData.thread);

	// wait for success/failure of window creation
	while (!threadData.createSignal);

	if (threadData.createSignal < 0) {
		threadData.thread = NULL;
		throw SVM_EXCEPTION_IO;
	}
}

SimpleVM::IOSevenSegment::~IOSevenSegment()
{
	if (threadData.createSignal > 0) {
		// signal a stop to the running thread
		threadData.stopSignal = 1;
		WaitForSingleObject(threadData.thread, INFINITE);
		
		CloseHandle(threadData.thread);
	}
}

IOMemoryRequest IOSevenSegment::getMemoryMapping()
{
	IOMemoryRequest req = { BASE_OFFSET, MEM_LENGTH };
	return req;
}

LRESULT IOSevenSegment::wndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	IOSevenSegment* obj = (IOSevenSegment*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (msg) {

	case WM_NCCREATE:
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lparam)->lpCreateParams);
		SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
		return TRUE;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		obj->onPaint(hWnd);
		InvalidateRect(hWnd, 0, 0);
		return 0;
	case WM_ERASEBKGND:
		return 1;
		
	}

	return DefWindowProc(hWnd, msg, wparam, lparam);
}

DWORD SimpleVM::IOSevenSegment::windowThread(LPVOID param)
{
	MSG msg;
	WNDCLASSEX Wc;

	CreateWindowThreadData *threadData = (CreateWindowThreadData*)param;

	Wc.cbSize = sizeof(WNDCLASSEX);
	Wc.style = 0;
	Wc.lpfnWndProc = wndProc;
	Wc.cbClsExtra = 0;
	Wc.cbWndExtra = 0;
	Wc.hInstance = threadData->hInst;
	Wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	Wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	Wc.hbrBackground = NULL;
	Wc.lpszMenuName = NULL;
	Wc.lpszClassName = L"SevenSegment";
	Wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&Wc)) {
		threadData->createSignal = -1;
		return -1;
	}

	threadData->window = CreateWindowEx(0, L"SevenSegment", L"Seven Segment Display",
		WS_OVERLAPPEDWINDOW, 0, 0, 640, 480,
		GetDesktopWindow(), NULL, threadData->hInst, threadData->pObj);

	if (!threadData->window) {
		threadData->createSignal = -2;
		return -2;
	}

	threadData->createSignal = 1;

	ShowWindow(threadData->window, SW_SHOW);
	UpdateWindow(threadData->window);

	do
	{
		if (threadData->stopSignal)
			break;

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while (msg.message != WM_QUIT);

	DestroyWindow(threadData->window);
	return 1;
}

void IOSevenSegment::onPaint(HWND hWnd) {
	PAINTSTRUCT paint;
	RECT rect;

	int win_width, win_height;

	HDC Memhdc;
	HBITMAP Membitmap;

	// get the client dimensions
	GetClientRect(hWnd, &rect);
	win_width = rect.right - rect.left;
	win_height = rect.bottom - rect.top;

	// start painting
	BeginPaint(hWnd, &paint);

	// create double buffer
	Memhdc = CreateCompatibleDC(paint.hdc);
	Membitmap = CreateCompatibleBitmap(paint.hdc, win_width, win_height);
	SelectObject(Memhdc, Membitmap);

	// DRAWING

	// fill background with black
	FillRect(Memhdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

	VM_UINT8 *segments = this->getPtr();

	// we will go thru bit by bit across the segment byte data
	for (int i = 0; i < 7; i++) {
		rect.left = sevenSegDimensions[i * 4];
		rect.top = sevenSegDimensions[i * 4 + 1];
		rect.right = rect.left + sevenSegDimensions[i * 4 + 2];
		rect.bottom = rect.top + sevenSegDimensions[i * 4 + 3];

		OffsetRect(&rect, SEV_SHORT, SEV_SHORT);

		// left 7-segment 
		if (segments[0] & (1 << i))
			SetDCBrushColor(Memhdc, RGB(60, 210, 60));
		else
			SetDCBrushColor(Memhdc, RGB(25, 25, 25));
		FillRect(Memhdc, &rect, (HBRUSH)GetStockObject(DC_BRUSH));

		OffsetRect(&rect, SEV_WIDTH + SEV_SHORT, 0);

		// right 7-segment
		if (segments[1] & (1 << i))
			SetDCBrushColor(Memhdc, RGB(60, 210, 60));
		else
			SetDCBrushColor(Memhdc, RGB(25, 25, 25));
		FillRect(Memhdc, &rect, (HBRUSH)GetStockObject(DC_BRUSH));
	}

	// BLIT
	BitBlt(paint.hdc, 0, 0, win_width, win_height, Memhdc, 0, 0, SRCCOPY);

	DeleteObject(Membitmap);
	DeleteDC(Memhdc);

	EndPaint(hWnd, &paint);

}