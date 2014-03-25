
//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

struct VertexShaderInput
{
	float4	PositionLocal		: POSITION;
	float3	NormalLocal			: NORMAL;
	float2	Tex					: TEXCOORD0;
};

struct PixelShaderInput
{
	float4	PositionClip				: SV_POSITION;
	float3	NormalView					: NORMAL;
	float2  Tex							: TEXCOORD0;
};

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

cbuffer Spatial
{
	row_major float4x4 g_World;
	row_major float4x4 g_View;
	row_major float4x4 g_Proj;
};

//-----------------------------------------------------------------------------
// Shader
//-----------------------------------------------------------------------------

PixelShaderInput main(in VertexShaderInput input, in const uint vertexId : SV_VERTEXID)
{
	PixelShaderInput output = (PixelShaderInput) 0;
	printf("CPU::Scene::VertexShaderCall");
	printf("----------------------------------------------------");

	input.PositionLocal.w = 1.0f;
	output.PositionClip = mul(input.PositionLocal, g_World);
	output.PositionClip = mul(output.PositionClip, g_View);
	output.PositionClip = mul(output.PositionClip, g_Proj);
	output.NormalView = mul(input.NormalLocal, (float3x3)g_World);
	output.NormalView = mul(output.NormalView, (float3x3)g_View);
	output.Tex = input.Tex;
	return output;
};