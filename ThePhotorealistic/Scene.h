#pragma once

#include "D3DUtil.h"
#include <vector>
#include <memory>
#include "Light.h"

class Scene
{
	struct FrameMatrix
	{
		XMMATRIX View;
		XMMATRIX Projection;
	};
	struct LightingPass
	{
		XMFLOAT3 EyePosition;
		float Pad1;

		XMFLOAT4 LightAmbient;
		XMFLOAT4 LightDiffuse;
		XMFLOAT3 LightIntensity;
		float Pad2;
		XMFLOAT3 LightDirection;
		float Pad3;
		XMFLOAT3 LightPosition;
		float Pad4;

		float FogStart;
		float FogRange;
		XMFLOAT2 Pad5;
		XMFLOAT4 FogColor;
	};
public:
	Scene(ID3D11Device& device, ID3D11DeviceContext& deviceContext);
	virtual ~Scene() = default;

	virtual void Update(ID3D11DeviceContext& deviceContext, float dt);
	virtual void Render(ID3D11DeviceContext& deviceContext);

	virtual void SetSceneRTV(ID3D11RenderTargetView* sceneRTV) { mSceneRTV = sceneRTV; }
	virtual void SetSceneDSV(ID3D11DepthStencilView* sceneDSV) { mSceneDSV = sceneDSV; }
	virtual void SetSceneViewport(D3D11_VIEWPORT sceneViewport) { mSceneViewport = sceneViewport; }

private:
	virtual void CreateConstantBuffer(ID3D11Device& device);
	virtual void UpdateConstantBuffer(ID3D11DeviceContext& deviceContext, XMMATRIX viewMatrix);

	virtual void BindRtvAndDsv(ID3D11DeviceContext& deviceContext);

private:
	std::vector<std::unique_ptr<class Object>> mObjectList;
	std::vector<std::unique_ptr<Light>> mLightList;
	std::unique_ptr<class Sky> mSky;
	std::unique_ptr<class Fog> mFog;

	std::unique_ptr<class ShadowMap> mShadowMap;

	ComPtr<ID3D11Buffer> mFrameBuffer = nullptr;
	ComPtr<ID3D11Buffer> mPassBuffer = nullptr;

	ID3D11Device& mDevice;
	ID3D11DeviceContext& mDeviceContext;

	ID3D11RenderTargetView* mSceneRTV;
	ID3D11DepthStencilView* mSceneDSV;
	D3D11_VIEWPORT mSceneViewport;
};