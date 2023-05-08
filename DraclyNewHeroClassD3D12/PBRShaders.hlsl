//PBR Shaders

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    uint texIndex : TEXINDEX;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 n : NORMAL;
    float2 texcoord : TEXCOORD;
    uint texIndex : TEXINDEX;
};

//Texture2D g_texture : register(t0);
//SamplerState g_sampler : register(s0);

PSInput VSMain(VSInput Input)
{
    PSInput Output;

    Output.position = float4(Input.position, 1.0f);
    Output.n = Input.normal;
    Output.texcoord = Input.texcoord;
    Output.texIndex = Input.texIndex;

    return Output;
}

float4 PSMain() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}