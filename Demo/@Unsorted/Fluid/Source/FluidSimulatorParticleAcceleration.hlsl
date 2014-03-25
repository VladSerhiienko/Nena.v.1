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

struct ParticleAcceleration
{
    float2 Acceleration;
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

float2 GridCalculateCell( in float2 Position )
{
	const float2 MinValue = float2(
		SimulationGridIndexMin, 
		SimulationGridIndexMin
		);	// 0	
	const float2 MaxValue = float2(
		SimulationGridIndexMax, 
		SimulationGridIndexMax
		); // 255

	const float2 ThisValue = 
		// Particle position * 
		Position * g_vGridDim.xy + // (1/SmoothLength, 1/SmoothLength) + 
		g_vGridDim.zw; // (0,0)

	return clamp(
		ThisValue, // Value to clamp with min/max 
		MinValue, // (0, 0)
		MaxValue // (255, 255)
		);
}

uint GridConstuctKey( in uint2 i2GridCoords )
{
    return dot(
		i2GridCoords.yx, 
		uint2(256, 1)
		);
}

float CalculatePressure( in float fDensity )
{
	const float fRhoDivRho0PowY = max(
		pow( fDensity / g_fRestDensity, 3 ) - 1, 0 
		);

	return g_fPressureStiffness * fRhoDivRho0PowY;
}

float2 CalculateGradPressure( 
	in float fDistanceLength, 
	in float fParticlePressure,
	in float fNeighbourPressure, 
	in float fNeighbourDensity,
	in float2 f2DistanceVector
	)
{
	const float fAvgPressure = (fParticlePressure + fNeighbourPressure) / 2.0f;
	const float SqrOfHMinusR = pow((g_fSmoothlen - fDistanceLength), 2);
	return (g_fGradPressureCoef * fAvgPressure / fNeighbourDensity) * 
		(SqrOfHMinusR / fDistanceLength) * 
		(f2DistanceVector);
}

float2 CalculateLapVelocity( 
	in float fDistanceLength, 
	in float2 f2ParticleVelocity,
	in float2 f2NeighbourVelocity,
	in float fNeighbourDensity
	)
{
	const float2 f2VelocityDelta = (f2NeighbourVelocity - f2ParticleVelocity);
	return (g_fLapViscosityCoef / fNeighbourDensity) * 
		(g_fSmoothlen - fDistanceLength) * 
		(f2VelocityDelta);
}

StructuredBuffer<Particle> ParticlesRO : register( t0 );
StructuredBuffer<ParticleDensity> ParticlesDensityRO : register( t1 );
StructuredBuffer<uint2> GridIndicesRO : register( t2 );
RWStructuredBuffer<ParticleAcceleration> ParticlesAccelerationRW : register( u0 );

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
	//printf( "ACCELERATION %d \t\t", u3ThreadId.x);
	//

	const uint uParticleId = u3ThreadId.x;
	const float fSmoothLengthSqr = g_fSmoothlen * g_fSmoothlen;

	const float2 f2ParticlePosition = ParticlesRO[uParticleId].Position;
	const float2 f2ParticleVelocity = ParticlesRO[uParticleId].Velocity;
	const float fParticleDensity = ParticlesDensityRO[uParticleId].Density;
	const float fParticlePressure = CalculatePressure( fParticleDensity );

	const float2 f2GridCoords = (GridCalculateCell(f2ParticlePosition));
	const int2 i2GridCoords = (int2)(GridCalculateCell(f2ParticlePosition));
	const int iYGridStart = max(i2GridCoords.y - 1, SimulationGridIndexMinAsInt);
	const int iXGridStart = max(i2GridCoords.x - 1, SimulationGridIndexMinAsInt);
	const int iYGridMax = min(i2GridCoords.y + 1, SimulationGridIndexMaxAsInt);
	const int iXGridMax = min(i2GridCoords.x + 1, SimulationGridIndexMaxAsInt);

