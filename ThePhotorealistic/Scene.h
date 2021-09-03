#pragma once

#include "D3DUtil.h"
#include <vector>
#include <memory>

class Scene
{
	struct Light
	{
		XMFLOAT3 Strength;
		float FalloffStart;           // point/spot light only
		XMFLOAT3 Direction;    // directional/spot light only
		float FalloffEnd;            // point/spot light only
		XMFLOAT3 Position;     // point light only
		float SpotPower;          // spot light only
	};
	struct Pass
	{
		XMFLOAT4 AmbientLight;
		XMFLOAT3 EyePosition;
		float Pad;

		Light Lights[16];
	};
public:
	Scene(ID3D11Device& device);
	virtual ~Scene() = default;

	virtual void Render(ID3D11DeviceContext& deviceContext);

private:
	virtual void CreateConstantBuffer(ID3D11Device& device);
	virtual void SetConstantBuffer(ID3D11DeviceContext& deviceContext);

private:
	std::vector<std::unique_ptr<class Object>> mObjectList;
	std::vector<std::unique_ptr<Light>> mLightList;

	ComPtr<ID3D11Buffer> mPassBuffer = nullptr;
};