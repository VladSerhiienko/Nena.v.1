
struct GsInput
{
	uint3x3 V : GSVERTEXINDICES;
};

struct PsInput
{
	float4 PositionClip			: SV_POSITION; // camera related position
	float2 Tex					: TEXCOORD0; // texture coord
	float3 NormalWorld			: NORMAL; // camera related normal
	float4 LightPositionClip	: TEXCOORD1; // light source related position
	float3 LightDirection		: TEXCOORD2; // light ray direction
};

namespace Globals
{
	cbuffer Matrices : register (cb0)
	{
		row_major float4x4 CameraWorld;
		row_major float4x4 CameraView;
		row_major float4x4 CameraProj;
	};

#define _cw		Globals::CameraWorld
#define _cv		Globals::CameraView
#define _cp		Globals::CameraProj

	cbuffer Light : register (cb1)
	{
		row_major float4x4 LightView;
		row_major float4x4 LightProj;
		float4 LightPosition;
	};

#define _lv		Globals::LightView
#define _lp		Globals::LightProj
#define _lpos	Globals::LightPosition

	StructuredBuffer<float3> Points : register (t0);
	StructuredBuffer<float3> Normals : register (t1);
	StructuredBuffer<float2> Texcoords : register (t2);

#define _c(i)	Globals::Points[i]
#define _n(i)	Globals::Normals[i]
#define _t(i)	Globals::Texcoords[i]

} 

[maxvertexcount(3)] void main(in point GsInput input[1], inout TriangleStream<PsInput> output)
{
	printf("ShadowGS");

	PsInput element;

	for (uint i = 0; i < 3; i++)
	{
		// acquire data from buffers
		element.Tex = _t(input[0].V[i][2]);
		element.NormalWorld = _n(input[0].V[i][1]);
		element.PositionClip = float4(_c(input[0].V[i][0]), 1.0f);

		element.LightPositionClip = // store world position
		element.PositionClip = mul(element.PositionClip, _cw);
		// transform to clip space from camera perspective
		element.PositionClip = mul(element.PositionClip, _cv);
		element.PositionClip = mul(element.PositionClip, _cp);
		printf("pos clip: %2.2f %2.2f %2.2f",
			element.PositionClip.x,
			element.PositionClip.y,
			element.PositionClip.z
			);

		element.NormalWorld = normalize(mul(element.NormalWorld, (float3x3) _cw));
		printf("norm world: %2.2f %2.2f %2.2f",
			element.NormalWorld.x,
			element.NormalWorld.y,
			element.NormalWorld.z
			);

		//element.LightPositionClip still equals to world position
		element.LightDirection = normalize(_lpos.xyz - element.LightPositionClip.xyz);
		printf("light dir world: %2.2f %2.2f %2.2f",
			element.LightDirection.x,
			element.LightDirection.y,
			element.LightDirection.z
			);

		// transform to clip space from light perspective
		element.LightPositionClip = mul(element.LightPositionClip, _lv);
		element.LightPositionClip = mul(element.LightPositionClip, _lp);
		printf("light pos clip: %2.2f %2.2f %2.2f",
			element.LightPositionClip.x,
			element.LightPositionClip.y,
			element.LightPositionClip.z
			);

		// send compiled vertices to pixel shader
		output.Append(element);
	}
}