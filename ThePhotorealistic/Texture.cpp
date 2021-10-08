#include "stdafx.h"
#include "Texture.h"
#include "Lib/DirectXTex/DirectXTex.h"
#include <fstream>

Texture::Texture(ID3D11Device& device)
{
	Init(device, L"Texture/checkboard.dds");
}

Texture::Texture(ID3D11Device& device, const WCHAR* filename)
{
	Init(device, filename);
}

Texture::Texture(ID3D11Device& device, const aiTexture* aiTexture)
	: mTextureWidth(aiTexture->mWidth), mTextureHeight(aiTexture->mHeight)
{
	if (aiTexture->mHeight == 0)
	{
		return;
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	if (aiTexture->mHeight == 0)
	textureDesc.Width = aiTexture->mWidth;
	textureDesc.Height = aiTexture->mHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = aiTexture->pcData;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	try
	{
		device.CreateTexture2D(&textureDesc, &subResource, mTexture.GetAddressOf());
		device.CreateShaderResourceView(mTexture.Get(), &srvDesc, mTextureView.GetAddressOf());
	}
	catch (std::exception e)
	{
		OutputDebugStringA(e.what());
	}
}

ComPtr<ID3D11ShaderResourceView> Texture::GetShaderResourceView()
{
	return mTextureView;
}

void Texture::Init(ID3D11Device& device, const WCHAR* filename)
{
	HRESULT hResult;
	ScratchImage image;
	TexMetadata metadata;
	int textureWidth = 0, textureHeight = 0;

	WCHAR ext[_MAX_EXT];
	_wsplitpath_s(filename, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	if (_wcsicmp(ext, L".dds") == 0)
	{
		hResult = LoadFromDDSFile(filename, DDS_FLAGS_NONE, &metadata, image);
		if (FAILED(hResult)) {
			return;
		}
	}
	else if (_wcsicmp(ext, L".tga") == 0)
	{
		hResult = DirectX::LoadFromTGAFile(filename, &metadata, image);
	}
	else
	{
		hResult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
		if (FAILED(hResult)) {
			return;
		}

		hResult = LoadFromWICFile(filename, WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hResult)) {
			return;
		}
	}

	// �̹����� ũ�⸦ ����ϴ�.
	textureWidth = (int)pow(2.0, floor(log((double)metadata.width) / log(2.0) + 1.01));
	if ((textureWidth / 2) == metadata.width)
		textureWidth /= 2;
	textureHeight = (int)pow(2.0, floor(log((double)metadata.height) / log(2.0) + 1.01));
	if ((textureHeight / 2) == metadata.height)
		textureHeight /= 2;

	// �ؽ�ó�� ���̴� ���ҽ� �並 ����ϴ�.
	hResult = CreateShaderResourceView(&device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), mTextureView.GetAddressOf());

	if (FAILED(hResult))
	{
		return;
	}

	mTextureWidth = static_cast<int>(metadata.width);
	mTextureHeight = static_cast<int>(metadata.height);
}
