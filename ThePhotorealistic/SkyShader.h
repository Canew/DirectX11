#pragma once

#include "Shader.h"

class SkyShader : public Shader
{
private:
	SkyShader() = default;

public:
	static SkyShader* StaticClass()
	{
		static SkyShader skyShader;
		return &skyShader;
	}

	virtual void Init(ID3D11Device& device) override;

	virtual void SetShader(ID3D11DeviceContext& deviceContext) override;

protected:
	virtual void BuildShader(ID3D11Device& device) override;

private:
	std::vector<ComPtr<ID3D11SamplerState>> mSamplerList;
};
