struct VertexOutput
{
    float4 position : SV_Position;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

SamplerState texSampler : register(s0);
Texture2D tex : register(t0);

VertexOutput VertexMain(float4 position : POSITION0, float4 normal : NORMAL0, float2 uv : TEXCOORD0)
{
    VertexOutput output;
    
    output.position = position;
    output.normal = normal;
    output.uv = uv;
    
    return output;
}

struct PixelOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
};

PixelOutput PixelMain(VertexOutput input)
{
    PixelOutput output;
    output.albedo = tex.Sample(texSampler, float2(input.uv.x, 1 - input.uv.y));
    output.normal = input.normal;
    return output;
}