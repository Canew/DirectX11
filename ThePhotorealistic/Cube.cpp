#include "Cube.h"

Cube::Cube(ID3D11Device& device, float width, float height, float depth) : Object(device)
{
	BuildGeometryBuffers(device, width, height, depth);
}

void Cube::BuildGeometryBuffers(ID3D11Device& device, float width, float height, float depth)
{
	// Create vertex buffer

	mVertices.resize(24);

	float halfWidth = 0.5f * width;
	float halfHeight = 0.5f * height;
	float halfDepth = 0.5f * depth;

	// Fill in the front face vertex data.
	mVertices[0] = Vertex(-halfWidth, -halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	mVertices[1] = Vertex(-halfWidth, +halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	mVertices[2] = Vertex(+halfWidth, +halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	mVertices[3] = Vertex(+halfWidth, -halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	mVertices[4] = Vertex(-halfWidth, -halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	mVertices[5] = Vertex(+halfWidth, -halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	mVertices[6] = Vertex(+halfWidth, +halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	mVertices[7] = Vertex(-halfWidth, +halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	mVertices[8] = Vertex(-halfWidth, +halfHeight, -halfDepth, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	mVertices[9] = Vertex(-halfWidth, +halfHeight, +halfDepth, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	mVertices[10] = Vertex(+halfWidth, +halfHeight, +halfDepth, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	mVertices[11] = Vertex(+halfWidth, +halfHeight, -halfDepth, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	mVertices[12] = Vertex(-halfWidth, -halfHeight, -halfDepth, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	mVertices[13] = Vertex(+halfWidth, -halfHeight, -halfDepth, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	mVertices[14] = Vertex(+halfWidth, -halfHeight, +halfDepth, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	mVertices[15] = Vertex(-halfWidth, -halfHeight, +halfDepth, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	mVertices[16] = Vertex(-halfWidth, -halfHeight, +halfDepth, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	mVertices[17] = Vertex(-halfWidth, +halfHeight, +halfDepth, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	mVertices[18] = Vertex(-halfWidth, +halfHeight, -halfDepth, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	mVertices[19] = Vertex(-halfWidth, -halfHeight, -halfDepth, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	mVertices[20] = Vertex(+halfWidth, -halfHeight, -halfDepth, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	mVertices[21] = Vertex(+halfWidth, +halfHeight, -halfDepth, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	mVertices[22] = Vertex(+halfWidth, +halfHeight, +halfDepth, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	mVertices[23] = Vertex(+halfWidth, -halfHeight, +halfDepth, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	mVertexCount = static_cast<unsigned int>(mVertices.size());

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

	// Create the index buffer
	mIndices.resize(36);
	mIndices[0] = 0; mIndices[1] = 1; mIndices[2] = 2;
	mIndices[3] = 0; mIndices[4] = 2; mIndices[5] = 3;

	// Fill in the back face index data
	mIndices[6] = 4; mIndices[7] = 5; mIndices[8] = 6;
	mIndices[9] = 4; mIndices[10] = 6; mIndices[11] = 7;

	// Fill in the top face index data
	mIndices[12] = 8; mIndices[13] = 9; mIndices[14] = 10;
	mIndices[15] = 8; mIndices[16] = 10; mIndices[17] = 11;

	// Fill in the bottom face index data
	mIndices[18] = 12; mIndices[19] = 13; mIndices[20] = 14;
	mIndices[21] = 12; mIndices[22] = 14; mIndices[23] = 15;

	// Fill in the left face index data
	mIndices[24] = 16; mIndices[25] = 17; mIndices[26] = 18;
	mIndices[27] = 16; mIndices[28] = 18; mIndices[29] = 19;

	// Fill in the right face index data
	mIndices[30] = 20; mIndices[31] = 21; mIndices[32] = 22;
	mIndices[33] = 20; mIndices[34] = 22; mIndices[35] = 23;

	mIndexCount = static_cast<unsigned int>(mIndices.size());

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
