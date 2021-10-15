#include "stdafx.h"
#include "Geosphere.h"

Geosphere::Geosphere(ID3D11Device& device, ID3D11DeviceContext& deviceContext, float radius, UINT numSubdivisions)
	: Object(device, deviceContext)
{
	// Put a cap on the number of subdivisions.
	numSubdivisions = numSubdivisions < 5u ? numSubdivisions : 5u;

	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
	};

	DWORD k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	std::vector<Vertex> vertices(12);
	std::vector<unsigned int> indices(60);

	for (UINT i = 0; i < 12; ++i)
		vertices[i].Position = pos[i];

	for (UINT i = 0; i < 60; ++i)
		indices[i] = k[i];

	for (UINT i = 0; i < numSubdivisions; ++i)
	{
		// Save a copy of the input geometry.
		std::vector<Vertex> copyVertices = vertices;
		std::vector<unsigned int> copyIndices = indices;

		vertices.resize(0);
		indices.resize(0);

		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		// v0    m2     v2

		UINT numTris = static_cast<UINT>(copyIndices.size()) / 3;
		for (UINT i = 0; i < numTris; ++i)
		{
			Vertex v0 = copyVertices[copyIndices[i * 3 + 0]];
			Vertex v1 = copyVertices[copyIndices[i * 3 + 1]];
			Vertex v2 = copyVertices[copyIndices[i * 3 + 2]];

			//
			// Generate the midpoints.
			//

			Vertex m0, m1, m2;

			// For subdivision, we just care about the position component.  We derive the other
			// vertex components in CreateGeosphere.

			m0.Position = XMFLOAT3(
				0.5f * (v0.Position.x + v1.Position.x),
				0.5f * (v0.Position.y + v1.Position.y),
				0.5f * (v0.Position.z + v1.Position.z));

			m1.Position = XMFLOAT3(
				0.5f * (v1.Position.x + v2.Position.x),
				0.5f * (v1.Position.y + v2.Position.y),
				0.5f * (v1.Position.z + v2.Position.z));

			m2.Position = XMFLOAT3(
				0.5f * (v0.Position.x + v2.Position.x),
				0.5f * (v0.Position.y + v2.Position.y),
				0.5f * (v0.Position.z + v2.Position.z));

			//
			// Add new geometry.
			//

			vertices.push_back(v0); // 0
			vertices.push_back(v1); // 1
			vertices.push_back(v2); // 2
			vertices.push_back(m0); // 3
			vertices.push_back(m1); // 4
			vertices.push_back(m2); // 5

			indices.push_back(i * 6 + 0);
			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 5);

			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 4);
			indices.push_back(i * 6 + 5);

			indices.push_back(i * 6 + 5);
			indices.push_back(i * 6 + 4);
			indices.push_back(i * 6 + 2);

			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 1);
			indices.push_back(i * 6 + 4);
		}
	}

	// Project vertices onto sphere and scale.
	for (UINT i = 0; i < vertices.size(); ++i)
	{
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&vertices[i].Position));

		// Project onto sphere.
		XMVECTOR p = radius * n;

		XMStoreFloat3(&vertices[i].Position, p);
		XMStoreFloat3(&vertices[i].Normal, n);

		// Derive texture coordinates from spherical coordinates.
		float theta = 0.0f;

		// Quadrant I or IV
		if (vertices[i].Position.x >= 0.0f)
		{
			// If x = 0, then atanf(y/x) = +pi/2 if y > 0
			//                atanf(y/x) = -pi/2 if y < 0
			theta = atanf(vertices[i].Position.z / vertices[i].Position.x); // in [-pi/2, +pi/2]

			if (theta < 0.0f)
				theta += 2.0f * XM_PI; // in [0, 2*pi).
		}

		// Quadrant II or III
		else
			theta = atanf(vertices[i].Position.z / vertices[i].Position.x) + XM_PI; // in [0, 2*pi).;

		float phi = acosf(vertices[i].Position.y / radius);

		vertices[i].TexCoord.x = theta / XM_2PI;
		vertices[i].TexCoord.y = phi / XM_PI;
	}
}
