#pragma once

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include <Windows.h>
#include <wrl/client.h>
#include <Eigen/Dense>

#include <string>

#include "MappedRegion.h"

using Microsoft::WRL::ComPtr;
using Eigen::Vector2i;

class D3D {
private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGIFactory2> _dxgi_factory;
	ComPtr<IDXGISwapChain1> _swap_chain;
	ComPtr<ID3D11Texture2D> _back_buffer;
	ComPtr<ID3D11RenderTargetView> _rtv;
	ComPtr<ID3D11ShaderResourceView> _srv;
	ComPtr<ID3D11Texture2D> _dynamic_buffer; // for writing to the CPU as swap chain back buffer cannot be mapped

	D3D11_MAPPED_SUBRESOURCE _mapped = {};

private:
	bool _swap_chain_occluded = false;

public:
	D3D(HWND window_handle);
	~D3D();

public:
	// clear render target view to back
	void clear_rtv();

	void copy_dynamic_to_back_buffer();

	// present swap chain and check whether its occluded
	void present(bool vsync = true);

	// resize swapchain, create new render target and texture, new SRV
	void resize(const Vector2i& size);

	// creates a mapped memory region to the dynamic buffer
	MappedRegion<uint32_t> map();

	// unmaps the dynamic buffer
	void unmap();


public:
	bool occluded() const { return _swap_chain_occluded; };

	ID3D11Device* get_device() const { return _device.Get(); }
	ID3D11DeviceContext* get_context() const { return _context.Get(); }
};