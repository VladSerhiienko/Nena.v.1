
struct VsInput
{
	float4 PositionObj : POSITION;
};

struct PsInput
{
	float4 PositionClip : SV_POSITION;
	//float4 PositionProj : SV_POSITION;
	//float4 PositionClip : TEXCOORD0;
	float Depth : TEXCOORD0;
};

namespace Resources
{
	cbuffer Matrices : register (cb0)
	{
		row_major float4x4 g_World;
		row_major float4x4 g_View;
		row_major float4x4 g_Proj;
	};

#define _w Resources::g_World
#define _v Resources::g_View
#define _p Resources::g_Proj
};


PsInput main( in VsInput input )
{
	printf("VS");

	PsInput output;
	input.PositionObj.w = 1.0f;
	output.PositionClip = mul(input.PositionObj, _w);
	output.PositionClip = mul(output.PositionClip, _v);
	output.PositionClip = mul(output.PositionClip, _p);
	output.Depth = output.PositionClip.z / output.PositionClip.w;
	//output.PositionProj = mul(input.PositionTransform, _w);
	//output.PositionProj = mul(output.PositionProj, _v);
	//output.PositionProj = mul(output.PositionProj, _p);
	//output.PositionClip = output.PositionProj;
	return output;
}