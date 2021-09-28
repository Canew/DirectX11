#pragma once

#include "Object.h"

class Cylinder : public Object
{
public:
	Cylinder(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount);
	virtual ~Cylinder() = default;
};

