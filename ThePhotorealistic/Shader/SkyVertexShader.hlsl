
cbuffer cbPerFrame : register(b0)
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
};

struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	// Use local vertex position as cubemap lookup vector.
	vout.PosL = vin.PosL;

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosH = mul(vout.PosH, gView);
	vout.PosH = mul(vout.PosH, gProjection);
	//vout.PosH.z = vout.PosH.w;

	return vout;
}