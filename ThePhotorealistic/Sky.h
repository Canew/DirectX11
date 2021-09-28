#pragma once

#include "D3DUtil.h"
#include "Object.h"

class Sky : public Object
{
public:
	Sky(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float skySphereRadius);
	virtual ~Sky() = default;

	void Render(ID3D11DeviceContext& deviceContext) override;

private:
	std::vector<XMFLOAT3> mPositions;
};
