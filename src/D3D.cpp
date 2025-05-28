#include "D3D.h"

#include <string>
#include <stdexcept>

D3D::D3D(HWND window_handle)
{
	HRESULT hr; // for all results

	// create device and context
	UINT device_flags = 0;

#ifdef _DX11_ENABLE_DEBUG_LAYER
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	constexpr const D3D_FEATURE_LEVEL features[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		device_flags,
		features,
		sizeof(features) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		_device.GetAddressOf(),
		nullptr,
		_context.GetAddressOf()
	);
	if (FAILED(hr)) {
		throw std::runtime_error("Could not create directx11 device and context");
	}

	// create dxgi factory
	UINT dxgi_flags = 0;

#ifdef _DX11_ENABLE_DEBUG_LAYER
	dxgi_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hr = CreateDXGIFactory2(dxgi_flags, IID_PPV_ARGS(_dxgi_factory.GetAddressOf()));
	if (FAILED(hr)) {
		throw std::runtime_error("Could not create DXGI factory");
	}


	// create swap chain
	DXGI_SWAP_CHAIN_DESC1 sc_desc = {};
	sc_desc.Width = 0;
	sc_desc.Height = 0;
	sc_desc.Stereo = false;
	sc_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	sc_desc.BufferCount = 2;
	sc_desc.SampleDesc.Count = 1;
	sc_desc.SampleDesc.Quality = 0;
	sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; 
	sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fs_desc;
	fs_desc.Windowed = true;
	

	hr = _dxgi_factory->CreateSwapChainForHwnd(_device.Get(), window_handle, &sc_desc, &fs_desc, nullptr, _swap_chain.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("Could not create swap chain for window handle");
	}

	// resize
	RECT rect;
	if (GetWindowRect(window_handle, &rect)) {
		Vector2i window_size = { rect.right - rect.left, rect.bottom - rect.top };
		resize(window_size);
	} else {
		throw std::runtime_error("Initial query of window size failed");
	}
}

D3D::~D3D()
{
}


void D3D::render() {


	// clear RTV
	const float color[4] = { 0.f,0.f,0.f,0.f };
	_context->ClearRenderTargetView(_rtv.Get(), color);

	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	_context->PSSetShaderResources(0, 1, _srv.GetAddressOf());

	_context->Draw(3, 0); // draw fullscreen triangle


	//_context->CopyResource(_dynamic_buffer.Get(), _back_buffer.Get());

}



void D3D::present(bool vsync)
{
	DXGI_PRESENT_PARAMETERS params = {};
	HRESULT hr = _swap_chain->Present1(vsync ? 1 : 0, 0, &params);   // present with vsync
	_swap_chain_occluded = (hr == DXGI_STATUS_OCCLUDED);
}

void D3D::resize(const Vector2i& size)
{
	// delete rtv, src, back buffer, dynamic texture buffer
	_rtv.Reset();
	_srv.Reset();
	_back_buffer.Reset();
	_dynamic_buffer.Reset();

	// resize swapchain buffers
	_swap_chain->ResizeBuffers(0, size.x(), size.y(), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	// reaquire back buffer
	_swap_chain->GetBuffer(0, IID_PPV_ARGS(_back_buffer.GetAddressOf()));

	// create rtv for back buffer
	_device->CreateRenderTargetView(_back_buffer.Get(), nullptr, _rtv.GetAddressOf());

	// set render target view
	_context->OMSetRenderTargets(1, _rtv.GetAddressOf(), nullptr);

	HRESULT hr;

	// create dynamic buffer texture
	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = size.x();
	tex_desc.Height = size.y();
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R32_UINT; 
	tex_desc.SampleDesc.Count = 1;
	tex_desc.Usage = D3D11_USAGE_DYNAMIC; // allows for multiple threads to write to mapped data
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
	tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = _device->CreateTexture2D(&tex_desc, nullptr, _dynamic_buffer.GetAddressOf());

	if (FAILED(hr)) {
		throw std::runtime_error("could not create dynamic buffer texture");
	}

	// create srv for the dynamic buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Format = tex_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;

	_device->CreateShaderResourceView(_dynamic_buffer.Get(), &srv_desc, _srv.GetAddressOf());

	// set viewport
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(size.x());
	viewport.Height = static_cast<float>(size.y());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	_context->RSSetViewports(1, &viewport);

}

std::span<uint32_t> D3D::map() {
	_context->Map(_dynamic_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &_mapped);

	D3D11_TEXTURE2D_DESC tex_desc;
	_dynamic_buffer->GetDesc(&tex_desc);

	return std::span<uint32_t>(reinterpret_cast<uint32_t*>(_mapped.pData), tex_desc.Width * tex_desc.Height);

}

void D3D::unmap() {
	_context->Unmap(_dynamic_buffer.Get(), 0);
}