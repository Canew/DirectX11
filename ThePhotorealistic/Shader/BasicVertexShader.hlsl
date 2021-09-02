cbuffer cbPerObject
{
	matrix gWorld;
	matrix gView;
	matrix gProjection;
};

struct VertexIn
{
	float3 Pos  : POSITION;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 Pos  : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.Pos = mul(float4(vin.Pos, 1.0f), gWorld);
	vout.Pos = mul(vout.Pos, gView);
	vout.Pos = mul(vout.Pos, gProjection);

	// Just pass vertex color into the pixel shader.
	vout.Color = vin.Color;

	return vout;
}

