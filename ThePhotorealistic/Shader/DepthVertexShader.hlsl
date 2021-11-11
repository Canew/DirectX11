
cbuffer cbPerObject : register(b0)
{
	matrix gWorld;
};

cbuffer cbPerFrame : register(b1)
{
	matrix gView;
	matrix gProjection;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexCoord : TEXCOORD;
	float3 TangentL : TANGENT;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosH = mul(vout.PosH, gView);
	vout.PosH = mul(vout.PosH, gProjection);

	vout.TexCoord = vin.TexCoord;

	return vout;
}

