#pragma once

#include <d3d11.h>
#include <Windows.h>

class GUI {
private:
	bool _show_demo_window = true;

public:
	GUI(ID3D11Device* device, ID3D11DeviceContext* context, HWND window_handle);
	~GUI();

public:
	// new imgui frame
	void new_frame();

	// render imgui data to render target view.
	void render();

	void show_demo_window();
};