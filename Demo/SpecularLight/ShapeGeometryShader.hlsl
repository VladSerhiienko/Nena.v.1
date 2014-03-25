
struct GSInput
{
	uint3x3 V : GSVERTEXINDICES;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float3 View : TEXCOORD1;
};

namespace Globals
{
	cbuffer ShapeTransform : register (cb0)
	{
		row_major float4x4 World;
		row_major float4x4 View;
		row_major float4x4 Proj;
	};

#define _Wm Globals::World
#define _Vm Globals::View
#define _Pm Globals::Proj

	cbuffer Camera : register (cb1)
	{
		float4 CameraPosition;
	};

#define _Cp Globals::CameraPosition.xyz

	StructuredBuffer<float3> Points : register (t0);
	StructuredBuffer<float3> Normals : register (t1);
	StructuredBuffer<float2> Texcoords : register (t2);

#define _Get_point(i) Globals::Points[i]
#define _Get_normal(i) Globals::Normals[i]
#define _Get_texcoords(i) Globals::Texcoords[i]

} 

[maxvertexcount(3)] void main(in point GSInput input[1], inout TriangleStream<PSInput> output)
{
	/*printf("m w %2.2f %2.2f %2.2f %2.2f", _Wm[0][0], _Wm[0][1], _Wm[0][2], _Wm[0][3]);
	printf("    %2.2f %2.2f %2.2f %2.2f", _Wm[1][0], _Wm[1][1], _Wm[1][2], _Wm[1][3]);
	printf("    %2.2f %2.2f %2.2f %2.2f", _Wm[2][0], _Wm[2][1], _Wm[2][2], _Wm[2][3]);
	printf("    %2.2f %2.2f %2.2f %2.2f", _Wm[3][0], _Wm[3][1], _Wm[3][2], _Wm[3][3]);

	printf("m v %2.2f %2.2f %2.2f %2.2f", _Vm[0][0], _Vm[0][1], _Vm[0][2], _Vm[0][3]);
	printf("    %2.2f %2.2f %2.2f %2.2f", _Vm[1][0], _Vm[1][1], _Vm[1][2], _Vm[1][3]);
	printf("    %2.2f %2.2f %2.2f %2.2f", _Vm[2][0], _Vm[2][1], _Vm[2][2], _Vm[2][3]);
	printf("    %2.2f %2.2f %2.2f %2.2f", _Vm[3][0], _Vm[3][1], _Vm[3][2], _Vm[3][3]);

	printf("m p %2.2f %2.2f %2.2f %2.2f", _Pm[0][0], _Pm[0][1], _Pm[0][2], _Pm[0][3]);
	printf("    %2.2f %2.2f %2.2f %2.2f", _Pm[1][0], _Pm[1][1], _Pm[1][2], _Pm[1][3]);
	printf("    %2.2f %2.2f %2.2f %2.2f", _Pm[2][0], _Pm[2][1], _Pm[2][2], _Pm[2][3]);
	printf("    %2.2f %2.2f %2.2f %2.2f", _Pm[3][0], _Pm[3][1], _Pm[3][2], _Pm[3][3]);*/

	for (uint i = 0; i < 3; i++)
	{
		PSInput element;

		element.Position = float4(_Get_point(input[0].V[i][0]), 1.0f);
		element.Normal = _Get_normal(input[0].V[i][1]);
		element.Tex = _Get_texcoords(input[0].V[i][2]);

		element.Position = mul(element.Position, _Wm);
		element.View = normalize(_Cp - element.Position.xyz);

		element.Position = mul(element.Position, _Vm);
		element.Position = mul(element.Position, _Pm);

		element.Normal = mul(element.Normal, (float3x3) _Wm);
		element.Normal = mul(element.Normal, (float3x3) _Vm);
		element.Normal = mul(element.Normal, (float3x3) _Pm);
		element.Normal = normalize(element.Normal);

		output.Append(element);
	}
}