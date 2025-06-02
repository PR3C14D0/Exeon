cbuffer WVP : register(b0) {
    matrix World;
    matrix View;
    matrix Projection;
}

struct VertexOutput
{
    float4 position : SV_Position;
};

VertexOutput VertexMain(float4 position : POSITION)
{
    VertexOutput output;
    output.position = mul(position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);
    
    return output;
}

void PixelMain(VertexOutput input)
{
    return;
}