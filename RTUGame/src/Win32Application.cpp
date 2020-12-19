//Copyright (c) 2021 sselecirPyM
//This file is part of RTU Game.
//
//RTU Game is free software : you can redistribute itand /or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//RTU Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with RTU Game.If not, see < https://www.gnu.org/licenses/>.
#include "stdafx.h"
#include "Win32Application.h"

int Win32Application::Run(IAppCore* pAppCore, HINSTANCE hInstance, int nCmdShow)
{
	// Parse the command line parameters
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	pAppCore->ParseCommandLineArgs(argv, argc);
	LocalFree(argv);

	// Initialize the window class.
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"RTUGame";
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(pAppCore->GetWidth()), static_cast<LONG>(pAppCore->GetHeight()) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	pAppCore->m_hinstance = hInstance;
	pAppCore->m_hwnd = CreateWindow(
		windowClass.lpszClassName,
		pAppCore->GetTitle(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		hInstance,
		pAppCore);

	pAppCore->OnInit();

	ShowWindow(pAppCore->m_hwnd, nCmdShow);
	UpdateWindow(pAppCore->m_hwnd);

	MSG msg = {};
	//while (msg.message != WM_QUIT)
	//{
	//	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//}
	while (msg.message != WM_QUIT)
	{
		GetMessage(&msg, nullptr, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	pAppCore->OnDestroy();

	// Return this part of the WM_QUIT message to Windows.
	return static_cast<char>(msg.wParam);
}

LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	IAppCore* pAppCore = reinterpret_cast<IAppCore*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
	}
	return 0;
	case WM_MOUSEMOVE:
		if (pAppCore)
		{
			pAppCore->OnMouseMove(LOWORD(lParam), HIWORD(lParam), static_cast<UINT8>(wParam));
		}
		return 0;

	case WM_KEYDOWN:
		if (pAppCore)
		{
			pAppCore->OnKeyDown(static_cast<UINT8>(wParam), lParam, 0);
		}
		return 0;

	case WM_SYSKEYDOWN:
		if (pAppCore)
		{
			pAppCore->OnKeyDown(static_cast<UINT8>(wParam), lParam, 1);
		}
		return 0;

	case WM_KEYUP:
		if (pAppCore)
		{
			pAppCore->OnKeyUp(static_cast<UINT8>(wParam), lParam, 0);
		}
		return 0;

	case WM_LBUTTONDOWN:
	{
		UINT x = LOWORD(lParam);
		UINT y = HIWORD(lParam);
		pAppCore->OnLeftButtonDown(x, y, wParam);
	}
	return 0;

	case WM_LBUTTONUP:
	{
		UINT x = LOWORD(lParam);
		UINT y = HIWORD(lParam);
		pAppCore->OnLeftButtonUp(x, y, wParam);
	}
	return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		if (pAppCore)
		{
		}
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_SIZE:
		if (pAppCore)
		{
			RECT windowRect = {};
			GetWindowRect(hWnd, &windowRect);
			//pAppCore->SetWindowBounds(windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);

			RECT clientRect = {};
			GetClientRect(hWnd, &clientRect);
			pAppCore->OnSizeChanged(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
