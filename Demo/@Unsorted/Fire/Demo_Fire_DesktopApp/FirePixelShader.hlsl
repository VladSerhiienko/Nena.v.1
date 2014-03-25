Texture2D g_FireColor			: register(t0);
Texture2D g_FireNoise			: register(t1);
Texture2D g_FireBody			: register(t2);

SamplerState g_WrapSampler		: register(s0);
SamplerState g_ClampSampler		: register(s1);

cbuffer Distortion				: register(cb0)
{
	float2 g_Distortion0;
	float2 g_Distortion1;
	float2 g_Distortion2;
	float g_DistortionScale;
	float g_DistortionBias;
};

struct PixelShaderInput
{
	float4	PositionClip		: SV_POSITION;
	float2	ColorTex			: TEXCOORD0;
	float2	NoiseOctave0		: TEXCOORD1;
	float2	NoiseOctave1		: TEXCOORD2;
	float2	NoiseOctave2		: TEXCOORD3;
};

float4 main(in const PixelShaderInput input) : SV_TARGET
{
	printf("CPU::PixelShaderCall");

	const float perturb = (1.0f - input.ColorTex.y) * g_DistortionScale + g_DistortionBias;

	printf("perturb      = <%f>", perturb);
	printf("scale        = <%f>", g_DistortionScale);
	printf("bias         = <%f>", g_DistortionBias);

	float4 noise0 = (g_FireNoise.Sample(g_WrapSampler, input.NoiseOctave0) - 0.5f) * 2.0f; // * g_Distortion0;
	float4 noise1 = (g_FireNoise.Sample(g_WrapSampler, input.NoiseOctave1) - 0.5f) * 2.0f; // * g_Distortion1;
	float4 noise2 = (g_FireNoise.Sample(g_WrapSampler, input.NoiseOctave2) - 0.5f) * 2.0f; // * g_Distortion2;
	noise0.xy = noise0.xy * g_Distortion0.xy;
	noise1.xy = noise1.xy * g_Distortion1.xy;
	noise2.xy = noise2.xy * g_Distortion2.xy;

	const float4 accumNoise = noise0 + noise1 + noise2;
	//const float2 finalNoise = accumNoise.xy + input.ColorTex.xy;
	const float2 finalNoise = (accumNoise.xy * perturb) + input.ColorTex.xy;

	const float4 fireBody = g_FireBody.Sample(g_ClampSampler, finalNoise);
	float4 fireColor = g_FireColor.Sample(g_ClampSampler, finalNoise);
	fireColor.a = fireBody.r;
	/*fireColor.xyz *= 0.0f;
	fireColor.b = 0.7f;
	fireColor.g = 0.3f;*/

	return fireColor;
}