#include <iostream>

#include <imgui.h>

#include "Window.h"

int main(int argc, char* argv[]) {

	Window window(L"mandel", 1920, 1080);

	bool show_demo_window = true;

	while (!window.ShouldClose()) {
		window.HandleMessages();

		if (!window.SwapChainReady()) {
			Sleep(10);
			continue;
		}

		window.BeginFrame();

		ImGui::ShowDemoWindow(&show_demo_window);

		window.EndFrame();
	}

	return EXIT_SUCCESS;
}