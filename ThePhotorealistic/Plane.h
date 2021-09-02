#pragma once

#include "Object.h"

class Plane : public Object
{
protected:
	virtual void BuildGeometryBuffers(ID3D11Device& device) override;
};
