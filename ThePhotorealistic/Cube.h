#pragma once

#include "Object.h"

class Cube : public Object
{
public:
	Cube(ID3D11Device& device, float width, float height, float depth);
	virtual ~Cube() = default;

protected:
	virtual void BuildGeometryBuffers(ID3D11Device& device, float width, float height, float depth);
};
