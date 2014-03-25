

struct VsInput
{
	float4 Position : POSITION;
};

struct PsInput
{
	float4 PositionProj : SV_POSITION;
};

namespace PerCall
{
	cbuffer Camera
	{
		row_major float4x4 World;
		row_major float4x4 CameraView;
		row_major float4x4 CameraProjection;
	};

#define _cw PerCall::World
#define _cv PerCall::CameraView
#define _cp PerCall::CameraProjection
};

PsInput main(VsInput input)
{
	PsInput output;
	output.PositionProj = mul(input.Position, _cw);
	output.PositionProj = mul(output.PositionProj, _cv);
	output.PositionProj = mul(output.PositionProj, _cp);

	return output;
}