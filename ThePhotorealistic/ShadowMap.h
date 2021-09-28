#pragma once

#include "D3DUtil.h"

class ShadowMap
{
public:
	ShadowMap(ID3D11Device& device, UINT width, UINT height);
	~ShadowMap() = default;
	ShadowMap(const ShadowMap& rhs) = delete;
	ShadowMap& operator=(const ShadowMap& rhs) = delete;

	ComPtr<ID3D11ShaderResourceView> DepthMapSRV();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext& deviceContext);

private:
	UINT mWidth;
	UINT mHeight;

	ComPtr<ID3D11ShaderResourceView> mDepthMapSRV;
	ComPtr<ID3D11DepthStencilView> mDepthMapDSV;

	D3D11_VIEWPORT mViewport;
};
