#include "Device.h"

#include <string>
#include <stdexcept>

Device::Device(HWND hWnd)
{
	CreateDeviceD3D(hWnd);
}

Device::~Device()
{
	CleanupDeviceD3D();
	ImGui_ImplDX11_Shutdown();
}

bool Device::CreateDeviceD3D(HWND hWnd, unsigned refreshRate)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = refreshRate;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;

#ifdef _DX11_ENABLE_DEBUG_LAYER
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	HRESULT res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevelArray,
		2,
		D3D11_SDK_VERSION,
		&sd,
		_swapChain.GetAddressOf(),
		_device.GetAddressOf(),
		&featureLevel,
		_deviceContext.GetAddressOf()
	);

	// Try high-performance WARP software driver if hardware is not available.
	if (res == DXGI_ERROR_UNSUPPORTED) {
		res = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_WARP,
			nullptr,
			createDeviceFlags,
			featureLevelArray,
			2,
			D3D11_SDK_VERSION,
			&sd,
			_swapChain.GetAddressOf(),
			_device.GetAddressOf(),
			&featureLevel,
			_deviceContext.GetAddressOf()
		);
	}
	if (res != S_OK) {
		throw std::runtime_error("Error D3D11CreateDeviceAndSwapChain HRESULT:" + std::to_string(res));
		return false;
	}

	CreateRenderTarget();

	return true;
}

void Device::CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (_swapChain) { _swapChain->Release(); _swapChain = nullptr; }
	if (_deviceContext) { _deviceContext->Release(); _deviceContext = nullptr; }
	if (_device) { _device->Release(); _device = nullptr; }
}

void Device::CreateRenderTarget()
{
	ID3D11Texture2D* backBuffer;
	_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	if (backBuffer == nullptr) {
		throw std::runtime_error("Error ID3D11SwapChain::GetBuffer. Back Buffer was nullptr");
		return;
	}

	HRESULT res = _device->CreateRenderTargetView(backBuffer, nullptr, &_renderTargetView);
	backBuffer->Release();

	if (res != S_OK) {
		throw std::runtime_error("Error ID3D11Device::CreateRenderTargetView HRESULT:" + std::to_string(res));
	}
}

void Device::CleanupRenderTarget()
{
	if (_renderTargetView) { _renderTargetView->Release(); _renderTargetView = nullptr; }
}

bool Device::InitializeImGuiDX11()
{
	return ImGui_ImplDX11_Init(_device.Get(), _deviceContext.Get());
}

void Device::ResizeBuffer(const Eigen::Vector2i& size)
{
	CleanupRenderTarget();
	_swapChain->ResizeBuffers(0, size.x(), size.y(), DXGI_FORMAT_UNKNOWN, 0);
	CreateRenderTarget();
}

bool Device::SwapChainOccluded()
{
	if (_swapChainOccluded && _swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
		return true;
	}
	else {
		_swapChainOccluded = false;
		return false;
	}
}

void Device::ClearRenderTargetView(const Vector4f& color)
{
	const float colorarr[4] = { color.x() * color.w(),color.y() * color.w(),color.z() * color.w(), color.w() };

	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), colorarr);
}

void Device::Present(bool vsync)
{
	HRESULT hr = _swapChain->Present(vsync ? 1 : 0, 0);   // Present with vsync
	_swapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
}
