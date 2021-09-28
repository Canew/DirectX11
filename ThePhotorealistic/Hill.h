#pragma once

#include "Object.h"

class Hill : public Object
{
public:
	Hill(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float width, float depth, UINT m, UINT n);
	virtual ~Hill() = default;

private:
	float GetHeight(float x, float z) const;
};
