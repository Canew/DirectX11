#pragma once

#include "Shader.h"

class BasicShader : public Shader
{
private:
	BasicShader() = default;

public:
	static BasicShader* StaticClass()
	{
		static BasicShader basicShader;
		return &basicShader;
	}

	virtual void Init(ID3D11Device& device) override;

	virtual void SetShader(ID3D11DeviceContext& deviceContext) override;

private:
	virtual void BuildShader(ID3D11Device& device) override;

private:
	std::vector<ComPtr<ID3D11SamplerState>> mSamplerList;
};
