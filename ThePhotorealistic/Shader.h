#pragma once

#include "D3DUtil.h"
#include <vector>
#include <string>
#include <fstream>

class ShaderInclude : public ID3DInclude
{
    HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
    {
        std::ifstream in(pFileName);
        if (in.is_open())
        {
            in.seekg(0, std::ios::end);
            UINT size = static_cast<UINT>(in.tellg());
            *pBytes = size;

            in.seekg(0, std::ios::beg);
            char* data = new char[size];
            in.read(data, size);
            *ppData = data;

            return S_OK;
        }
        else
        {
            return E_FAIL;
        }
    }

    HRESULT Close(LPCVOID pData) override
    {
        delete (const_cast<void*>(pData));
        return S_OK;
    }
};

class Shader
{
public:
	void Init(ID3D11Device& device);
	void SetShader(ID3D11DeviceContext& deviceContext);

private:
	void BuildShader(ID3D11Device& device);

private:
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader> mPixelShader;
	ComPtr<ID3D11InputLayout> mInputLayout;

    std::vector<ComPtr<ID3D11SamplerState>> mSamplerList;
};
