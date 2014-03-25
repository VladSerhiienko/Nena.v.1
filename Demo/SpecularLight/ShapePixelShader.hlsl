
struct PSInput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float3 View : TEXCOORD1;
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
		float4 Specular; // xyz = Color, z = Power
	};
}

float4 main(PSInput input) : SV_TARGET
{
	float3 diffuseOpposite = normalize(-Globals::DiffuseDirection.xyz);

	float4 color = Globals::AmbientColor;

	float4 shapeColor = Globals::ShapeTexture.Sample(
		Globals::ShapeSampler, input.Tex
		);

	float diffuseIntesity = saturate(
		dot(input.Normal, diffuseOpposite)
		);


	float3 reflection;
	float4 specular = 0.0f;

	if (diffuseIntesity > 0.0f)
	{
		color += saturate(Globals::DiffuseColor * diffuseIntesity);
		reflection = normalize(2 * diffuseIntesity * input.Normal - diffuseOpposite);
		specular = pow(saturate(dot(reflection, input.View)), Globals::Specular.w);
	}

	shapeColor = color * shapeColor;
	shapeColor = shapeColor + specular;

	return saturate(shapeColor);
}