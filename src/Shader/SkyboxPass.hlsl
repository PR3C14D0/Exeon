struct VertexOutput
{
    float4 position : SV_Position;
    float3 texCoord : TEXCOORD0; // Coordenadas de textura para la skybox
};

cbuffer VP : register(b0)
{
    matrix View;
    matrix Projection;
}

Texture2D frontTex : register(t0);
Texture2D backTex : register(t1);
Texture2D leftTex : register(t2);
Texture2D rightTex : register(t3);
Texture2D topTex : register(t4);
Texture2D bottomTex : register(t5);

SamplerState skyboxSampler : register(s0);

VertexOutput VertexMain(float4 position : POSITION0)
{
    VertexOutput output;
    output.position = mul(position, View);
    output.position = mul(output.position, Projection);

    // Normalizamos la posición para usarla como coordenadas para la skybox.
    // La normalización en el espacio de la cámara es importante.
    output.texCoord = normalize(position.xyz);
    //output.texCoord = float3(1.f, 1.f, 1.f);

    return output;
}

struct PixelOutput
{
    float4 screen : SV_Target0;
};

// En PixelMain
PixelOutput PixelMain(VertexOutput input)
{
    PixelOutput output;

    // Las coordenadas de textura están en el espacio de la cámara, así que mapea correctamente.
    if (abs(input.texCoord.x) > abs(input.texCoord.y) && abs(input.texCoord.x) > abs(input.texCoord.z))
    {
        // Frente y atrás (eje X)
        if (input.texCoord.x > 0.0)
            output.screen = frontTex.Sample(skyboxSampler, float2(-input.texCoord.z, -input.texCoord.y));
        else
            output.screen = backTex.Sample(skyboxSampler, float2(input.texCoord.z, -input.texCoord.y));
    }
    else if (abs(input.texCoord.y) > abs(input.texCoord.z))
    {
        // Arriba y abajo (eje Y)
        if (input.texCoord.y > 0.0)
            output.screen = topTex.Sample(skyboxSampler, float2(input.texCoord.x, input.texCoord.z));
        else
            output.screen = bottomTex.Sample(skyboxSampler, float2(input.texCoord.x, -input.texCoord.z));
    }
    else
    {
        // Derecha e izquierda (eje Z)
        if (input.texCoord.z > 0.0)
            output.screen = rightTex.Sample(skyboxSampler, float2(input.texCoord.x, input.texCoord.y));
        else
            output.screen = leftTex.Sample(skyboxSampler, float2(-input.texCoord.x, input.texCoord.y));
    }
    
    output.screen = float4(1.f, 0.f, 0.f, 1.f);

    return output;
}

