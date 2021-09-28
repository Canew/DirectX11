#pragma once

#include "Object.h"

class Grid : public Object
{
public:
	Grid(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float width, float depth, UINT m, UINT n);
	virtual ~Grid() = default;

private:
	float GetHeight(float x, float z) const;
};
