#pragma once

#include "D3DUtil.h"

class DynamicCubeMap
{
public:
	DynamicCubeMap(ID3D11Device& device);

private:
	void BuildCubeFaceCamera(float x, float y, float z);
	void DrawScene(ID3D11DeviceContext& deviceContext);

private:
	static const int CubeMapSize = 256;

	ComPtr<ID3D11RenderTargetView> mDynamicCubeMapRTV[6];
	ComPtr<ID3D11ShaderResourceView> mDynamicCubeMapSRV;
	ComPtr<ID3D11DepthStencilView> mDynamicCubeMapDSV;
	D3D11_VIEWPORT mCubeMapViewport;

	std::unique_ptr<class Camera> mCubeMapCamera[6];
};

