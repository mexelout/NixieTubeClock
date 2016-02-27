#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HINSTANCE hInst;
HWND hWnd;

HDC hMemDC, hNixieDC;
HBITMAP hMemPrev, hNixiePrev;
unsigned short mouseX = 0, mouseY = 0;

void Release() {
	HBITMAP hBitmap = (HBITMAP)SelectObject(hMemDC, hMemPrev);
	DeleteObject(hBitmap);
	DeleteObject(hMemDC);

	hBitmap = (HBITMAP)SelectObject(hNixieDC, hNixiePrev);
	DeleteObject(hBitmap);
	DeleteObject(hNixieDC);
}

int screenWidth, screenHeight;

char str[25];
typedef struct _TAG_POSITION_ {
	float x;
	float y;
}NPOSITION;
typedef struct _TAG_SIZE_ {
	float width;
	float height;
}NSIZE;
typedef struct _TAG_RECT_ {
	NPOSITION origin;
	NSIZE size;
}NRECT;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
	switch(msg){
		case WM_CREATE: {
			HDC hdc = GetDC(hwnd);

			hMemDC = CreateCompatibleDC(hdc);
			HBITMAP hBitmap = CreateCompatibleBitmap(hdc, screenWidth, screenHeight);
			hMemPrev = (HBITMAP)SelectObject(hMemDC, hBitmap);

			hNixieDC = CreateCompatibleDC(hdc);
			hBitmap = LoadBitmap(hInst, "NIXIETUBE");
			hNixiePrev = (HBITMAP)SelectObject(hNixieDC, hBitmap);

			ReleaseDC(hwnd, hdc);

			SetTimer(hwnd, 1, 10, NULL);

			return 0;
		}
		case WM_TIMER: {
			SYSTEMTIME st;
			GetSystemTime(&st);
			// wHour���X���ԑ����āA���{���Ԃɂ���
			wsprintf(str, "%04d%02d%02d%02d%02d%02d%03d",
			st.wYear, st.wMonth, st.wDay,
			(st.wHour + 9) % 24, st.wMinute, st.wSecond, st.wMilliseconds);

			InvalidateRect(hwnd, NULL, false);
			return 0;
		}
		case WM_PAINT: {
			PatBlt(hMemDC, 0, 0, screenWidth, screenHeight, BLACKNESS);

			float rate = screenWidth / 1440.0;
			float width = 90 * rate;
			float height = 230 * rate;

			NRECT r, fr;
			r.origin.x = 0;
			r.origin.y = screenHeight / 2 - height;
			r.size.width = width;
			r.size.height = height;
			fr.origin.x = 0;
			fr.origin.y = 0;
			fr.size.width = 90;
			fr.size.height = 230;

			for(int i = 0; i < 8; i++) {
				r.origin.x = (float(i) * width) + ((float)screenWidth / 2) - (width * 4);
				char c[2] = {0};
				c[0] = str[i];
				int number = atoi(c);
				fr.origin.x = number * 90;
				StretchBlt(hMemDC, r.origin.x, r.origin.y, r.size.width, r.size.height,
							hNixieDC, fr.origin.x, fr.origin.y, fr.size.width, fr.size.height, SRCCOPY);
			}
			r.origin.y += height;
			for(int i = 8; i < 17; i++) {
				r.origin.x = (float(i - 8) * width) + ((float)screenWidth / 2) - (width * 4.5f);
				char c[2] = {0};
				c[0] = str[i];
				int number = atoi(c);
				fr.origin.x = number * 90;
				StretchBlt(hMemDC, r.origin.x, r.origin.y, r.size.width, r.size.height,
							hNixieDC, fr.origin.x, fr.origin.y, fr.size.width, fr.size.height, SRCCOPY);
			}

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			BitBlt(hdc, 0, 0, screenWidth, screenHeight, hMemDC, 0, 0, SRCCOPY);
			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_KEYDOWN: {
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;
		}
		case WM_MOUSEMOVE: {
			unsigned short x = LOWORD(lp);
			unsigned short y = HIWORD(lp);
			if(mouseX == 0 && mouseY == 0) {
				mouseX = x;
				mouseY = y;
			}
			if(abs(mouseX - x) > 10 || abs(mouseY - y)) {
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			return 0;
		}
		case WM_DESTROY: {                                   // �E�B���h�E���j�����ꂽ�Ƃ��̏���
			Release();
			PostQuitMessage(0);
			return 0;
		}
		default:                                            // �f�t�H���g����
			return DefWindowProc(hwnd, msg, wp, lp);
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {
	if(strcmp(cmdLine, "/s")) return 0;

	hInst = hInstance;
	WNDCLASS wc;

	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style         = CS_HREDRAW | CS_VREDRAW;             // �E�B���h�E�X�^�C��
	wc.lpfnWndProc   = (WNDPROC)WndProc;                    // �E�B���h�E�v���V�[�W��
	wc.cbClsExtra    = 0;                                   // �⏕�̈�T�C�Y
	wc.cbWndExtra    = 0;                                   // �⏕�̈�T�C�Y
	wc.hInstance     = hInstance;                           // �C���X�^���X�n���h��
	wc.hIcon         = NULL;     // �A�C�R��
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);         // �}�E�X�J�[�\��
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);          // �N���C�A���g�̈�w�i�F
	wc.lpszMenuName  = NULL;                                // �E�B���h�E���j���[
	wc.lpszClassName = TEXT("NixieTubeClock");                     // �E�B���h�E�N���X��
	if(!RegisterClass(&wc)) return 0;

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(
		wc.lpszClassName,                                   // �E�B���h�E�N���X��
		TEXT("NixieTubeClock"),                         // �E�B���h�E�^�C�g��
		WS_VISIBLE | WS_POPUP,        // �E�B���h�E�X�^�C��
		0, 0,
		screenWidth,
		screenHeight,
		NULL,                                               // �e�E�B���h�E
		NULL,                                               // �E�B���h�E���j���[
		hInstance,                                          // �C���X�^���X�n���h��
		NULL);                                              // WM_CREATE���

	// �E�B���h�E�̕\��
	ShowWindow(hWnd, cmdShow);                             // �\����Ԃ̐ݒ�
	UpdateWindow(hWnd);                                     // �N���C�A���g�̈�̍X�V

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);                             // ���z�L�[���b�Z�[�W�̕ϊ�
		DispatchMessage(&msg);                              // �E�B���h�E�v���V�[�W���֓]��
	}

	return (int)msg.wParam;
}
