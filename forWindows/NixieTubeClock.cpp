/*
  windows側のソースコード
*/

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

			ShowCursor(FALSE);
			SetTimer(hwnd, 1, 10, NULL);

			return 0;
		}
		case WM_TIMER: {
			SYSTEMTIME st;
			GetLocalTime(&st);
			wsprintf(str, "%04d%02d%02d%02d%02d%02d%03d",
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

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
		case WM_DESTROY: {                                   // ウィンドウが破棄されたときの処理
			Release();
			PostQuitMessage(0);
			return 0;
		}
		default:                                            // デフォルト処理
			return DefWindowProc(hwnd, msg, wp, lp);
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {
	if(strcmp(cmdLine, "/s")) return 0;

	hInst = hInstance;
	WNDCLASS wc;

	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style         = CS_HREDRAW | CS_VREDRAW;             // ウィンドウスタイル
	wc.lpfnWndProc   = (WNDPROC)WndProc;                    // ウィンドウプロシージャ
	wc.cbClsExtra    = 0;                                   // 補助領域サイズ
	wc.cbWndExtra    = 0;                                   // 補助領域サイズ
	wc.hInstance     = hInstance;                           // インスタンスハンドル
	wc.hIcon         = NULL;     // アイコン
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);         // マウスカーソル
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);          // クライアント領域背景色
	wc.lpszMenuName  = NULL;                                // ウィンドウメニュー
	wc.lpszClassName = TEXT("NixieTubeClock");                     // ウィンドウクラス名
	if(!RegisterClass(&wc)) return 0;

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// ウィンドウの作成
	hWnd = CreateWindow(
		wc.lpszClassName,                                   // ウィンドウクラス名
		TEXT("NixieTubeClock"),                         // ウィンドウタイトル
		WS_VISIBLE | WS_POPUP,        // ウィンドウスタイル
		0, 0,
		screenWidth,
		screenHeight,
		NULL,                                               // 親ウィンドウ
		NULL,                                               // ウィンドウメニュー
		hInstance,                                          // インスタンスハンドル
		NULL);                                              // WM_CREATE情報

	// ウィンドウの表示
	ShowWindow(hWnd, cmdShow);                             // 表示状態の設定
	UpdateWindow(hWnd);                                     // クライアント領域の更新

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);                             // 仮想キーメッセージの変換
		DispatchMessage(&msg);                              // ウィンドウプロシージャへ転送
	}

	return (int)msg.wParam;
}
