struct VSInput
{
	float4 Position : POSITION;
	float4 Color : COLOR;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

namespace Globals
{
	cbuffer PerFrame : register (cb0)
	{
		row_major float4x4 World;
		row_major float4x4 View;
		row_major float4x4 Proj;
	};

#define _Wm Globals::World
#define _Vm Globals::View
#define _Pm Globals::Proj
#define _Cr Globals::Color

};


PSInput main(VSInput input)
{
	PSInput output;

	output.Position = input.Position;
	output.Position = mul(output.Position, _Wm);
	output.Position = mul(output.Position, _Vm);
	output.Position = mul(output.Position, _Pm);
	output.Color = input.Color;

	return output;
}