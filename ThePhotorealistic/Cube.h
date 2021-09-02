#pragma once

#include "Object.h"

class Cube : public Object
{
	virtual void BuildGeometryBuffers(ID3D11Device& device) override;
};
