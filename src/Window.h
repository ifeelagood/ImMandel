#pragma once

#include <memory>


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
using Eigen::Vector4f;

class Window {
private:
	WNDCLASSEXW _wc;
	HWND _hWnd;
	bool _should_close = false;

private:
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	Window(LPCWSTR title, unsigned width, unsigned height);
	~Window();

public:
	// when window is resized, new size is stored in here.
	static Vector2i resize;

public:
	// poll and handle messages (input, window resize, etc.). must be called start of each frame.
	void handle_messages();


public:
	bool should_close() const { return _should_close; }
	HWND get_handle() const { return _hWnd;  }
};