#include "stdafx.h"
#include "BasicShader.h"
#include "SkyShader.h"
#include "Camera.h"
#include "Shader.h"
#include "Object.h"

Object::Object(ID3D11Device& device, ID3D11DeviceContext& deviceContext, std::string_view filepath)
	: mTexture(device), mMaterial(XMFLOAT4(0.5f, 0.5f, 0.5f, 0.1f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f), mShaderClass(BasicShader::StaticClass()), mDevice(device), mDeviceContext(deviceContext)
{
	if (!filepath.empty())
	{
		try
		{
			LoadModel(filepath);
		}
		catch (std::exception& e)
		{
			OutputDebugStringA(e.what());
		}
	}

	// Build Matrix Constant Buffers
	D3D11_BUFFER_DESC objectBufferDesc = {};
	objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectBufferDesc.ByteWidth = sizeof(ObjectMatrix);
	objectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectBufferDesc.MiscFlags = 0;
	objectBufferDesc.StructureByteStride = 0;
	device.CreateBuffer(&objectBufferDesc, NULL, mObjectBuffer.GetAddressOf());

	// Build Material Constant Buffers
	D3D11_BUFFER_DESC materialBufferDesc = {};
	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(Material);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;
	device.CreateBuffer(&materialBufferDesc, NULL, mMaterialBuffer.GetAddressOf());
}

void Object::Update(ID3D11DeviceContext& deviceContext, float dt)
{
}

void Object::Render(ID3D11DeviceContext& deviceContext)
{
	XMMATRIX world = XMMatrixIdentity();
	world *= XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	world *= XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
	world *= XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	XMMATRIX view = Camera::StaticClass()->GetView();
	XMMATRIX projection = Camera::StaticClass()->GetProjection();

	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);

	// Set VS constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext.Map(mObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	ObjectMatrix* objectBufferPtr = reinterpret_cast<ObjectMatrix*>(mappedResource.pData);
	objectBufferPtr->World = world;
	objectBufferPtr->View = view;
	objectBufferPtr->Projection = projection;

	deviceContext.Unmap(mObjectBuffer.Get(), 0);

	unsigned int bufferNumber = 0;
	deviceContext.VSSetConstantBuffers(bufferNumber, 1, mObjectBuffer.GetAddressOf());

	// Set PS constant buffer.
	deviceContext.Map(mMaterialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	Material* materialBufferPtr = reinterpret_cast<Material*>(mappedResource.pData);
	materialBufferPtr->DiffuseAlbedo = mMaterial.DiffuseAlbedo;
	materialBufferPtr->Fresnel = mMaterial.Fresnel;
	materialBufferPtr->Roughness = mMaterial.Roughness;

	deviceContext.Unmap(mMaterialBuffer.Get(), 0);

	bufferNumber = 0;
	deviceContext.PSSetConstantBuffers(bufferNumber, 1, mMaterialBuffer.GetAddressOf());

	// Set texture.
	deviceContext.PSSetShaderResources(0, 1, mTexture.GetShaderResourceView().GetAddressOf());

	// Render meshes.
	for (unsigned int i = 0; i < mMeshes.size(); ++i)
	{
		mMeshes[i].Render();
	}
}

void Object::SetPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void Object::SetPosition(XMFLOAT3 position)
{
	mPosition = position;
}

void Object::SetRotation(float x, float y, float z)
{
	mRotation.x = x;
	mRotation.y = y;
	mRotation.z = z;
}

void Object::SetRotation(XMFLOAT3 rotation)
{
	mRotation = rotation;
}

void Object::SetScale(float x, float y, float z)
{
	mScale.x = x;
	mScale.y = y;
	mScale.z = z;
}

void Object::SetScale(XMFLOAT3 scale)
{
	mScale = scale;
}

void Object::SetMaterial(XMFLOAT4 diffuseAlbedo, XMFLOAT3 fresnel, float roughness)
{
	mMaterial.DiffuseAlbedo = diffuseAlbedo;
	mMaterial.Fresnel = fresnel;
	mMaterial.Roughness = roughness;
}

void Object::SetTexture(ID3D11Device& device, const WCHAR* filename)
{
	mTexture = Texture(device, filename);
}

void Object::SetShaderClass(Shader* shader)
{
	mShaderClass = shader;
}


Shader* Object::GetShaderClass()
{
	return mShaderClass;
}

bool Object::IsTransparent()
{
	return (mMaterial.DiffuseAlbedo.w < 1.0f);
}

void Object::LoadModel(std::string_view filepath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filepath.data(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (pScene == nullptr)
	{
		throw std::exception();
	}

	ProcessNode(pScene->mRootNode, pScene);
}

void Object::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		mMeshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Object::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Get Vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		
		if (mesh->HasTextureCoords(0))
		{
			vertex.TexC.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
			vertex.TexC.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.TexC.x = 0.0f;
			vertex.TexC.y = 0.0f;
		}

		vertices.push_back(vertex);
	}

	// Get Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	return Mesh(mDevice, mDeviceContext, vertices, indices);
}
