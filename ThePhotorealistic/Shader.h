#pragma once

#include "D3DUtil.h"
#include <vector>
#include <string>

class Shader abstract
{
protected:
	Shader() = default;

public:
	virtual void Init(ID3D11Device& device);

	virtual void SetShader(ID3D11DeviceContext& deviceContext);

protected:
	virtual void BuildShader(ID3D11Device& device);

protected:
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader> mPixelShader;
	ComPtr<ID3D11InputLayout> mInputLayout;
};
