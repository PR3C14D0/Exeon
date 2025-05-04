struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
};

VertexOutput VertexMain(float4 position : POSITION0, float2 uv : TEXCOORD0)
{
    VertexOutput output;
    
    output.position = position;
    output.uv = uv;
    
    return output;
}

struct PixelOutput
{
    float4 albedo : SV_Target0;
    float4 normal : SV_Target1;
};

PixelOutput PixelMain(VertexOutput input, uint index : SV_SampleIndex)
{
    PixelOutput output;
    output.albedo = float4(1.f, 0.f, 0.f, 1.f);
    output.normal = float4(0.f, 0.f, 0.f, 1.f);
    return output;
}