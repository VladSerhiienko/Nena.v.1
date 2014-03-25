
struct VsInput
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

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
	cbuffer Matrices : register (cb0)
	{
		row_major float4x4 g_World;
		row_major float4x4 g_View;
		row_major float4x4 g_Proj;
	};

#define _w			Resources::g_World
#define _v			Resources::g_View
#define _p			Resources::g_Proj

	cbuffer Noise : register (cb1)
	{
		float4 g_Noise[2];
	};

#define _vscroll	Resources::g_Noise[0]
#define _scales		Resources::g_Noise[1]
#define _t			_vscroll.x
#define _vscroll1	_vscroll.y
#define _vscroll2	_vscroll.z
#define _vscroll3	_vscroll.w
#define _scales1	_scales.x
#define _scales2	_scales.y
#define _scales3	_scales.z

};

PsInput main(VsInput input)
{
	PsInput output;

	printf("VS");

	input.Pos.w = 1.0f;
	output.Pos = mul(input.Pos, _w);
	//printf("time: %2.2f", _t);
	//printf("output.Pos: %2.2f %2.2f %2.2f", output.Pos.x, output.Pos.y, output.Pos.z);
	output.Pos = mul(output.Pos, _v);
	output.Pos = mul(output.Pos, _p);

	output.Tex0 = input.Tex;
	printf("output.Tex0: %2.2f %2.2f", output.Tex0.x, output.Tex0.y);

	output.Tex1 = input.Tex * _scales1;
	output.Tex2 = input.Tex * _scales2;
	output.Tex3 = input.Tex * _scales3;
	output.Tex1.y += _t * _vscroll1;
	output.Tex2.y += _t * _vscroll2;
	output.Tex3.y += _t * _vscroll3;

	//printf("output.Tex0: %2.2f %2.2f", output.Tex0.x, output.Tex0.y);
	//printf("output.Tex1: %2.2f %2.2f", output.Tex1.x, output.Tex1.y);
	//printf("output.Tex2: %2.2f %2.2f", output.Tex2.x, output.Tex2.y);
	//printf("output.Tex3: %2.2f %2.2f", output.Tex3.x, output.Tex3.y);

	return output;
}