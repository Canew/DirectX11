#pragma once

#include "D3DUtil.h"
#include <vector>
#include "Texture.h"


class Object abstract
{
protected:
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 TexC;

		Vertex() {}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT2& uv)
			: Position(p), Normal(n), TexC(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz), TexC(u, v) {}
	};
	struct Material
	{
		XMFLOAT4 DiffuseAlbedo;
		XMFLOAT3 Fresnel;
		float Roughness;

		Material(const XMFLOAT4& diffuseAlbedo, const XMFLOAT3& fresnel, const float roughness)
			: DiffuseAlbedo(diffuseAlbedo), Fresnel(fresnel), Roughness(roughness) {}
	};
	struct ObjectMatrix
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

public:
	Object(ID3D11Device& device);
	virtual ~Object() = default;

	virtual void Render(ID3D11DeviceContext& deviceContext);

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(XMFLOAT3 position);
	virtual void SetScale(float x, float y, float z);
	virtual void SetScale(XMFLOAT3 scale);

	virtual void SetMaterial(XMFLOAT4 diffuseAlbedo, XMFLOAT3 fresnel, float roughness);
	virtual void SetTexture(ID3D11Device& device, const WCHAR* filename);

protected:
	ComPtr<ID3D11Buffer> mObjectBuffer = nullptr;
	ComPtr<ID3D11Buffer> mMaterialBuffer = nullptr;

	std::vector<Vertex> mVertices;
	std::vector<unsigned long> mIndices;
	ComPtr<ID3D11Buffer> mVertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> mIndexBuffer = nullptr;
	unsigned int mVertexCount = 0;
	unsigned int mIndexCount = 0;

	XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mRotation = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

	Material mMaterial;
	Texture mTexture;
};
