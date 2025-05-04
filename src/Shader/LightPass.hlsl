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
    output.screen = albedo.Load(input.position.xy, index);
    return output;
}