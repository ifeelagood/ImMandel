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

	if (!CreateDeviceD3D()) {
		throw std::runtime_error("could not create d3d device");
		UnregisterClassW(_wc.lpszClassName, _wc.hInstance);
		return;
	}

	ShowWindow(_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(_hWnd);

	// start imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(_hWnd);
	ImGui_ImplDX11_Init(_device, _deviceContext);
}

Window::~Window()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(_hWnd);
	UnregisterClassW(_wc.lpszClassName, _wc.hInstance);
}

void Window::BeginFrame()
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

	/*
	if (_swapChainOccluded && _swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
		Sleep(10);
		continue;
	}
	_swapChainOccluded = false;
	*/

	if (_resize.x() != 0 && _resize.y() != 0) {
		CleanupRenderTarget();
		_swapChain->ResizeBuffers(0, _resize.x(), _resize.y(), DXGI_FORMAT_UNKNOWN, 0);
		_resize.x() = _resize.y() = 0;
		CreateRenderTarget();
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}

void Window::EndFrame(bool vsync)
{
	ImGui::Render();

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	_deviceContext->OMSetRenderTargets(1, &_renderTargetView, nullptr);
	_deviceContext->ClearRenderTargetView(_renderTargetView, clear_color_with_alpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present
	HRESULT hr = _swapChain->Present(vsync ? 1 : 0, 0);   // Present with vsync
	_swapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
}

bool Window::CreateDeviceD3D()
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;

#ifdef _DX11_ENABLE_DEBUG_LAYER
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &_swapChain, &_device, &featureLevel, &_deviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &_swapChain, &_device, &featureLevel, &_deviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();

	return true;
}

void Window::CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (_swapChain) { _swapChain->Release(); _swapChain = nullptr; }
	if (_deviceContext) { _deviceContext->Release(); _deviceContext = nullptr; }
	if (_device) { _device->Release(); _device = nullptr; }
}

void Window::CreateRenderTarget()
{
	ID3D11Texture2D* backBuffer;
	_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	_device->CreateRenderTargetView(backBuffer, nullptr, &_renderTargetView);
	backBuffer->Release();
}

void Window::CleanupRenderTarget()
{
	if (_renderTargetView) { _renderTargetView->Release(); _renderTargetView = nullptr; }
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


