#include "Window.h"

#include <stdexcept>

Vector2i Window::_resize({ 0,0 });

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
		L"Dear ImGui DirectX11 Example", 
		WS_OVERLAPPEDWINDOW, 
		100, 100, 
		width, height, 
		nullptr, nullptr, _wc.hInstance, nullptr
	);

	_device = std::make_unique<Device>(_hWnd);

	/*
	if (!CreateDeviceD3D()) {
		throw std::runtime_error("could not create d3d device");
		UnregisterClassW(_wc.lpszClassName, _wc.hInstance);
		return;
	}*/

	ShowWindow(_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(_hWnd);

	// start imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(_hWnd);

	_device->InitializeImGuiDX11(); // ImGui_ImplDX11_Init()

}

Window::~Window()
{
	_device.release();

	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	
	DestroyWindow(_hWnd);
	UnregisterClassW(_wc.lpszClassName, _wc.hInstance);
}

void Window::HandleMessages()
{
	// poll and handle messages (input, window resize, etc.)

	MSG msg;

	while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) {
			_shouldClose = true;
		}
	}
}

bool Window::SwapChainReady()
{
	return !_device->SwapChainOccluded(); // i know, inverting makes it slightly confusing. didn't want to name it Window::SwapChainOccluded
}

void Window::BeginFrame()
{


	if (_resize.x() != 0 && _resize.y() != 0) {
		_device->ResizeBuffer(_resize);
		_resize.x() = _resize.y() = 0;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Window::EndFrame(bool vsync)
{
	ImGui::Render();

	Vector4f clear_color({ 0.45f, 0.55f, 0.60f, 1.00f });
	_device->ClearRenderTargetView(clear_color);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present
	_device->Present(vsync);
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
		Window::_resize.x() = (UINT)LOWORD(lParam); // Queue resize
		Window::_resize.y() = (UINT)HIWORD(lParam);
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


