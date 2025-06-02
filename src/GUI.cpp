#include "GUI.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

GUI::GUI(ID3D11Device* device, ID3D11DeviceContext* context, HWND window_handle)
{
	// start imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window_handle);

	ImGui_ImplDX11_Init(device, context);
}

GUI::~GUI() {
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GUI::new_frame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void GUI::render()
{
	for (auto& drawable : _draw_list) {
		drawable->draw();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GUI::register_drawable(std::unique_ptr<ImGuiDrawable> drawable)
{
	_draw_list.emplace_back(std::move(drawable));
}

void GUI::show_demo_window()
{
	ImGui::ShowDemoWindow(&_show_demo_window);
}
