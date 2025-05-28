#pragma once 

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

#include <string>
#include <stdexcept>

using Microsoft::WRL::ComPtr;

template <typename TParams>
class Shader {
private:
	ComPtr<ID3D11VertexShader> _vs;
	ComPtr<ID3D11PixelShader> _ps;
	ComPtr<ID3D11Buffer> _param_buffer;
	TParams _params{};

public:
	Shader(ID3D11Device* device, const std::wstring& vs_file, const std::wstring& ps_file, const std::string& vs_entry = "main", const std::string& ps_entry = "main");

	// update parameter buffer
	void update(ID3D11DeviceContext* context);
	
	// bind shader and parameter buffer
	void bind(ID3D11DeviceContext* context, UINT slot = 0);

	// setter for parameters
	TParams& parameters() {	return _params; };
};

template<typename TParams>
inline Shader<TParams>::Shader(ID3D11Device* device, const std::wstring& vs_file, const std::wstring& ps_file, const std::string& vs_entry, const std::string& ps_entry)
{
	// compile shaders
	ComPtr<ID3DBlob> vs_blob, ps_blob;
	D3DCompileFromFile(vs_file.c_str(), nullptr, nullptr, vs_entry.c_str(), "vs_5_0", 0, 0, vs_blob.GetAddressOf(), nullptr);
	D3DCompileFromFile(ps_file.c_str(), nullptr, nullptr, ps_entry.c_str(), "ps_5_0", 0, 0, ps_blob.GetAddressOf(), nullptr);

	// create shaders
	device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, _vs.GetAddressOf());
	device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, _ps.GetAddressOf());

	// create parameter buffer
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(TParams);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT hr = device->CreateBuffer(&desc, nullptr, _param_buffer.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("could not create shader parameter buffer");
	}
}

template<typename TParams>
inline void Shader<TParams>::update(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	context->Map(_param_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &_params, sizeof(TParams));
	context->Unmap(_param_buffer.Get(), 0);
}

template<typename TParams>
inline void Shader<TParams>::bind(ID3D11DeviceContext* context, UINT slot)
{
	context->VSSetShader(_vs.Get(), nullptr, 0);
	context->PSSetShader(_ps.Get(), nullptr, 0);
	context->PSSetConstantBuffers(slot, 1, _param_buffer.GetAddressOf());
}
