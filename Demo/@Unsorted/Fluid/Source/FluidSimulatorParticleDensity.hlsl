#define SimulationBlockSize			(256)
#define SimulationBlockSizeSqr		(256*256)
#define SimulationBlockSizeCube		(256*256*256)
#define SimulationGridIndexMin		(0.0f)
#define SimulationGridIndexMinAsInt	(0)
#define SimulationGridIndexMax		(255.0f)
#define SimulationGridIndexMaxAsInt	(255)

struct Particle
{
    float2 Position;
    float2 Velocity;
};

struct ParticleDensity
{
    float Density;
};

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

RWStructuredBuffer<ParticleDensity> ParticlesDensityRW : register( u0 );
StructuredBuffer<Particle> ParticlesRO : register( t0 );
StructuredBuffer<uint2> GridIndicesRO : register( t1 );


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

float CalculateDensity( 
	in float fSmoothLengthSqr, 
	in float f2DistanceLengthSqr 
	)
{
	const float fWPoly6 = pow((fSmoothLengthSqr - f2DistanceLengthSqr), 3);
	return g_fDensityCoef * fWPoly6;
}

[numthreads(SimulationBlockSize, 1, 1)]
void main( 
	in uint3 u3GroupId			: SV_GroupID, 
	in uint3 u3ThreadId			: SV_DispatchThreadID, 
	in uint3 u3GroupThreadId	: SV_GroupThreadID, 
	in uint uGroupIndex			: SV_GroupIndex 
	)
{
	//
	//printf( "------------------------------------------------------" );
	//printf( "DENSITY %d \t\t", u3ThreadId.x );
	//

	const float fSmoothLengthSqr = g_fSmoothlen * g_fSmoothlen;

	const uint uParticleId = u3ThreadId.x;
	const float2 f2ParticlePosition = ParticlesRO[uParticleId].Position;

	const float2 f2GridCoords = (GridCalculateCell(f2ParticlePosition));
	const int2 i2GridCoords = (int2)(GridCalculateCell(f2ParticlePosition));

	const int iYGridStart = max(i2GridCoords.y - 1, 0);
	const int iXGridStart = max(i2GridCoords.x - 1, 0);
	const int iYGridMax = min(i2GridCoords.y + 1, 255);
	const int iXGridMax = min(i2GridCoords.x + 1, 255);

	//
	//printf( "Density coef %f \t\t", g_fDensityCoef );
	//printf( "Particles[%d].Position.x = %f \t\t", uParticleId, f2ParticlePosition.x );
	//printf( "Particles[%d].Position.y = %f \t\t", uParticleId, f2ParticlePosition.y );
	//printf( "GridCoords.x = %f %d \t\t", f2GridCoords.x, i2GridCoords.x );
	//printf( "GridCoords.y = %f %d \t\t", f2GridCoords.y,  i2GridCoords.y );
	//printf( "Grid x [min %d max %d] \t\t", iXGridStart, iXGridMax );
	//printf( "Grid y [min %d max %d] \t\t", iYGridStart, iYGridMax );
	//

	float fDensity = 0.0f;

	for ( int iY = iYGridStart; iY <= iYGridMax; iY++ )
	for ( int iX = iXGridStart; iX <= iXGridMax; iX++ )
	{
		const int iGridCell = GridConstuctKey( uint2(iX, iY) );
		const uint2 i2GridCell = GridIndicesRO[iGridCell];
		const uint iGridCellStart = i2GridCell.x;
		const uint iGridCellEnd = i2GridCell.y;

		//printf( "Key %d Grid Cell [s %d e %d] \t x %d y %d \t\t", iGridCell, iGridCellStart, iGridCellEnd, iX, iY );

		// unroll ?
		for ( uint iAdjCell = iGridCellStart; iAdjCell < iGridCellEnd; iAdjCell++ )
		{
			const float2 f2AdjCellPosition = ParticlesRO[iAdjCell].Position;
			const float2 f2DistanceVector = f2AdjCellPosition - f2ParticlePosition;
			const float f2DistanceLengthSqr = dot(f2DistanceVector, f2DistanceVector);
			
			//printf( "f2DistanceVector %f %f \t\t", f2DistanceVector.x, f2DistanceVector.y );
			//printf( "f2DistanceLengthSqr %f fSmoothLengthSqr %f \t\t", f2DistanceLengthSqr, fSmoothLengthSqr );
			//printf( "AdjParticles[%d].Position.x = %f \t\t", iAdjCell, f2AdjCellPosition.x );
			//printf( "AdjParticles[%d].Position.y = %f \t\t", iAdjCell, f2AdjCellPosition.y );

			if ( f2DistanceLengthSqr < fSmoothLengthSqr ) 
			{
				fDensity += CalculateDensity( 
					fSmoothLengthSqr, f2DistanceLengthSqr
					);

				//printf( "Density updated %f \t\t", fDensity );
			}
		}
	}

	//
	//printf( "Old Particle[%d].Density = %f", uParticleId, ParticlesDensityRW[uParticleId].Density );
	//

	ParticlesDensityRW[uParticleId].Density = fDensity == 0.0f ? 0.1f : fDensity;

	//
	//printf( "New Particle[%d].Density = %f \t\t", uParticleId, ParticlesDensityRW[uParticleId].Density );
	//printf( "------------------------------------------------------" );
	//
}