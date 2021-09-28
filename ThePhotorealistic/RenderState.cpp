#include "stdafx.h"
#include "RenderState.h"

ComPtr<ID3D11RasterizerState> RenderState::WireframeRS = 0;
ComPtr<ID3D11RasterizerState> RenderState::NoCullRS = 0;

ComPtr<ID3D11BlendState> RenderState::AlphaToCoverageBS = 0;
ComPtr<ID3D11BlendState> RenderState::TransparentBS = 0;

ComPtr<ID3D11DepthStencilState> RenderState::LessEqualDSS = 0;
ComPtr<ID3D11DepthStencilState> RenderState::NoDoubleBlendDSS = 0;

std::unique_ptr<float[]> RenderState::DefaultBlendFactor = 0;

void RenderState::Init(ID3D11Device& device)
{
	//
	// WireframeRS
	//
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	device.CreateRasterizerState(&wireframeDesc, &WireframeRS);

	//
	// NoCullRS
	//
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;

	device.CreateRasterizerState(&noCullDesc, &NoCullRS);

	//
	// AlphaToCoverageBS
	//
	D3D11_BLEND_DESC alphaToCoverageDesc = { 0 };
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device.CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS);

	//
	// TransparentBS
	//
	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device.CreateBlendState(&transparentDesc, &TransparentBS);

	//
	// LessEqualDSS
	//
	D3D11_DEPTH_STENCIL_DESC lessEqualDesc = {};
	lessEqualDesc.DepthEnable = true;
	lessEqualDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	lessEqualDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	lessEqualDesc.StencilEnable = false;
	lessEqualDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	lessEqualDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	lessEqualDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	lessEqualDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	lessEqualDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	lessEqualDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	lessEqualDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	lessEqualDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	lessEqualDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	lessEqualDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//
	// NoDoubleBlendDSS
	//
	D3D11_DEPTH_STENCIL_DESC noDoubleBlendDesc = {};
	noDoubleBlendDesc.DepthEnable = true;
	noDoubleBlendDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlendDesc.DepthFunc = D3D11_COMPARISON_LESS;
	noDoubleBlendDesc.StencilEnable = true;
	noDoubleBlendDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	noDoubleBlendDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	noDoubleBlendDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	noDoubleBlendDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	device.CreateDepthStencilState(&noDoubleBlendDesc, NoDoubleBlendDSS.GetAddressOf());

	// DefaultBlendFactor
	RenderState::DefaultBlendFactor = std::make_unique<float[]>(4);
	RenderState::DefaultBlendFactor[0] = 0.0f;
	RenderState::DefaultBlendFactor[1] = 0.0f;
	RenderState::DefaultBlendFactor[2] = 0.0f;
	RenderState::DefaultBlendFactor[3] = 0.0f;
}
