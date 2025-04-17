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

struct PixelOutput
{
    float4 screen : SV_Target0;
};

PixelOutput PixelMain(VertexOutput input, uint index : SV_SampleIndex)
{
    PixelOutput output;
    output.screen = float4(0.f, 0.f, 0.f, 1.f);
    return output;
}