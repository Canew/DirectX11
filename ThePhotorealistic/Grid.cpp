#include "Grid.h"

Grid::Grid(ID3D11Device& device, float width, float depth, UINT m, UINT n) : Object(device)
{
	BuildGeometryBuffers(device, width, depth, m, n);
}

void Grid::BuildGeometryBuffers(ID3D11Device& device, float width, float depth, UINT m, UINT n)
{
	UINT mVertexCount = m * n;
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

	mVertices.resize(mVertexCount);
	for (UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			mVertices[i * n + j].Position = XMFLOAT3(x, 0.0f, z);
			mVertices[i * n + j].Position.y = GetHeight(x, z);
			mVertices[i * n + j].Normal = XMFLOAT3(
				-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
				1.0f,
				-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

			// Stretch texture over grid.
			mVertices[i * n + j].TexC.x = j * du;
			mVertices[i * n + j].TexC.y = i * dv;
		}
	}

	//
	// Create the indices.
	//

	mIndices.resize(faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	UINT k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (UINT j = 0; j < n - 1; ++j)
		{
			mIndices[k] = i * n + j;
			mIndices[k + 1] = i * n + j + 1;
			mIndices[k + 2] = (i + 1) * n + j;

			mIndices[k + 3] = (i + 1) * n + j;
			mIndices[k + 4] = i * n + j + 1;
			mIndices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}
	mIndexCount = static_cast<unsigned int>(mIndices.size());

    // Vertex Buffer
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * mVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData = {};
	vinitData.pSysMem = mVertices.data();

	device.CreateBuffer(&vbd, &vinitData, mVertexBuffer.GetAddressOf());

    // Index Buffer
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = mIndices.data();

	device.CreateBuffer(&ibd, &iinitData, mIndexBuffer.GetAddressOf());
}

float Grid::GetHeight(float x, float z) const
{
	return 0.1f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}
