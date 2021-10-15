#include "stdafx.h"
#include "DynamicCubeMap.h"
#include "Camera.h"

DynamicCubeMap::DynamicCubeMap(ID3D11Device& device)
{
	// Create texture 2d.
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = CubeMapSize;
	texDesc.Height = CubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	ComPtr<ID3D11Texture2D> cubeTex;
	device.CreateTexture2D(&texDesc, 0, cubeTex.GetAddressOf());

	// Create render target view per face.
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 1;

	for (int i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		device.CreateRenderTargetView(cubeTex.Get(), &rtvDesc, mDynamicCubeMapRTV[i].GetAddressOf());
	}

	// Create shader resource view for cube map.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	device.CreateShaderResourceView(cubeTex.Get(), &srvDesc, mDynamicCubeMapSRV.GetAddressOf());

	// Create depth buffer.
	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.Width = CubeMapSize;
	depthTexDesc.Height = CubeMapSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> depthTex;
	device.CreateTexture2D(&depthTexDesc, 0, depthTex.GetAddressOf());

	// Create viewport for cube map face.
	mCubeMapViewport.TopLeftX = 0.0f;
	mCubeMapViewport.TopLeftY = 0.0f;
	mCubeMapViewport.Width = static_cast<float>(CubeMapSize);
	mCubeMapViewport.Height = static_cast<float>(CubeMapSize);
	mCubeMapViewport.MinDepth = 0.0f;
	mCubeMapViewport.MaxDepth = 1.0f;

	// Create camera.
	for (int i = 0; i < 6; ++i)
	{
		mCubeMapCamera[i] = std::make_unique<Camera>();
	}
}

void DynamicCubeMap::BuildCubeFaceCamera(float x, float y, float z)
{
	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f, y, z),
		XMFLOAT3(x - 1.0f, y, z),
		XMFLOAT3(x, y + 1.0f, z),
		XMFLOAT3(x, y - 1.0f, z),
		XMFLOAT3(x, y, z + 1.0f),
		XMFLOAT3(x, y, z - 1.0f)
	};

	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f)
	};

	for (int i = 0; i < 6; ++i)
	{
		mCubeMapCamera[i]->LookAt(center, targets[i], ups[i]);
		mCubeMapCamera[i]->SetLens(0.5f * XM_PI, 1.0f, 0.1f, 1000.0f);
		mCubeMapCamera[i]->UpdateViewMatrix();
	}
}

void DynamicCubeMap::DrawScene(ID3D11DeviceContext& deviceContext)
{
	ComPtr<ID3D11RenderTargetView> renderTarget;

	// Create cube map.
	deviceContext.RSSetViewports(1, &mCubeMapViewport);
	for (int i = 0; i < 6; ++i)
	{
		// Clear views.
		float backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		deviceContext.ClearRenderTargetView(mDynamicCubeMapRTV[i].Get(), backgroundColor);
		deviceContext.ClearDepthStencilView(mDynamicCubeMapDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// bind cube map face to render target.
		renderTarget = mDynamicCubeMapRTV[i];
		deviceContext.OMSetRenderTargets(1, renderTarget.GetAddressOf(), mDynamicCubeMapDSV.Get());

		//DrawScene(mCubeMapCamera[i], false);
	}

	deviceContext.GenerateMips(mDynamicCubeMapSRV.Get());
}