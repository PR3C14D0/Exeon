#define PI 3.14159265359
cbuffer ScreenQuadBuffer : register(b0)
{
    matrix InverseView;
    matrix InverseProjection;
    float3 cameraPos;
    float2 screenSize;
}

struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv : TEEXCOORD0;
};

VertexOutput VertexMain(float4 position : POSITION0, float2 uv : TEXCOORD0)
{
    VertexOutput output;
    output.position = position;
    output.uv = uv;
    return output;
}

Texture2DMS<float4> albedo : register(t0);
Texture2DMS<float4> normal : register(t1);
Texture2DMS<float4> depthTex : register(t2);
Texture2DMS<float4> orm : register(t3);

Texture2D frontTex : register(t4);
Texture2D backTex : register(t5);
Texture2D leftTex : register(t6);
Texture2D rightTex : register(t7);
Texture2D topTex : register(t8);
Texture2D bottomTex : register(t9);

SamplerState skyboxSampler : register(s0);

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

float3 ReconstructPosition(int2 pixelCoord, uint index)
{
    float depth = depthTex.Load(pixelCoord, index).r;

    float2 ndc = (float2(pixelCoord) / screenSize) * 2.0f - 1.0f;
    ndc.y *= -1.0f;
    
    float z = depth * 2.0f - 1.0f;
    float4 clipPos = float4(ndc.xy, depth, 1.0f);
    
    float4 viewPos = mul(InverseProjection, clipPos);
    viewPos /= viewPos.w;

    float4 worldPos = mul(InverseView, viewPos);

    return worldPos.xyz;
}

float3 ViewDirectionFromUV(float2 uv)
{
    float2 ndc = uv * 2.0f - 1.0f;

    float3 rayView = normalize(mul(float4(-ndc.x, ndc.y, 1.0f, 0.0f), InverseProjection).xyz);

    float3 rayWorld = normalize(mul(float4(rayView, 0.0f), InverseView).xyz);

    return -rayWorld;
}


float3 SampleSkybox(float3 dir)
{
    float3 absDir = abs(dir);
    float ma;
    float2 uv;
    float3 color;

    if (absDir.x >= absDir.y && absDir.x >= absDir.z)
    {
        ma = absDir.x;
        if (dir.x > 0)
        {
            // Right face (+X)
            uv = float2(-dir.z, -dir.y) / ma * 0.5 + 0.5;
            color = rightTex.Sample(skyboxSampler, uv).rgb;
        }
        else
        {
            // Left face (-X)
            uv = float2(dir.z, -dir.y) / ma * 0.5 + 0.5;
            color = leftTex.Sample(skyboxSampler, uv).rgb;
        }
    }
    else if (absDir.y >= absDir.x && absDir.y >= absDir.z)
    {
        ma = absDir.y;
        if (dir.y > 0)
        {
            // Top face (+Y)
            uv = float2(dir.x, dir.z) / ma * 0.5 + 0.5;
            color = topTex.Sample(skyboxSampler, uv).rgb;
        }
        else
        {
            // Bottom face (-Y)
            uv = float2(dir.x, -dir.z) / ma * 0.5 + 0.5;
            color = bottomTex.Sample(skyboxSampler, uv).rgb;
        }
    }
    else
    {
        ma = absDir.z;
        if (dir.z > 0)
        {
            // Front face (+Z)
            uv = float2(dir.x, -dir.y) / ma * 0.5 + 0.5;
            color = frontTex.Sample(skyboxSampler, uv).rgb;
        }
        else
        {
            // Back face (-Z)
            uv = float2(-dir.x, -dir.y) / ma * 0.5 + 0.5;
            color = backTex.Sample(skyboxSampler, uv).rgb;
        }
    }

    return color;
}

PixelOutput PixelMain(VertexOutput input, uint index : SV_SampleIndex)
{
    float depth = depthTex.Load(input.position.xy, index).r;
    if (depth >= 1.0f - 1e-5)
    {
        float3 dir = ViewDirectionFromUV(input.uv);
        float3 skyColor = SampleSkybox(dir);

        PixelOutput outSky;
        outSky.screen = float4(skyColor, 1.0f);
        return outSky;
    }
    
    float3 lightPos = float3(0.f, 1.f, 1.f);
    float3 lightColor = float3(50.f, 50.f, 50.f);
    
    int2 pixelCoord = int2(input.position.xy);

    float3 albedoColor = albedo.Load(pixelCoord, index).rgb;
    float3 N = normalize(normal.Load(pixelCoord, index).rgb * 2 - 1);
    float3 pos = ReconstructPosition(pixelCoord, index);
    
    float3 ormData = orm.Load(pixelCoord, index).rgb;
    float ao = ormData.r;
    float roughness = ormData.g;
    float metallic = ormData.b;

    float3 V = normalize(cameraPos - pos);
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedoColor, metallic);

    float3 L = normalize(lightPos - pos);
    float3 H = normalize(V + L);
    float distance = length(lightPos - pos);
    float attenuation = 1.0f / (distance * distance + 1.f);
    float3 radiance = lightColor * attenuation;
    radiance *= (1.0 + metallic * 0.2);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelShlick(saturate(dot(H, V)), F0);

    float3 kS = F;
    float3 kD = (1.0 - kS) * (1.0 - metallic);

    float3 numerator = NDF * G * F;
    float denominator = 4.f * max(dot(N, V), 0.f) * max(dot(N, L), 0.f) + 0.0001f;
    float3 specular = numerator / denominator;

    float NdotL = max(dot(N, L), 0.f);
    float3 Lo = (kD * albedoColor / PI + specular) * radiance * NdotL;

    float3 ambient = 0.1f * albedoColor * ao;

    float3 color = ambient + Lo;
    
    /* Tonemap */
    color = color / (color + 1.0);
    //color = pow(saturate(color), 1.0 / 2.2);

    PixelOutput output;
    output.screen = float4(color.x, color.y, color.z, 1.f);
    return output;
}
