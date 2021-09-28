#pragma once

#include "d3dUtil.h"
#include <memory>

class RenderState
{
public:
	static void Init(ID3D11Device& device);

	static ComPtr<ID3D11RasterizerState> WireframeRS;
	static ComPtr<ID3D11RasterizerState> NoCullRS;

	static ComPtr<ID3D11BlendState> AlphaToCoverageBS;
	static ComPtr<ID3D11BlendState> TransparentBS;

	static ComPtr<ID3D11DepthStencilState> LessEqualDSS;
	static ComPtr<ID3D11DepthStencilState> NoDoubleBlendDSS;

	static std::unique_ptr<float[]> DefaultBlendFactor;
};
