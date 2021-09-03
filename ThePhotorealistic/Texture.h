#pragma once

#include "D3DUtil.h"

class Texture
{
public:
	Texture(ID3D11Device& device);
	Texture(ID3D11Device& device, const WCHAR* filename);
	virtual ~Texture() = default;

	ComPtr<ID3D11ShaderResourceView> GetShaderResourceView();

private:
	void Init(ID3D11Device& device, const WCHAR* filename);

private:
	ComPtr<ID3D11ShaderResourceView> mTextureView = nullptr;
	int mTextureWidth;
	int mTextureHeight;
};
