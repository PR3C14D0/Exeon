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

struct PixelOutput
{
    float4 screen : SV_Target0;
};

PixelOutput PixelMain(VertexOutput input, uint index : SV_SampleIndex)
{
    PixelOutput output;
    float4 albedoColor = albedo.Load(input.position.xy, index);
    float4 nml = normalize(normal.Load(input.position.xy, index) * 2 - 1);
    float4 vertexPos = position.Load(input.position.xy, index);
    
    float4 ambientColor = float4(0.1f, 0.1f, 0.1f, 1.f);
    float4 lightPos = float4(0.f, 200.f, 200.f, 1.f);
    float4 lightColor = float4(1.f, 1.f, 1.f, 1.f);
    
    float3 lightDir = normalize(lightPos.xyz - vertexPos.xyz);
    float LdotN = saturate(dot(lightDir, nml.xyz));
    
    float4 outputColor = saturate(saturate(LdotN + ambientColor) * lightColor) * albedoColor;
    
    output.screen = outputColor;
    return output;
}