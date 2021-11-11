
struct PixelIn
{
    float4 PosH : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 main(PixelIn pin) : SV_Target
{
    return pin.PosH;
}