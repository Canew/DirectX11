
TextureCube gCubeMap;

SamplerState samTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct PixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

float4 main(PixelIn pin) : SV_Target
{
	return gCubeMap.Sample(samTriLinearSam, pin.PosL);
}
