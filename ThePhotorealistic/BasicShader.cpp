#include "stdafx.h"
#include "BasicShader.h"

void BasicShader::Init(ID3D11Device& device)
{
	BuildShader(device);
}

void BasicShader::SetShader(ID3D11DeviceContext& deviceContext)
{
	deviceContext.IASetInputLayout(mInputLayout.Get());

	deviceContext.VSSetShader(mVertexShader.Get(), NULL, 0);
	deviceContext.PSSetShader(mPixelShader.Get(), NULL, 0);

	deviceContext.PSSetSamplers(0, static_cast<UINT>(mSamplerList.size()), mSamplerList.data()->GetAddressOf());
}

void BasicShader::BuildShader(ID3D11Device& device)
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3D10Blob> vertexShaderBuffer = nullptr;
	ComPtr<ID3D10Blob> errorMessage = nullptr;
	if (FAILED(D3DCompileFromFile(L"Shader/BasicVertexShader.hlsl", NULL, NULL, "main", "vs_5_0", shaderFlags, 0, vertexShaderBuffer.GetAddressOf(), errorMessage.GetAddressOf())))
	{
		if (errorMessage)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));
		}

		return;
	}
	device.CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, mVertexShader.GetAddressOf());

	ComPtr<ID3D10Blob> pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(L"Shader/BasicPixelShader.hlsl", NULL, NULL, "main", "ps_5_0", shaderFlags, 0, pixelShaderBuffer.GetAddressOf(), errorMessage.GetAddressOf())))
	{
		if (errorMessage)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));
		}

		return;
	}
	device.CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, mPixelShader.GetAddressOf());


	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int numElements = sizeof(inputDesc) / sizeof(inputDesc[0]);

	device.CreateInputLayout(inputDesc, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), mInputLayout.GetAddressOf());

	// Create sampler state.
	// point wrap sampler.
	ComPtr<ID3D11SamplerState> pointWrapSampler;
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device.CreateSamplerState(&samplerDesc, pointWrapSampler.GetAddressOf());
	mSamplerList.push_back(pointWrapSampler);

	// point clamp sampler.
	ComPtr<ID3D11SamplerState> pointClampSampler;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device.CreateSamplerState(&samplerDesc, pointClampSampler.GetAddressOf());
	mSamplerList.push_back(pointClampSampler);

	// linear wrap sampler.
	ComPtr<ID3D11SamplerState> linearWrapSampler;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	device.CreateSamplerState(&samplerDesc, linearWrapSampler.GetAddressOf());
	mSamplerList.push_back(linearWrapSampler);

	// linear clamp sampler.
	ComPtr<ID3D11SamplerState> linearClampSampler;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device.CreateSamplerState(&samplerDesc, linearClampSampler.GetAddressOf());
	mSamplerList.push_back(linearClampSampler);

	// anisotropic wrap sampler.
	ComPtr<ID3D11SamplerState> anisotropicWrapSampler;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	device.CreateSamplerState(&samplerDesc, anisotropicWrapSampler.GetAddressOf());
	mSamplerList.push_back(anisotropicWrapSampler);

	// anisotropic clamp sampler.
	ComPtr<ID3D11SamplerState> anisotropicClampSampler;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device.CreateSamplerState(&samplerDesc, anisotropicClampSampler.GetAddressOf());
	mSamplerList.push_back(anisotropicClampSampler);
}