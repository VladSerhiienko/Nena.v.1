
struct GsInput
{
	uint3x3 V : GSVERTEXINDICES;
};

struct PsInput
{
	float4 PositionProj			: SV_POSITION; // camera related position
	float2 Tex					: TEXCOORD0; // texture coord
	float3 NormalWorld			: NORMAL; // camera related normal
	float4 PositionClip			: TEXCOORD1; // light source related position
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
	//printf("ShadowGS");

	PsInput element;

	for (uint i = 0; i < 3; i++)
	{
		// acquire data from buffers
		element.Tex = _t(input[0].V[i][2]);
		element.NormalWorld = _n(input[0].V[i][1]);
		element.PositionProj = float4(_c(input[0].V[i][0]), 1.0f);

		float4 worldPosition = // remember vertex world
		element.PositionProj = mul(element.PositionProj, _cw);

		// transform to clip space from camera perspective
		element.PositionProj = mul(element.PositionProj, _cv);
		element.PositionProj = mul(element.PositionProj, _cp);

		// remeber vertex position in clip space
		element.PositionClip = element.PositionProj;
		/*printf("pos clip: %2.2f %2.2f %2.2f",
			element.PositionClip.x,
			element.PositionClip.y,
			element.PositionClip.z
			);*/

		element.NormalWorld = normalize(mul(element.NormalWorld, (float3x3) _cw));
		/*printf("norm world: %2.2f %2.2f %2.2f",
			element.NormalWorld.x,
			element.NormalWorld.y,
			element.NormalWorld.z
			);*/

		element.LightDirection = normalize(_lpos.xyz - worldPosition.xyz);
		/*printf("light dir world: %2.2f %2.2f %2.2f",
			element.LightDirection.x,
			element.LightDirection.y,
			element.LightDirection.z
			);*/

		// send compiled vertices to pixel shader
		output.Append(element);
	}
}