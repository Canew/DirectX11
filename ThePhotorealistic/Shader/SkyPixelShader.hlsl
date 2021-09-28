
TextureCube gCubeMap : register(t0);

SamplerState gPointWrapSampler : register(s0);
SamplerState gPointClampSampler : register(s1);
SamplerState gLinearWrapSampler : register(s2);
SamplerState gLinearClampSampler : register(s3);
SamplerState gAnisotropicWrapSampler : register(s4);
SamplerState gAnisotropicClampSampler : register(s5);

struct PixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

float4 main(PixelIn pin) : SV_Target
{
	return gCubeMap.Sample(gLinearWrapSampler, pin.PosL);
}
