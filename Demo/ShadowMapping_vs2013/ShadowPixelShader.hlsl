
struct PsInput
{
	float4 PositionClip : SV_POSITION; // camera related position
	float2 Tex : TEXCOORD0; // texture coord
	float3 NormalWorld : NORMAL; // camera related normal
	float4 LightPositionClip : TEXCOORD1; // light source related position
	float3 LightDirection : TEXCOORD2; // light ray direction
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
	Texture2D<float> g_Depth : register (t1);

#define _color Resources::g_Color
#define _depth Resources::g_Depth

	SamplerState g_Clamp : register (s0);
	SamplerState g_Wrap : register (s1);

#define _clamp Resources::g_Clamp
#define _wrap Resources::g_Wrap

};

float4 main(PsInput input) : SV_TARGET
{
	printf("ShadowPS");

	float4 color = _amb;

	printf("ambient %f %f %f",
		_amb.x, _amb.y, _amb.z
		);
	printf("diffuse %f %f %f",
		_dff.x, _dff.y, _dff.z
		);

	float2 lightPositionProj = float2(
		input.LightPositionClip.x / input.LightPositionClip.w * 0.5f + 0.5f,
		input.LightPositionClip.y / input.LightPositionClip.w * -0.5f + 0.5f
		);

	printf("light pos proj %f %f",
		lightPositionProj.x,
		lightPositionProj.y
		);

	if (saturate(lightPositionProj.x) == lightPositionProj.x &&
		saturate(lightPositionProj.y) == lightPositionProj.y)
	{
		float depth = _depth.Sample(_clamp, lightPositionProj);
		float lightDepth = input.LightPositionClip.z / input.LightPositionClip.w -_bias;

		printf("depth %f light depth %f",
			depth, lightDepth
			);

		if (lightDepth < depth)
		{
			float lightIntensity = saturate(dot(input.NormalWorld, input.LightDirection));
			printf("intensity %f ",
				lightIntensity
				);

			if (lightIntensity > 0.0f)
			{
				color += _dff * lightIntensity;
				color = saturate(color);
			}
		}
	}

	float4 textureColor = _color.Sample(_wrap, input.Tex);

	printf("tex color %f %f %f",
		textureColor.x, textureColor.y, textureColor.z
		);

	color *= textureColor;

	printf("color %f %f %f",
		color.x, color.y, color.z
		);

	return color;
}