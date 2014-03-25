
struct PSInput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORDS0;
};

namespace Globals
{
	SamplerState ShapeSampler : register (s0);
	Texture2D<float4> ShapeTexture : register (t0);

	cbuffer Light : register (cb0)
	{
		float4 AmbientColor;
		float4 DiffuseColor;
		float4 DiffuseDirection;
	};
}

float4 main(PSInput input) : SV_TARGET
{
	float3 diffuseOpposite = -Globals::DiffuseDirection.xyz;

	float4 shapeColor = Globals::ShapeTexture.Sample(
		Globals::ShapeSampler, input.Tex
		);

	float diffuseIntesity = saturate(
		dot(input.Normal, diffuseOpposite)
		);

	float4 color = Globals::AmbientColor;

	if (diffuseIntesity > 0.0f)	color += saturate(
		Globals::DiffuseColor * diffuseIntesity
		);

	shapeColor = color * shapeColor;

	return shapeColor;
}