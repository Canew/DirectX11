#pragma once

#include "Object.h"

class Geosphere : public Object
{
public:
	Geosphere(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float radius, UINT numSubdivisions);
	virtual ~Geosphere() = default;
};

