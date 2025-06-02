#include "Application.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "TransformGUI.h"

Application::Application()
	: _window(std::make_unique<Window>(L"ImMandel", 640, 480))
	, _d3d(std::make_unique<D3D>(_window->get_handle()))
	, _gui(std::make_unique<GUI>(_d3d->get_device(), _d3d->get_context(), _window->get_handle()))
	, _transform(std::make_unique<Transform<float>>(_window->get_size()))
	, _engine(std::make_unique<EscapeTimeEngine<float>>())
	, _greyscale_shader(std::make_unique<Shader<GreyscaleParameters>>(_d3d->get_device(), L"res/shaders/greyscale_vs.hlsl", L"res/shaders/greyscale_ps.hlsl")) 

{
	_gui->register_drawable(std::make_unique<TransformGUI<float>>(*_transform.get()));
}

Application::~Application()
{
}

int Application::run()
{
	_greyscale_shader->parameters().max_it = 512; // example value
	_greyscale_shader->update(_d3d->get_context());

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
			_transform->set_size(Window::resize);
			Window::resize.x() = Window::resize.y() = 0;
		}


		_gui->new_frame();
		//_gui->show_demo_window();

		if (_transform->changed()) {
			Surface<uint32_t> buffer = _d3d->map();

			_engine->compute(*_transform.get(), buffer, _greyscale_shader->parameters().max_it);

			_d3d->unmap();
		}


		_greyscale_shader->bind(_d3d->get_context());
		
		_d3d->render();

		_gui->render();
		_d3d->present();
	}

	return 0;
}

