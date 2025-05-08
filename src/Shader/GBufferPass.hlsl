cbuffer WVP : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 vertexPos : POSITION0;
};

SamplerState texSampler : register(s0);
Texture2D tex : register(t0);
Texture2D ormTex : register(t1);

VertexOutput VertexMain(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    VertexOutput output;
    
    output.position = mul(position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    output.normal = float4(normalize(mul((float3) normal, (float3x3) World)), 0.0f);
    output.uv = uv;
    output.vertexPos = mul(position, World);
    
    return output;
}

struct PixelOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
    float4 orm : SV_Target2;
    float4 position : SV_Target3;
};

PixelOutput PixelMain(VertexOutput input)
{
    float2 uvFlipped = float2(input.uv.x, 1.0f - input.uv.y);
    
    PixelOutput output;
    output.albedo = tex.Sample(texSampler, uvFlipped);
    output.normal = input.normal * 0.5f + 0.5f;
    output.orm = ormTex.Sample(texSampler, uvFlipped);
    output.position = input.vertexPos;
    return output;
}