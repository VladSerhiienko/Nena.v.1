
struct VertexShaderInput
{
	float3 PositionLocal			: POSITION;
	float3 NormalLocal				: NORMAL;
	float2 Tex						: TEXCOORD0;
};

struct PixelShaderInput
{
	float4 PositionClip				: SV_POSITION;
	float3 NormalWorld				: NORMAL;
	float2 Tex						: TEXCOORD0;
	float4 LightPositionClip[4]		: LIGHTCLP0;
	float3 LightPositionInertial[4]	: LIGHTDIR0;
	uint NumLights					: NLIGHTS;
};

cbuffer Spatial : register (cb0)
{
	row_major float4x4 g_World;
	row_major float4x4 g_CameraView;
	row_major float4x4 g_CameraProj;
	row_major float4x4 g_LightView[4];
	row_major float4x4 g_LightProj[4];
};

cbuffer Light : register (cb1)
{
	float4 g_LightPositionWorld[4];
};

cbuffer NumLights : register (cb1)
{
	uint g_NumLights;
};

PixelShaderInput main( in VertexShaderInput input )
{
	PixelShaderInput output;

	const uint numLights = g_NumLights;

	printf("vertex shader: num lights = %d", numLights);

	float4 positionLocal = float4(input.PositionLocal, 1.0f); positionLocal.w = 1.0f;
	float4 positionWorld = mul(positionLocal, g_World);

	output.NumLights = numLights;
	output.Tex = input.Tex;

	output.PositionClip = positionWorld;
	output.PositionClip = mul(output.PositionClip, g_CameraView);
	output.PositionClip = mul(output.PositionClip, g_CameraProj);

	[unroll] for (uint i = 0; i < numLights; i++)
	{
		output.LightPositionClip[i] = positionWorld;
		output.LightPositionClip[i] = mul(output.LightPositionClip[i], g_LightView[i]);
		output.LightPositionClip[i] = mul(output.LightPositionClip[i], g_LightProj[i]);
	}

	output.NormalWorld = mul(input.NormalLocal, (float3x3)g_World);
	output.NormalWorld = normalize(output.NormalWorld);

	[unroll] for (uint j = 0; j < numLights; j++)
	{
		printf( "%f %f %f", g_LightPositionWorld[j].x, g_LightPositionWorld[j].y, g_LightPositionWorld[j].z);

		output.LightPositionInertial[j] = g_LightPositionWorld[j].xyz - positionWorld.xyz;
		output.LightPositionInertial[j] = normalize(output.LightPositionInertial[j]);
	}

	return output;
}