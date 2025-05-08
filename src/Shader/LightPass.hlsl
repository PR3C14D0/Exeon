#define PI 3.14159265359
cbuffer ScreenQuadBuffer : register(b0)
{
    float3 cameraPos;
}

struct VertexOutput
{
    float4 position : SV_Position;
};

VertexOutput VertexMain(float4 position : POSITION0, float2 uv : TEXCOORD0)
{
    VertexOutput output;
    output.position = position;
    return output;
}

Texture2DMS<float4> albedo : register(t0);
Texture2DMS<float4> normal : register(t1);
Texture2DMS<float4> position : register(t2);
Texture2DMS<float4> orm : register(t3);

struct PixelOutput
{
    float4 screen : SV_Target0;
};

float3 FresnelShlick(float cosTheta, float3 F0)
{
    return F0 + (1.f - F0) * pow(saturate(1.f - cosTheta), 5.f);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH * (a2 - 1.f) + 1.f);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.f);
    float k = (r * r) / 8.f;
    
    float num = NdotV;
    float denom = NdotV * (1.f - k) + k;
    
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.f);
    float NdotL = max(dot(N, L), 0.f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

PixelOutput PixelMain(VertexOutput input, uint index : SV_SampleIndex)
{
    float3 lightPos = float3(0.f, 5.f, 5.f);
    float3 lightColor = float3(100.f, 100.f, 100.f);
    
    float4 albedoColor = albedo.Load(input.position.xy, index);
    float4 nml = normalize(normal.Load(input.position.xy, index) * 2 - 1);
    float4 vertexPos = position.Load(input.position.xy, index);
    
    /* ORM G-Buffer */
    float3 ormData = orm.Load(input.position.xy, index).rgb;
    float ao = ormData.r;
    float roughness = ormData.g;
    float metallic = ormData.b;
    
    
    float3 N = nml.xyz;
    float3 V = normalize(cameraPos - vertexPos.xyz);
    float3 F0 = float3(.04f, .04f, .04f);
    F0 = lerp(F0, albedoColor.xyz, metallic);
    
    /* Reflectance equation */
    float3 Lo = float3(0.f, 0.f, 0.f);
    
    /* Calculate light radiance */
    float3 L = normalize(lightPos - vertexPos.xyz);
    float3 H = normalize(V + L);
    float distance = length(lightPos - vertexPos.xyz);
    float attenuation = 1.0f / (distance * distance + 1.f);
    float3 radiance = lightColor * attenuation;
    radiance *= (1.0 + metallic * 0.2);
    
    /* Cook torrance BRDF */
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelShlick(saturate(dot(H, V)), F0);
    
    float3 kS = F; 
    float3 kD = (1.0 - kS) * (1.0 - metallic);;
    kD *= 1.f - metallic;
    
    float3 numerator = NDF * G * F;
    float denominator = 4.f * max(dot(N, V), 0.f) * max(dot(N, L), 0.f) + 0.0001f;
    float3 specular = numerator / denominator;
    
    float NdotL = max(dot(N, L), 0.f);
    
    Lo += (kD * albedoColor.xyz / PI + specular) * radiance * NdotL;
    
    float3 ambient = float3(0.3f, 0.3f, 0.3f) * albedoColor.xyz * ao;
    ambient = lerp(ambient, albedoColor.xyz * ao * 0.2, metallic); // Los metales reflejan más el ambiente
    
    float3 color = ambient + Lo;
    
    PixelOutput output;
    
    output.screen = float4(color.x, color.y, color.z, 1.f);
    return output;
}