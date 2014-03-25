
cbuffer Spatial
{
	row_major float4x4 g_World;
	row_major float4x4 g_View;
	row_major float4x4 g_Proj;
};

cbuffer Noise
{
	float	g_FrameTime;
	float3	g_ScrollSpeeds;
	float3	g_Scales;
	float	g_Reserved0;
};

struct VertexShaderInput
{
	float4	PositionLocal		: POSITION;
	float2	Tex					: TEXCOORD0;
};

struct PixelShaderInput
{
	float4	PositionClip		: SV_POSITION;
	float2	ColorTex			: TEXCOORD0;
	float2	NoiseOctave0		: TEXCOORD1;
	float2	NoiseOctave1		: TEXCOORD2;
	float2	NoiseOctave2		: TEXCOORD3;
};

PixelShaderInput main(in VertexShaderInput input, in uint vertexId : SV_VERTEXID)
{
	/*printf("World: %f %f %f %f", g_World[0][0], g_World[0][1], g_World[0][2], g_World[0][3]);
	printf("       %f %f %f %f", g_World[1][0], g_World[1][1], g_World[1][2], g_World[1][3]);
	printf("       %f %f %f %f", g_World[2][0], g_World[2][1], g_World[2][2], g_World[2][3]);
	printf("       %f %f %f %f", g_World[3][0], g_World[3][1], g_World[3][2], g_World[3][3]);

	printf("View : %f %f %f %f", g_View[0][0], g_View[0][1], g_View[0][2], g_View[0][3]);
	printf("       %f %f %f %f", g_View[1][0], g_View[1][1], g_View[1][2], g_View[1][3]);
	printf("       %f %f %f %f", g_View[2][0], g_View[2][1], g_View[2][2], g_View[2][3]);
	printf("       %f %f %f %f", g_View[3][0], g_View[3][1], g_View[3][2], g_View[3][3]);

	printf("Proj : %f %f %f %f", g_Proj[0][0], g_Proj[0][1], g_Proj[0][2], g_Proj[0][3]);
	printf("       %f %f %f %f", g_Proj[1][0], g_Proj[1][1], g_Proj[1][2], g_Proj[1][3]);
	printf("       %f %f %f %f", g_Proj[2][0], g_Proj[2][1], g_Proj[2][2], g_Proj[2][3]);
	printf("       %f %f %f %f", g_Proj[3][0], g_Proj[3][1], g_Proj[3][2], g_Proj[3][3]);*/

	PixelShaderInput output = (PixelShaderInput) 0;

	printf("CPU::VertexShaderCall");
	printf("----------------------------------------------------");
	printf("vertex id      = <%d>", vertexId);
	printf("input position = <%f %f %f>", input.PositionLocal.x, input.PositionLocal.y, input.PositionLocal.z);
	printf("input tex      = <%f %f>", input.Tex.x, input.Tex.y);
	printf("frame time     = <%f>", g_FrameTime);
	printf("----------------------------------------------------");

	input.PositionLocal.w = 1.0f;
	output.PositionClip = mul(input.PositionLocal, g_World);
	output.PositionClip = mul(output.PositionClip, g_View);
	output.PositionClip = mul(output.PositionClip, g_Proj);

	output.ColorTex = input.Tex;
	output.NoiseOctave0 = input.Tex * g_Scales.x;
	output.NoiseOctave1 = input.Tex * g_Scales.y;
	output.NoiseOctave2 = input.Tex * g_Scales.z;

	output.NoiseOctave0.y = output.NoiseOctave0.y + (g_FrameTime * g_ScrollSpeeds.x);
	output.NoiseOctave1.y = output.NoiseOctave1.y + (g_FrameTime * g_ScrollSpeeds.y);
	output.NoiseOctave2.y = output.NoiseOctave2.y + (g_FrameTime * g_ScrollSpeeds.z);

	/*printf("----------------------------------------------------");
	printf("sclale       x = <%f>", g_Scales.x);
	printf("sclale       y = <%f>", g_Scales.y);
	printf("sclale       z = <%f>", g_Scales.z);
	printf("octave       0 = <%f %f>", output.NoiseOctave0.x, output.NoiseOctave0.y);
	printf("octave       1 = <%f %f>", output.NoiseOctave1.x, output.NoiseOctave1.y);
	printf("octave       2 = <%f %f>", output.NoiseOctave2.x, output.NoiseOctave2.y);
	printf("scroll speed x = <%f>", g_ScrollSpeeds.x);
	printf("scroll speed y = <%f>", g_ScrollSpeeds.y);
	printf("scroll speed z = <%f>", g_ScrollSpeeds.z);
	printf("----------------------------------------------------");*/


	return output;
}