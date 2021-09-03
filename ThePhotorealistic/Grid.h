#pragma once

#include "Object.h"

class Grid : public Object
{
public:
	Grid(ID3D11Device& device, float width, float depth, UINT m, UINT n);
	virtual ~Grid() = default;

protected:
	virtual void BuildGeometryBuffers(ID3D11Device& device, float width, float depth, UINT m, UINT n);

private:
	float GetHeight(float x, float z) const;
};
	