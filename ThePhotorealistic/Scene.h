#pragma once

#include "D3DUtil.h"
#include <vector>
#include <memory>

class Scene
{
	struct Light
	{
		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT3 Strength;
		float FalloffStart;           // point/spot light only
		XMFLOAT3 Direction;    // directional/spot light only
		float FalloffEnd;            // point/spot light only
		XMFLOAT3 Position;     // point light only
		float SpotPower;          // spot light only
	};
	struct LightingPass
	{
		XMFLOAT3 EyePosition;
		float Pad1;

		Light Light;

		float FogStart;
		float FogRange;
		XMFLOAT2 Pad2;
		XMFLOAT4 FogColor;
	};
public:
	Scene(ID3D11Device& device, ID3D11DeviceContext& deviceContext);
	virtual ~Scene() = default;

	virtual void Update(ID3D11DeviceContext& deviceContext, float dt);
	virtual void Render(ID3D11DeviceContext& deviceContext);

private:
	virtual void CreateConstantBuffer(ID3D11Device& device);
	virtual void UpdateConstantBuffer(ID3D11DeviceContext& deviceContext);

private:
	std::vector<std::unique_ptr<class Object>> mObjectList;
	std::vector<std::unique_ptr<Light>> mLightList;
	std::unique_ptr<class Sky> mSky;

	ComPtr<ID3D11Buffer> mPassBuffer = nullptr;

	ID3D11Device& mDevice;
	ID3D11DeviceContext& mDeviceContext;
};