#pragma once

#include "Object.h"

class Plane : public Object
{
public:
	Plane(ID3D11Device& device, ID3D11DeviceContext& deviceContext);
	virtual ~Plane() = default;
};
