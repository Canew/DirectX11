#pragma once

#include "D3DUtil.h"

class Vertex
{
public:
	Vertex() : Position(0.0f, 0.0f, 0.0f), Normal(0.0f, 0.0f, 0.0f), TexCoord(0.0f, 0.0f), Tangent(0.0f, 0.0f, 0.0f) {}
	Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT2& uv)
		: Position(p), Normal(n), TexCoord(uv), Tangent(0.0f, 0.0f, 0.0f) {}
	Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT2& uv, const XMFLOAT3& t)
		: Position(p), Normal(n), TexCoord(uv), Tangent(t) {}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float u, float v)
		: Position(px, py, pz), Normal(nx, ny, nz), TexCoord(u, v), Tangent(0.0f, 0.0f, 0.0f) {}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float u, float v,
		float tx, float ty, float tz)
		: Position(px, py, pz), Normal(nx, ny, nz), TexCoord(u, v), Tangent(tx, ty, tz) {}

	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Tangent;
};

