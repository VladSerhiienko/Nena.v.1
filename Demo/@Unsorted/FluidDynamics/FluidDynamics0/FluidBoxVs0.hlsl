
//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

struct VertexShaderInput
{
	float4 PositionLocal				: Position;
	float2 Tex							: TexCoord0;
};

struct PixelShaderInput
{
	float4 PositionClip					: SV_Position;
	float2 ColorTex						: TexCoord0;
};

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

cbuffer Spatial : register ( cb0 )
{
	row_major float4x4 g_World;
	row_major float4x4 g_View;
	row_major float4x4 g_Proj;
};

//-----------------------------------------------------------------------------
// Shader
//-----------------------------------------------------------------------------

PixelShaderInput main( in VertexShaderInput input, in const uint vertexId : SV_VERTEXID )
{

	PixelShaderInput output = (PixelShaderInput) 0;
	//printf( "vertex id      = <%d>", vertexId );

	/*printf( "CPU::FluidBox::VertexShaderCall" );
	printf( "----------------------------------------------------" );
	printf( "vertex id      = <%d>", vertexId );
	printf( "input position = <%f %f %f>", input.PositionLocal.x, input.PositionLocal.y, input.PositionLocal.z );
	printf( "input tex      = <%f %f>", input.Tex.x, input.Tex.y );
	printf( "----------------------------------------------------" );
	printf( "World: %f %f %f %f", g_World[ 0 ][ 0 ], g_World[ 0 ][ 1 ], g_World[ 0 ][ 2 ], g_World[ 0 ][ 3 ] );
	printf( "       %f %f %f %f", g_World[ 1 ][ 0 ], g_World[ 1 ][ 1 ], g_World[ 1 ][ 2 ], g_World[ 1 ][ 3 ] );
	printf( "       %f %f %f %f", g_World[ 2 ][ 0 ], g_World[ 2 ][ 1 ], g_World[ 2 ][ 2 ], g_World[ 2 ][ 3 ] );
	printf( "       %f %f %f %f", g_World[ 3 ][ 0 ], g_World[ 3 ][ 1 ], g_World[ 3 ][ 2 ], g_World[ 3 ][ 3 ] );
	printf( "----------------------------------------------------" );
	printf( "View : %f %f %f %f", g_View[ 0 ][ 0 ], g_View[ 0 ][ 1 ], g_View[ 0 ][ 2 ], g_View[ 0 ][ 3 ] );
	printf( "       %f %f %f %f", g_View[ 1 ][ 0 ], g_View[ 1 ][ 1 ], g_View[ 1 ][ 2 ], g_View[ 1 ][ 3 ] );
	printf( "       %f %f %f %f", g_View[ 2 ][ 0 ], g_View[ 2 ][ 1 ], g_View[ 2 ][ 2 ], g_View[ 2 ][ 3 ] );
	printf( "       %f %f %f %f", g_View[ 3 ][ 0 ], g_View[ 3 ][ 1 ], g_View[ 3 ][ 2 ], g_View[ 3 ][ 3 ] );
	printf( "----------------------------------------------------" );
	printf( "Proj : %f %f %f %f", g_Proj[ 0 ][ 0 ], g_Proj[ 0 ][ 1 ], g_Proj[ 0 ][ 2 ], g_Proj[ 0 ][ 3 ] );
	printf( "       %f %f %f %f", g_Proj[ 1 ][ 0 ], g_Proj[ 1 ][ 1 ], g_Proj[ 1 ][ 2 ], g_Proj[ 1 ][ 3 ] );
	printf( "       %f %f %f %f", g_Proj[ 2 ][ 0 ], g_Proj[ 2 ][ 1 ], g_Proj[ 2 ][ 2 ], g_Proj[ 2 ][ 3 ] );
	printf( "       %f %f %f %f", g_Proj[ 3 ][ 0 ], g_Proj[ 3 ][ 1 ], g_Proj[ 3 ][ 2 ], g_Proj[ 3 ][ 3 ] );
	*/

	input.PositionLocal.w = 1.0f;
	output.PositionClip = mul( input.PositionLocal, g_World );
	output.PositionClip = mul( output.PositionClip, g_View );
	output.PositionClip = mul( output.PositionClip, g_Proj );
	output.ColorTex = input.Tex;

	return output;
};