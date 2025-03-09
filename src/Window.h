#pragma once

#include <memory>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <Windows.h>
#include <Eigen/Dense>

#include "Device.h"

#ifdef _DEBUG
#define DX11_ENABLE_DEBUG_LAYER
#endif

#ifdef DX11_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

using Eigen::Vector2i;
using Eigen::Vector4f;

class Window {
private:
	std::unique_ptr<Device> _device;
private:
	WNDCLASSEXW _wc;
	HWND _hWnd;
	bool _shouldClose = false;

private:
	static Vector2i _resize;

private:
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	Window(LPCWSTR title, unsigned width, unsigned height);
	~Window();

public:
	void HandleMessages();
	bool SwapChainReady();
	void BeginFrame();
	void EndFrame(bool vsync = true);

public:
	bool ShouldClose() const { return _shouldClose; }
};