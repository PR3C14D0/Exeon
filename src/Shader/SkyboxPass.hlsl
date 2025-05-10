struct VertexOutput
{
    float4 position : SV_Position;
};

cbuffer VP : register(b0)
{
    matrix View;
    matrix Projection;
}

VertexOutput VertexMain(float4 position : POSITION0)
{
    VertexOutput output;
    output.position = mul(position, View);
    output.position = mul(output.position, Projection);
    return output;
}