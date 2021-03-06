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
		float Fresnel0;
		float Metallic;
		float Roughness;
		float padding = 0.0f;

		Material(const XMFLOAT4& diffuseAlbedo, const float fresnel0, const float metallic, const float roughness)
			: DiffuseAlbedo(diffuseAlbedo), Fresnel0(fresnel0), Metallic(metallic), Roughness(roughness) {}
	};
	struct ObjectMatrix
	{
		XMMATRIX World;
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

	virtual void SetMaterial(XMFLOAT4 diffuseAlbedo, float fresnel0, float metallic, float roughness);
	virtual void SetAlbedo(XMFLOAT4 diffuseAlbedo);
	virtual void SetFresnel0(float fresnel0);
	virtual void SetMetallic(float metallic);
	virtual void SetRoughness(float roughness);
	virtual void SetDiffuseMap(ID3D11Device& device, const WCHAR* filename);
	virtual void SetDiffuseMap(std::shared_ptr<Texture> texture);
	virtual void SetNormalMap(ID3D11Device& device, const WCHAR* filename);
	virtual void SetNormalMap(std::shared_ptr<Texture> texture);
	virtual void SetShaderClass(Shader* shader);

	virtual Shader* GetShaderClass();
	virtual std::shared_ptr<Texture> GetDiffuseMap() { return mDiffuseMap; }
	virtual std::shared_ptr<Texture> GetNormalMap() { return mNormalMap; }
	virtual std::vector<Mesh> GetMeshes() { return mMeshes; }

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
	std::shared_ptr<Texture> mDiffuseMap;
	std::shared_ptr<Texture> mNormalMap;
	Shader* mShaderClass;

	ID3D11Device& mDevice;
	ID3D11DeviceContext& mDeviceContext;
};
