
Texture2D    gDiffuseMap : register(t0);


SamplerState gPointWrapSampler : register(s0);
SamplerState gPointClampSampler : register(s1);
SamplerState gLinearWrapSampler : register(s2);
SamplerState gLinearClampSampler : register(s3);
SamplerState gAnisotropicWrapSampler : register(s4);
SamplerState gAnisotropicClampSampler : register(s5);


struct Light
{
    float4 Ambient;
    float4 Diffuse;
	float3 Intensity;
	float FalloffStart;           // point/spot light only
	float3 Direction;    // directional/spot light only
	float FalloffEnd;            // point/spot light only
	float3 Position;     // point light only
	float SpotPower;          // spot light only
};


cbuffer cbMaterial : register(b0)
{
	float4 gDiffuseAlbedo;
	float gFresnel0;
    float gMetallic;
	float gRoughness;
    float padding;
};

cbuffer cbPass : register(b1)
{
	float3 gEyePosW;
	float pad;

    Light gLight;

    float gFogStart;
    float gFogRange;
    float4 gFogColor;
};


struct PixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
};

static const float pi = 3.14159265f;
static const float gamma = 2.2f;

// return Specular D
float NDF(float3 normal, float3 halfNormal)
{
    float alpha = pow(gRoughness, 2);
    float ndoth = saturate(dot(normal, halfNormal));

    float numerator = alpha * alpha;
    float denominator = (ndoth * ndoth) * (alpha * alpha - 1.0f) + 1.0f;
    denominator = pi * pow(denominator, 2);

    return numerator / denominator;
}

// return Specular G
float GeometrySchlickGGX(float ndotx)
{
    float r = gRoughness + 1.0f;
    float k = (r * r) / 8.0f;

    float numerator = ndotx;
    float denominator = ndotx * (1.0f - k) + k;

    return numerator /  denominator;
}

float GeometryShadow(float3 normal, float3 toLight, float3 toEye)
{
    float ndotv = saturate(dot(normal, toEye));
    float ndotl = saturate(dot(normal, toLight));

    float ggx1 = GeometrySchlickGGX(ndotv);
    float ggx2 = GeometrySchlickGGX(ndotl);

    return ggx1 * ggx2;
}

// return Specular F
float SchlickFresnel(float3 halfNormal, float3 toLight)
{
    return gFresnel0 + ((1 - gFresnel0) * pow((1 - saturate(dot(halfNormal, toLight))), 5));
}

float4 main(PixelIn pin) : SV_Target
{
    float4 litColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    float4 texColor = gDiffuseMap.Sample(gAnisotropicWrapSampler, pin.TexC);
    texColor = pow(texColor, gamma);
    clip(texColor.a - 0.1f);

    float3 toEyeW = gEyePosW - pin.PosW;
    float distToEye = length(toEyeW);
    toEyeW /= distToEye;

    // Lighting
    // Attenuation
    float distance = length(gLight.Position - pin.PosW);
    float attenuation = 1.0f / (distance * distance);
    //float3 radiance = gLight.Diffuse.xyz * gLight.Intensity * attenuation;
    float3 radiance = { 1.0f, 1.0f, 1.0f };
    
    // Ambient
    //litColor = gLight.Ambient;

    // Diffuse BRDF
    float ndotl = saturate(dot(pin.NormalW, -gLight.Direction));// * 0.5f + 0.5f; // Half Lambert
    float3 diffuse = (saturate(texColor.xyz * ndotl));

    // Specular BRDF
    float3 halfNormal = normalize(-gLight.Direction + toEyeW);

    // Specular D
    float specularD = NDF(pin.NormalW, halfNormal);

    // Specular G
    float specularG = GeometryShadow(pin.NormalW, -gLight.Direction, toEyeW);

    // Specular F
    float specularF = SchlickFresnel(halfNormal, -gLight.Direction);

    // Calculate Specular
    float3 specular = (specularD * specularG * specularF) / pi * dot(pin.NormalW, -gLight.Direction) * dot(pin.NormalW, toEyeW) + 0.001f;
    
    // Energy Conservation
    float3 kS = specularF;
    float3 kD = 1.0f - kS;
    kD *= 1.0 - gMetallic;

    // Calculate Radiance
    float3 rad = ((kD * diffuse / pi) + specular) * radiance * dot(-gLight.Direction, pin.NormalW);
    litColor.xyz += rad;

    // Gamma Correction
    litColor.xyz = pow(litColor.xyz, float3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));

    // Fog
    float fogLerp = saturate((distToEye - gFogStart) / gFogRange);
    litColor = lerp(litColor, gFogColor, fogLerp);

    litColor.a = gDiffuseAlbedo.a;

    return litColor;
}