Texture2D g_Color : register(t0);
SamplerState g_LinearSampler : register(s0);

struct PixelShaderInput
{
	float4	PositionClip				: SV_POSITION;
	float3	NormalView					: NORMAL;
	float2  Tex							: TEXCOORD0;
};

float4 main(in const PixelShaderInput input) : SV_TARGET
{
	const float4 color = g_Color.Sample(g_LinearSampler, input.Tex);
	printf("CPU::Scene::PixelShaderCall");
	//printf("----------------------------------------------------");
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	return color;
}