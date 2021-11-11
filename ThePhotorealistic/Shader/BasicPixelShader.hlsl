
Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gShadowMap : register(t2);
TextureCube gCubeMap : register(t3);

SamplerState gPointWrapSampler : register(s0);
SamplerState gPointClampSampler : register(s1);
SamplerState gLinearWrapSampler : register(s2);
SamplerState gLinearClampSampler : register(s3);
SamplerState gAnisotropicWrapSampler : register(s4);
SamplerState gAnisotropicClampSampler : register(s5);

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
	float pad1;

    float4 gLightAmbient;
    float4 gLightDiffuse;
    float3 gLightIntensity;
    float pad2;
    float3 gLightDirection;
    float pad3;
    float3 gLightPosition;
    float pad4;

    float gFogStart;
    float gFogRange;
    float2 pad5;
    float4 gFogColor;
};


struct PixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD;
    float3 TangentW : TANGENT;
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
    float4 TexCoordolor = gDiffuseMap.Sample(gAnisotropicWrapSampler, pin.TexCoord);
    TexCoordolor = pow(TexCoordolor, gamma);
    clip(TexCoordolor.a - 0.1f);

    float3 toEyeW = gEyePosW - pin.PosW;
    float distToEye = length(toEyeW);
    toEyeW /= distToEye;

    // Bump Normal
    float3 normalMapSample = gNormalMap.Sample(gLinearWrapSampler, pin.TexCoord).xyz;
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    float3 N = pin.NormalW;
    float3 T = normalize(pin.TangentW - dot(pin.TangentW, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);

    float3 bumpedNormalW = normalize(mul(normalT, TBN));

    // Lighting
    // Attenuation
    float distance = length(gLightPosition - pin.PosW);
    float attenuation = 1.0f / (distance * distance);
    //float3 radiance = gLightDiffuse.xyz * gLightIntensity * attenuation;
    float3 radiance = { 1.0f, 1.0f, 1.0f };
    
    // Ambient
    //litColor = gLightAmbient;

    // Diffuse BRDF
    float ndotl = saturate(dot(bumpedNormalW, -gLightDirection));// * 0.5f + 0.5f; // Half Lambert
    float3 diffuse = (saturate(TexCoordolor.xyz * ndotl));

    // Specular BRDF
    float3 halfNormal = normalize(-gLightDirection + toEyeW);

    // Specular D
    float specularD = NDF(bumpedNormalW, halfNormal);

    // Specular G
    float specularG = GeometryShadow(bumpedNormalW, -gLightDirection, toEyeW);

    // Specular F
    float3 specularF = SchlickFresnel(halfNormal, -gLightDirection);
    float3 metalBaseColor = { 0.9f, 0.9f, 0.9f };
    specularF = lerp(0.08 * specularF, metalBaseColor, gMetallic);

    // Calculate Specular
    float3 specular = (specularD * specularG * specularF) / pi * dot(bumpedNormalW, -gLightDirection) * dot(bumpedNormalW, toEyeW) + 0.001f;
    
    // Energy Conservation
    float3 kS = specularF;
    float3 kD = 1.0f - kS;
    kD *= 1.0 - gMetallic;

    // Calculate Reflect
    float3 incident = -toEyeW;
    float3 reflectionVector = reflect(incident, bumpedNormalW);
    float4 reflectionColor = gCubeMap.Sample(gAnisotropicWrapSampler, reflectionVector);

    specular += saturate(specularF * (1.0f - gRoughness) * reflectionColor.xyz);

    // Calculate Radiance
    float3 rad = ((kD * diffuse / pi) + specular) * radiance * dot(-gLightDirection, bumpedNormalW);
    litColor.xyz += rad;
    

    // Gamma Correction
    litColor.xyz = pow(litColor.xyz, float3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));

    // Fog
    float fogLerp = saturate((distToEye - gFogStart) / gFogRange);
    litColor = lerp(litColor, gFogColor, fogLerp);

    litColor.a = gDiffuseAlbedo.a;

    return litColor;
}