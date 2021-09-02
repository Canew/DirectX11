#pragma once

#include "D3DUtil.h"

class Shader
{
public:
	void Init(ID3D11Device& device);
	void SetShader(ID3D11DeviceContext& deviceContext);

private:
	void BuildShader(ID3D11Device& device);

private:
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader> mPixelShader;
	ComPtr<ID3D11InputLayout> mInputLayout;
};
