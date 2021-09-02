#include "Shader.h"

void Shader::Init(ID3D11Device& device)
{
	BuildShader(device);
}

void Shader::SetShader(ID3D11DeviceContext& deviceContext)
{
	deviceContext.IASetInputLayout(mInputLayout.Get());

	deviceContext.VSSetShader(mVertexShader.Get(), NULL, 0);
	deviceContext.PSSetShader(mPixelShader.Get(), NULL, 0);
}

void Shader::BuildShader(ID3D11Device& device)
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3D10Blob> vertexShaderBuffer = nullptr;
	ComPtr<ID3D10Blob> errorMessage = nullptr;
	D3DCompileFromFile(L"Shader/BasicVertexShader.hlsl", NULL, NULL, "main", "vs_5_0", shaderFlags, 0, vertexShaderBuffer.GetAddressOf(), errorMessage.GetAddressOf());
	device.CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, mVertexShader.GetAddressOf());

	ComPtr<ID3D10Blob> pixelShaderBuffer = nullptr;
	D3DCompileFromFile(L"Shader/BasicPixelShader.hlsl", NULL, NULL, "main", "ps_5_0", shaderFlags, 0, pixelShaderBuffer.GetAddressOf(), errorMessage.GetAddressOf());
	device.CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, mPixelShader.GetAddressOf());

	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int numElements = sizeof(inputDesc) / sizeof(inputDesc[0]);

	device.CreateInputLayout(inputDesc, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), mInputLayout.GetAddressOf());
}