struct Particle
{
	float2 Position; // Particle location
	float2 Velocity; // Particle accumulated velocity
};

struct ParticleDensity
{
	float Density;
};

StructuredBuffer<Particle> ParticlesRO					: register ( t0 ); // Updated particle positions  
StructuredBuffer<ParticleDensity> ParticleDensityRO		: register ( t1 ); // Update fluid density in particle locations

cbuffer PerShader										: register ( cb0 ) // Static buffer
{
	static const float4 g_ParticleColors[5] = 
	{
		float4( 0.3,	0.3,	0.3,	1 ),		// the updated fluid density in the 
		float4( 0.6,	0.6,	0.6,	1 ),		// in the vertex shader according to 
		float4( 0.7,	0.7,	0.7,	1 ),		// to generate final particle color 
		float4( 0.4,	0.4,	0.4,	0 ),		// current particle position.
		float4( 0.8,	0.8,	0.8,	1 ),		// Those vector4 values will be used 
	};
};

struct VertexShaderOutput 
{
	// Particle position from the input buffer.
	float2 Position	: POSITION; 
	// Generated particle color (from calculated fluid density in particle location).
	float4 Color : COLOR;		
};

VertexShaderOutput main( 
	uint uVertex : SV_VertexID // Store the VS vertex index
	)
{
	//
	printf( "Vs call" );
	//

	const float ParticleDensity = ParticleDensityRO[uVertex].Density; // Get the fluid density in a certain location (particle location)
	const float DensityColor = 4.0f * saturate( (ParticleDensity - 1000.0f) / 1000.0f ); // Get the approximate density color index. 
	const float4 f4FirstColor = g_ParticleColors[ floor(DensityColor) ]; // Floor the density index (to lower int value)
	const float4 f4SecondColor = g_ParticleColors[ ceil(DensityColor) ]; // Ceil the density index (to greater int value)
	const float fLerpRatio	= frac(DensityColor); // Get fraction value (0, 1) // Remove int part

	VertexShaderOutput Output; // Copy particle position to propagate in to the geomerty shader
	Output.Position = ParticlesRO[uVertex].Position; // where the tri stream should be generated.
	Output.Color = lerp(f4FirstColor, f4SecondColor, fLerpRatio); // Interpolate the output particle color.

	printf( "Id %d Position %f %f", uVertex, Output.Position.x, Output.Position.y );
	//printf( "      Color    %f %f %f %f", Output.Color.x, Output.Color.y, Output.Color.z, Output.Color.w );

	return Output; // Pass further 
}