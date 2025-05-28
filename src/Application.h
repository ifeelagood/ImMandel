#pragma once

#include <memory>

#include "D3D.h"
#include "Window.h"
#include "GUI.h"


class Application {
private:
	// C4.7 Define and initialize data members in the order of member declaration
	std::unique_ptr<Window> _window;
	std::unique_ptr<D3D> _d3d;
	std::unique_ptr<GUI> _gui;
	

public:
	Application();
	~Application();

	int run();
};