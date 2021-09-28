#include "stdafx.h"
#include "Texture.h"
#include "Lib/DirectXTex/DirectXTex.h"

Texture::Texture(ID3D11Device& device)
{
	Init(device, L"Texture/checkboard.dds");
}

Texture::Texture(ID3D11Device& device, const WCHAR* filename)
{
	Init(device, filename);
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
