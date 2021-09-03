
cbuffer cbPerObject : register(b0)
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	// Transform to world space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = vout.PosH.xyz;

	// Transform to homogeneous clip space.
	vout.PosH = mul(vout.PosH, gView);
	vout.PosH = mul(vout.PosH, gProjection);


	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	vout.NormalW = normalize(vout.NormalW);
	vout.TexC = vin.TexC;

	return vout;
}

