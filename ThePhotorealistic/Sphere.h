#pragma once

#include "Object.h"

class Sphere : public Object
{
public:
	Sphere(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float radius, UINT sliceCount, UINT stackCount);
	virtual ~Sphere() = default;
};

