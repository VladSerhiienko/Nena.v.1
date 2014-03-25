struct GeometryShaderInput			// VertexShaderOutput
{
	float2 Position		: POSITION;	// Particle position from the input buffer.
	float4 Color		: COLOR;	// Generated particle color (from calculated fluid density in particle location).		 
};

struct GeometryShaderOutput
{
	float4 Color		: COLOR;
	float4 Position		: SV_POSITION;
	//float2 TexCoord		: TEXCOORD;
};

cbuffer PerFrame : register ( cb0 )
{
	// Those values are static unless the user wants to move cam 
	row_major float4x4 g_mViewProj; 
	float g_fParticleSize;
};

cbuffer PerShader : register ( cb1 )
{
	// Those static buffers are used to generate quad (2 tris) 
	// from the single vertex shader output entry as well the 
	// particle tris texture coordinates for each tri vertex. 
	
	static const float2 g_Positions[4] = 
	{ 
		float2( -1,	+1 ), 
		float2( +1,	+1 ), 
		float2( -1,	-1 ), 
		float2( +1,	-1 ), 
	};

	/*static const float2 g_TexCoords[4] = 
	{ 
		float2( 0, 1 ), 
		float2( 1, 1 ), 
		float2( 0, 0 ), 
		float2( 1, 0 ) 
	};*/
};

[maxvertexcount(4)]
void main(
	in point GeometryShaderInput Input[1], // Vertex shader output 
	inout TriangleStream<GeometryShaderOutput> OutputStream // Output tri stream
	)
{
	//
	//printf( "Gs call" );
	//printf( "View: %f %f %f %f", g_mViewProj[0][0], g_mViewProj[0][1], g_mViewProj[0][2], g_mViewProj[0][3]);
	//printf( "      %f %f %f %f", g_mViewProj[1][0], g_mViewProj[1][1], g_mViewProj[1][2], g_mViewProj[1][3]);
	//printf( "      %f %f %f %f", g_mViewProj[2][0], g_mViewProj[2][1], g_mViewProj[2][2], g_mViewProj[2][3]);
	//printf( "      %f %f %f %f", g_mViewProj[3][0], g_mViewProj[3][1], g_mViewProj[3][2], g_mViewProj[3][3]);
	//

	// Store the values to the local variable to reduce the number of accesses
	const float4 ParticlePosition = float4(Input[0].Position, 0, 1); // w = 1 !
	const float4 ParticleColor = Input[0].Color;

	[unroll] for ( uint uVertex = 0; uVertex < 4; uVertex++ )
	{
		const float4 PositionOffset = g_fParticleSize * float4(g_Positions[uVertex], 0, 0); // Inflame and pad the particle sprite vertex position
		const float4 Position = ParticlePosition + PositionOffset; // Calculate the final particle sprite vertex position 

		GeometryShaderOutput OutputEntry;					// Create tri stream item 
		OutputEntry.Position = mul(Position, g_mViewProj);	// Sprite vertex position to the clip space 
		OutputEntry.Color = ParticleColor;					// Store particle color 
		//OutputEntry.TexCoords = g_TexCoords[uVertex];		// Store particle texture coord 

		OutputStream.Append(OutputEntry);					// Append to sprite stream 
	}

	// Flush tri stream since the particle sprite is generated. 
	// All the other vertices will generate the new sprite tris. 
	OutputStream.RestartStrip(); 
}