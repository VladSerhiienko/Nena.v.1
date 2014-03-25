
struct PsInput
{
	float4 Pos : SV_POSITION;
	float2 Tex0 : TEXCOORD0;
	float2 Tex1 : TEXCOORD1;
	float2 Tex2 : TEXCOORD2;
	float2 Tex3 : TEXCOORD3;
};

namespace Resources
{
	cbuffer Distortion : register (cb0)
	{
		float4 g_Distortion[2];
	};

#define _dis1		Resources::g_Distortion[0].xy
#define _dis2		Resources::g_Distortion[0].zw
#define _dis3		Resources::g_Distortion[1].xy
#define _dbias		Resources::g_Distortion[1].w
#define _dscale		Resources::g_Distortion[1].z

	Texture2D<float4> g_FireTex		: register (t0);
	Texture2D<float4> g_NoiseTex	: register (t1);
	Texture2D<float4> g_AlphaTex	: register (t2);

#define _fire		Resources::g_FireTex
#define _noise		Resources::g_NoiseTex
#define _alpha		Resources::g_AlphaTex

	SamplerState g_Wrap		: register (s0);
	SamplerState g_Clamp	: register (s1);

#define _clamp		Resources::g_Clamp
#define _wrap		Resources::g_Wrap

};

float4 main(PsInput input) : SV_TARGET
{
	printf("PS");

	float perturb = ((1.0f - input.Tex0.y) * _dscale) + _dbias;
	//printf("perturb: %2.2f", perturb);

	float2 noise1 = (_noise.Sample(_wrap, input.Tex1).xy - 0.5f) * 2.0f * _dis1;
	float2 noise2 = (_noise.Sample(_wrap, input.Tex2).xy - 0.5f) * 2.0f * _dis2;
	float2 noise3 = (_noise.Sample(_wrap, input.Tex3).xy - 0.5f) * 2.0f * _dis3;
	//float2 noise1 = (_noise.Sample(_clamp, input.Tex1).xy - 0.5f) * 2.0f * _dis1;
	//float2 noise2 = (_noise.Sample(_clamp, input.Tex2).xy - 0.5f) * 2.0f * _dis2;
	//float2 noise3 = (_noise.Sample(_clamp, input.Tex3).xy - 0.5f) * 2.0f * _dis3;
	float2 noise = noise1 + noise2 + noise3;
	//printf("noise: %2.2f %2.2f", noise.x, noise.y);

	float2 coords = noise * perturb + input.Tex0.xy;
	//printf("coords: %2.2f %2.2f", coords.x, coords.y);

	float4 fire = _fire.Sample(_clamp, coords);
	//float4 fire = _fire.Sample(_wrap, coords);
	fire.a = _alpha.Sample(_clamp, coords).x;
	//fire.a = _alpha.Sample(_wrap, coords).x;

	printf("fire: %2.2f %2.2f %2.2f %2.2f", fire.x, fire.y, fire.z, fire.w);

	return fire;
}