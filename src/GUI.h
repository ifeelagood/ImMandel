#pragma once

#include <d3d11.h>
#include <Windows.h>

#include <memory>
#include <vector>
#include <functional>

#include "ImGuiDrawable.h"

class GUI {
private:
	bool _show_demo_window = true;

	std::vector<std::unique_ptr<ImGuiDrawable>> _draw_list;

public:
	GUI(ID3D11Device* device, ID3D11DeviceContext* context, HWND window_handle);
	~GUI();

public:
	// new imgui frame
	void new_frame();

	// draws all widgets and render imgui data to render target view.
	void render();

	void register_drawable(std::unique_ptr<ImGuiDrawable> drawable);

	void show_demo_window();
};