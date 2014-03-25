#define SimulationBlockSize			(256)

struct Particle
{
    float2 Position;
    float2 Velocity;
};

struct ParticleAcceleration
{
    float2 Acceleration;
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

RWStructuredBuffer<Particle> ParticlesRW						: register( u0 ); // Particles` buffer 
StructuredBuffer<Particle> ParticlesRO							: register( t0 ); // Sorted particles` buffer
StructuredBuffer<ParticleAcceleration> ParticlesAccelerationRO	: register( t1 ); // Updated particles` acceleration

[numthreads(SimulationBlockSize, 1, 1)]
void main( in uint3 u3ThreadId : SV_DispatchThreadID )
{
	//
	//printf( "------------------------------------------------------" );
	//printf( "Accumulate call" );
	//

	const uint uParticleId = u3ThreadId.x;

	float2 f2Accel = ParticlesAccelerationRO[uParticleId].Acceleration;
	float2 f2Position = ParticlesRO[uParticleId].Position;
	float2 f2Velocity = ParticlesRO[uParticleId].Velocity;

	for ( uint uWallPlane = 0; uWallPlane < 4; uWallPlane++ )
	{
		const float3 f3WallPlane = g_vPlanes[uWallPlane];
		const float fDistance = dot( 
			(float3(f2Position, 1)), 
			(f3WallPlane) 
			);

		f2Accel += min(fDistance, 0) * -g_fWallStiffness * f3WallPlane.xy;
	}

	f2Accel += g_vGravity.xy;
	f2Velocity += g_fTimeStep * f2Accel;
	f2Position += g_fTimeStep * f2Velocity;

	//
	//printf( "Old Particles[%d].Position.x = %f", uParticleId, ParticlesRW[uParticleId].Position.x );
	//printf( "Old Particles[%d].Position.y = %f", uParticleId, ParticlesRW[uParticleId].Position.y );
	//printf( "Old Particles[%d].Velocity.x = %f", uParticleId, ParticlesRW[uParticleId].Velocity.x );
	//printf( "Old Particles[%d].Velocity.y = %f", uParticleId, ParticlesRW[uParticleId].Velocity.y );
	//

	ParticlesRW[uParticleId].Position = f2Position;
	ParticlesRW[uParticleId].Velocity = f2Velocity;

	//
	//printf( "New Particles[%d].Position.x = %f", uParticleId, ParticlesRW[uParticleId].Position.x );
	//printf( "New Particles[%d].Position.y = %f", uParticleId, ParticlesRW[uParticleId].Position.y );
	//printf( "New Particles[%d].Velocity.x = %f", uParticleId, ParticlesRW[uParticleId].Velocity.x );
	//printf( "New Particles[%d].Velocity.y = %f", uParticleId, ParticlesRW[uParticleId].Velocity.y );
	//printf( "------------------------------------------------------" );
	//
}