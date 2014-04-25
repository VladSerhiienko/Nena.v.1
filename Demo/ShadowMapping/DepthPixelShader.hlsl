
struct PsInput
{
	float4 PositionProj : SV_POSITION;
	float4 PositionClip : TEXCOORD0;
	float Depth : TEXCOORD1;
};

float main(PsInput input) : SV_TARGET
{
	//printf("DepthPS %f", input.Depth);
	return input.PositionClip.z / input.PositionClip.w;
	//return input.Depth;
}