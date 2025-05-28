#include "Application.h"



Application::Application()
	: _window(std::make_unique<Window>(L"ImMandel", 1920, 1080))
	, _d3d(std::make_unique<D3D>(_window->get_handle()))
	, _gui(std::make_unique<GUI>(_d3d->get_device(), _d3d->get_context(), _window->get_handle()))

{
}

Application::~Application()
{
}

int Application::run()
{
	while (!_window->should_close()) {
		_window->handle_messages();

		// wait until swap chain is not occluded
		if (_d3d->occluded()) {
			Sleep(10);
			continue;
		}

		// if window has changed size, update 
		if (Window::resize.x() != 0 && Window::resize.y() != 0) {
			_d3d->resize(Window::resize);
			Window::resize.x() = Window::resize.y() = 0;
		}


		_gui->new_frame();
		_gui->show_demo_window();
		
		_d3d->copy_dynamic_to_back_buffer();
		_d3d->clear_rtv();

		_gui->render();
		_d3d->present();
	}

	return 0;
}

