#define SimulationBlockSize			(256)
#define SimulationBlockSizeSqr		(256*256)
#define SimulationBlockSizeCube		(256*256*256)
#define SimulationGridIndexMin		(0.0f)
#define SimulationGridIndexMax		(255.0f)

cbuffer PerFrame : register( cb0 )
{
    uint g_iNumParticles;
    float g_fTimeStep;
    float g_fSmoothlen;
    float g_fPressureStiffness;
    float g_fRestDensity;
    float g_fDensityCoef;
    float g_fGradPressureCoef;
    float g_fLapViscosityCoef;
    float g_fWallStiffness;
    float4 g_vGravity;
    float4 g_vGridDim;
    float3 g_vPlanes[4];
};

struct Particle
{
    float2 Position;
    float2 Velocity;
};

StructuredBuffer<Particle> ParticlesRO	: register( t0 );
RWStructuredBuffer<uint> GridRW			: register( u0 );

float2 GridCalculateCell( in float2 Position )
{
	return clamp(
		Position * g_vGridDim.xy + g_vGridDim.zw, // Value to clamp with min/max 
		float2(0, 0), // (0, 0)
		float2(255, 255)
		);
}

uint GridConstuctKey( in uint2 i2GridCoords )
{
    return dot(
		i2GridCoords.yx, 
		uint2(256, 1)
		);
}

// What we do here is generating the number which is 
// [00 - 15]	= 16 bits	= Value
// [16 - 23]	=  8 bits	= X Coord
// [24 - 31]	=  8 bits	= Y Coords

// To get the key-value-pair key (xy actually) we need lshift it
// = key-value-pair >> 0x10 // 16
// To get the key-value-pair value we need to saturate it
// = key-value-pair & 0xffff

#define GridGetValue( uPair )	(uPair & 0xffff) // &
#define GridGetKey( uPair )		(uPair >> 16) // << 16

uint GridConstuctKeyValuePair( 
	in uint2 u2Coords, // Key 
	in uint uValue // Value 
	)
{
	return dot( 
		uint3( u2Coords.yx, uValue ), 
		uint3( 256*256*256, 256*256, 1 ) 
		);
}

[numthreads(SimulationBlockSize, 1, 1)]
void main( 
	in uint3 u3GroupId			: SV_GroupID, 
	in uint3 u3ThreadId			: SV_DispatchThreadID, 
	in uint3 u3GroupThreadId	: SV_GroupThreadID, 
	in uint uGroupIndex			: SV_GroupIndex 
	)
{
	/*printf( "------------------------------------------------------" );
	printf( "Build grid call" );*/

	const uint uParticleId = u3ThreadId.x;
	const float2 Position = ParticlesRO[uParticleId].Position;
	const uint2 u2GridCoords = (uint2)(GridCalculateCell(Position));

	printf( "Particles[%d].Position = [%f %f]", uParticleId, Position.x, Position.y );
	printf( "GridCoords = [%d %d]", u2GridCoords.x, u2GridCoords.y );

	GridRW[uParticleId] = GridConstuctKeyValuePair( 
		u2GridCoords, 
		uParticleId 
		);

	printf( "Grid[%d] = %d", uParticleId, GridRW[uParticleId] );
	//printf( "------------------------------------------------------" );
}