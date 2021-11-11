#pragma once

#include "D3DUtil.h"

class ShadowMap
{
public:
	ShadowMap(ID3D11Device& device, UINT width, UINT height);
	virtual ~ShadowMap() = default;
	ShadowMap(const ShadowMap& rhs) = delete;
	ShadowMap& operator=(const ShadowMap& rhs) = delete;

	void Render(ID3D11DeviceContext& deviceContext, std::vector<std::unique_ptr<class Object>>& objects);

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext& deviceContext);

	ComPtr<ID3D11ShaderResourceView> GetDepthMapSRV();
	class Light* GetLightReference();
	void SetLightReference(class Light* lightReference);

private:
	UINT mWidth;
	UINT mHeight;

	ComPtr<ID3D11ShaderResourceView> mDepthMapSRV;
	ComPtr<ID3D11DepthStencilView> mDepthMapDSV;
	D3D11_VIEWPORT mViewport;

	class Light* mLightReference = nullptr;
};
