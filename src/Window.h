#pragma once

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <Windows.h>
#include <Eigen/Dense>

#ifdef _DEBUG
#define DX11_ENABLE_DEBUG_LAYER
#endif

#ifdef DX11_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

using Eigen::Vector2i;


class Window {
private:
	WNDCLASSEXW _wc;
	HWND _hWnd;

private:
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;
	IDXGISwapChain* _swapChain;
	ID3D11RenderTargetView* _renderTargetView;
	bool _swapChainOccluded = false;
	
	

	bool _shouldClose = false;

private:
	bool CreateDeviceD3D();
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();

private:
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	Window(LPCWSTR title, unsigned width, unsigned height);
	~Window();

public:
	void BeginFrame();
	void EndFrame(bool vsync = true);

public:
	bool ShouldClose() const { return _shouldClose; }

public:
	static Vector2i _resize;
};