	//
	//printf( "Particle id %d", uParticleId );
	//printf( "SmoothLenSqr %f", fSmoothLengthSqr );
	//printf( "Density %f \t\t", fParticleDensity );
	//printf( "Pressure %f \t\t", fParticlePressure );
	//printf( "Particles[%d].Position.x = %f \t\t", uParticleId, f2ParticlePosition.x );
	//printf( "Particles[%d].Position.y = %f \t\t", uParticleId, f2ParticlePosition.y );
	//printf( "Particles[%d].Velocity.x = %f \t\t", uParticleId, f2ParticleVelocity.x );
	//printf( "Particles[%d].Velocity.y = %f \t\t", uParticleId, f2ParticleVelocity.y );
	//printf( "GridCoords.x = %f %d \t\t", f2GridCoords.x, i2GridCoords.x );
	//printf( "GridCoords.y = %f %d \t\t", f2GridCoords.y,  i2GridCoords.y );
	//printf( "Grid x [min %d max %d] \t\t", iXGridStart, iXGridMax );
	//printf( "Grid y [min %d max %d] \t\t", iYGridStart, iYGridMax );
	//

	float2 f2Accel = float2(0.00001f, 0.000001f);

	for ( int iY = iYGridStart; iY <= iYGridMax; iY++ )
	for ( int iX = iXGridStart; iX <= iXGridMax; iX++ )
	{
		const int iGridCell = GridConstuctKey( uint2(iX, iY) );
		const uint2 i2GridCell = GridIndicesRO[iGridCell];
		const uint iGridCellStart = i2GridCell.x;
		const uint iGridCellEnd = i2GridCell.y;

		printf( "Key %d x %d y %d Grid cell [s %d e %d] \t\t", iGridCell, iX, iY, iGridCellStart, iGridCellEnd );

		// unroll ?
		for ( uint iAdjCell = iGridCellStart; iAdjCell < iGridCellEnd; iAdjCell++ )
		{
			const float2 f2NeighbourPosition = ParticlesRO[iAdjCell].Position;
			const float2 f2DistanceVector = (f2NeighbourPosition - f2ParticlePosition);
			const float fDistanceLengthSqr = dot(f2DistanceVector, f2DistanceVector);

			if ((fDistanceLengthSqr < fSmoothLengthSqr) && 
				(uParticleId != iAdjCell))
			{
				const float2 f2NeighbourVelocity = ParticlesRO[iAdjCell].Velocity;
				const float fNeighbourDensity = ParticlesDensityRO[iAdjCell].Density;
				const float fNeighbourPressure = CalculatePressure(fNeighbourDensity);
				const float fDistanceLength = sqrt(fDistanceLengthSqr);

				f2Accel += CalculateGradPressure(
					fDistanceLength, 
					fParticlePressure, 
					fNeighbourPressure,
					fNeighbourDensity, 
					f2DistanceVector
					);

				f2Accel += CalculateLapVelocity(
					fDistanceLength, 
					f2ParticleVelocity,
					f2NeighbourVelocity,
					fNeighbourDensity
					);

				//printf( "Acceleration updated %f %f \t\t", f2Accel.x, f2Accel.y );

			}
		}
	}

	//
	//printf( "Old Particles[%d].Acceleration.x = %f \t\t", uParticleId, ParticlesAccelerationRW[uParticleId].Acceleration.x );
	//printf( "Old Particles[%d].Acceleration.y = %f \t\t", uParticleId, ParticlesAccelerationRW[uParticleId].Acceleration.y );
	//

	ParticlesAccelerationRW[uParticleId].Acceleration = 
		f2Accel / fParticleDensity;

	//
	//printf( "New Particles[%d].Acceleration.x = %f \t\t", uParticleId, ParticlesAccelerationRW[uParticleId].Acceleration.x );
	//printf( "New Particles[%d].Acceleration.y = %f \t\t", uParticleId, ParticlesAccelerationRW[uParticleId].Acceleration.y );
	//printf( "------------------------------------------------------" );
	//
}