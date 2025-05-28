#include "Window.h"

#include <stdexcept>
#include <imgui_impl_win32.h>

Vector2i Window::resize({ 0,0 });

Window::Window(LPCWSTR title, unsigned width, unsigned height)  {
	_wc = {
		sizeof(_wc),
		CS_CLASSDC,
		Window::WndProc,
		0,
		0,
		GetModuleHandle(nullptr),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		L"ImGui_DX11",
		nullptr 
	};
	RegisterClassExW(&_wc);

	_hWnd = CreateWindowW(
		_wc.lpszClassName, 
		title,
		WS_OVERLAPPEDWINDOW, 
		100, 100, 
		width, height, 
		nullptr, nullptr, _wc.hInstance, nullptr
	);


	ShowWindow(_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(_hWnd);
}

Window::~Window()
{
	DestroyWindow(_hWnd);
	UnregisterClassW(_wc.lpszClassName, _wc.hInstance);
}

void Window::handle_messages()
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) {
			_should_close = true;
		}
	}
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		Window::resize.x() = (UINT)LOWORD(lParam); // Queue resize
		Window::resize.y() = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


