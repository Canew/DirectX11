#include "stdafx.h"
#include "Grid.h"

Grid::Grid(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float width, float depth, UINT m, UINT n)
	: Object(device, deviceContext)
{
	UINT mVerTexCoordount = m * n;
	UINT faceCount = (m - 1) * (n - 1) * 2;

	//
	// Create the vertices.
	//
	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	std::vector<Vertex>vertices(mVerTexCoordount);
	for (UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			vertices[i * n + j].Position = XMFLOAT3(x, 0.0f, z);
			vertices[i * n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[i * n + j].Tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

			// Stretch texture over grid.
			vertices[i * n + j].TexCoord.x = j * du;
			vertices[i * n + j].TexCoord.y = i * dv;
		}
	}

	//
	// Create the indices.
	//

	std::vector<unsigned int> indices(faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	UINT k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (UINT j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	mMeshes.push_back(Mesh(device, deviceContext, vertices, indices));
}

float Grid::GetHeight(float x, float z) const
{
	return 0.1f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}
