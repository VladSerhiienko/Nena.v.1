struct PixelShaderInput
{
	float4 PositionWorld : POSITION;
	float4 NormalWorld : NORMAL;
	float2 TexCoord : TEXCOORD0;
	int TexId : TEXCOORD1;
	float4 PositionClip : SV_POSITION;
};

struct Vertex
{
	float4 PositionWorld;
	float4 NormalWorld;
	float2 TexCoord;
	int TexId;
};

cbuffer Spatial : register (cb0)
{
	row_major float4x4 g_World;
	row_major float4x4 g_View;
	row_major float4x4 g_Proj;
};

StructuredBuffer<Vertex> g_Vertices : register (t0);

PixelShaderInput main( in uint vertexId	: SV_VERTEXID )
{
	PixelShaderInput output = (PixelShaderInput)0;
	Vertex input = g_Vertices[vertexId];

	printf("vertex id = %d", 
		vertexId
		);
	printf("world position = <%f %f %f>", 
		input.PositionWorld.x,
		input.PositionWorld.y,
		input.PositionWorld.z
		);
	/*printf("world normal = <%f %f %f>", 
		input.NormalWorld.x,
		input.NormalWorld.y,
		input.NormalWorld.z
		);

	printf( "World: %f %f %f %f", g_World[0][0], g_World[0][1], g_World[0][2], g_World[0][3]);
	printf( "       %f %f %f %f", g_World[1][0], g_World[1][1], g_World[1][2], g_World[1][3]);
	printf( "       %f %f %f %f", g_World[2][0], g_World[2][1], g_World[2][2], g_World[2][3]);
	printf( "       %f %f %f %f", g_World[3][0], g_World[3][1], g_World[3][2], g_World[3][3]);
	
	printf( "View : %f %f %f %f", g_View[0][0], g_View[0][1], g_View[0][2], g_View[0][3]);
	printf( "       %f %f %f %f", g_View[1][0], g_View[1][1], g_View[1][2], g_View[1][3]);
	printf( "       %f %f %f %f", g_View[2][0], g_View[2][1], g_View[2][2], g_View[2][3]);
	printf( "       %f %f %f %f", g_View[3][0], g_View[3][1], g_View[3][2], g_View[3][3]);
	
	printf( "Proj : %f %f %f %f", g_Proj[0][0], g_Proj[0][1], g_Proj[0][2], g_Proj[0][3]);
	printf( "       %f %f %f %f", g_Proj[1][0], g_Proj[1][1], g_Proj[1][2], g_Proj[1][3]);
	printf( "       %f %f %f %f", g_Proj[2][0], g_Proj[2][1], g_Proj[2][2], g_Proj[2][3]);
	printf( "       %f %f %f %f", g_Proj[3][0], g_Proj[3][1], g_Proj[3][2], g_Proj[3][3]);*/

	output.PositionWorld = mul(input.PositionWorld, g_World);
	output.PositionClip = output.PositionWorld;
	output.PositionClip = mul(output.PositionClip, g_View);
	output.PositionClip = mul(output.PositionClip, g_Proj);
	output.NormalWorld = input.NormalWorld;
	output.TexCoord = input.TexCoord;
	output.TexId = input.TexId;

	return output;
}