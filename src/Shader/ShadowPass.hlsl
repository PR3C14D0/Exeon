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

float4 PixelMain(VertexOutput input) : SV_Target
{
    return float4(0, 0, 0, 0);
}