

struct PsInput
{
	float4 PositionProj			: SV_POSITION; // camera related position
	float2 Tex					: TEXCOORD0; // texture coord
	float3 NormalWorld			: NORMAL; // camera related normal
	float4 PositionClip			: TEXCOORD1; // light source related position
	float3 LightDirection		: TEXCOORD2; // light ray direction
};

namespace Resources
{
	cbuffer Light : register (cb0)
	{
		float4 g_Ambient;
		float4 g_Diffuse;
	};

#define _amb Resources::g_Ambient
#define _dff Resources::g_Diffuse
#define _bias 0.00001f

	Texture2D<float4> g_Color : register (t0);
	Texture2D<float4> g_Shadow : register (t1);
	//Texture2D<float4> g_Depth : register (t1);
	//Texture2D<float> g_Depth : register (t1);

#define _color Resources::g_Color
#define _shadow Resources::g_Shadow
//#define _depth Resources::g_Depth

	SamplerState g_Clamp : register (s0);
	SamplerState g_Wrap : register (s1);

#define _clamp Resources::g_Clamp
#define _wrap Resources::g_Wrap

};

float4 main(PsInput input) : SV_TARGET
{
	float4 color = _amb;

	float lightIntensity = saturate(dot(input.NormalWorld, input.LightDirection));
	if (lightIntensity > 0.0f) color = saturate(color + _dff * lightIntensity);

	color *= _color.Sample(_wrap, input.Tex);

	float2 shadowPositionProj = float2(
		input.PositionClip.x / input.PositionClip.w * 0.5f + 0.5f,
		input.PositionClip.y / input.PositionClip.w * -0.5f + 0.5f
		);

	color *= _shadow.Sample(_clamp, shadowPositionProj).r;

	return color;
}