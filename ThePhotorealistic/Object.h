#pragma once

#include "D3DUtil.h"
#include <vector>
#include <memory>
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

class Object
{
protected:
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
	Object(ID3D11Device& device, ID3D11DeviceContext& deviceContext, std::string_view filepath = "");
	virtual ~Object() = default;

	virtual void Update(ID3D11DeviceContext& deviceContext, float dt);
	virtual void Render(ID3D11DeviceContext& deviceContext);

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(XMFLOAT3 position);
	virtual void SetRotation(float x, float y, float z);
	virtual void SetRotation(XMFLOAT3 rotation);
	virtual void SetScale(float x, float y, float z);
	virtual void SetScale(XMFLOAT3 scale);

	virtual void SetMaterial(XMFLOAT4 diffuseAlbedo, XMFLOAT3 fresnel, float roughness);
	virtual void SetTexture(ID3D11Device& device, const WCHAR* filename);
	virtual void SetShaderClass(Shader* shader);

	virtual Shader* GetShaderClass();

	bool IsTransparent();

protected:
	virtual void LoadModel(std::string_view filepath);
	virtual void ProcessNode(aiNode* node, const aiScene* scene);
	virtual Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

protected:
	ComPtr<ID3D11Buffer> mObjectBuffer = nullptr;
	ComPtr<ID3D11Buffer> mMaterialBuffer = nullptr;

	std::vector<Mesh> mMeshes;

	XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mRotation = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

	Material mMaterial;
	Texture mTexture;
	Shader* mShaderClass;

	ID3D11Device& mDevice;
	ID3D11DeviceContext& mDeviceContext;
};
