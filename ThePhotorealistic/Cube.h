#pragma once

#include "Object.h"

class Cube : public Object
{
public:
	Cube(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float width, float height, float depth);
	virtual ~Cube() = default;
};
