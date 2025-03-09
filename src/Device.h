#pragma once

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <Windows.h>
#include <wrl/client.h>
#include <Eigen/Dense>

#include <string>

#ifdef _DEBUG
#define DX11_ENABLE_DEBUG_LAYER
#endif

#ifdef DX11_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

using Microsoft::WRL::ComPtr;
using Eigen::Vector2i;
using Eigen::Vector4f;

class Device {
private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _deviceContext;
	ComPtr<IDXGISwapChain> _swapChain;
	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	bool _swapChainOccluded = false;

private:
	bool CreateDeviceD3D(HWND hWnd, unsigned refreshRate = 165);

	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();

public:
	Device(HWND hWnd);
	~Device();

public:
	bool InitializeImGuiDX11();
	
	void ResizeBuffer(const Vector2i& size);

	bool SwapChainOccluded();
	void ClearRenderTargetView(const Vector4f& color);
	void Present(bool vsync = true);
};