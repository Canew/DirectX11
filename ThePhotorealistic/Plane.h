#pragma once

#include "Object.h"

class Plane : public Object
{
public:
	Plane(ID3D11Device& device);
	virtual ~Plane() = default;
};
