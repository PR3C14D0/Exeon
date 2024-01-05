struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

VertexOutput VertexMain(float4 position: POSITION0, float4 color: COLOR0)
{
    VertexOutput output;
    output.position = position;
    output.color = color;
    
    return output;
}

struct PixelOutput
{
    float4 color : SV_Target0;
};

PixelOutput PixelMain(VertexOutput input)
{
    PixelOutput output;
    output.color = input.color;
    return output;
}