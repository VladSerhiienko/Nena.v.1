#define SimulationBlockSize			(256)
#define GridGetValue( uPair )		(uPair & 0xffff) // & ushort_max

struct Particle
{
    float2 Position;
    float2 Velocity;
};

RWStructuredBuffer<Particle> ParticlesRW : register( u0 );
StructuredBuffer<Particle> ParticlesRO	: register( t0 );
StructuredBuffer<uint> GridRO : register( t1 );

[numthreads(SimulationBlockSize, 1, 1)]
void main( in uint3 u3ThreadId : SV_DispatchThreadID )
{
	//printf( "------------------------------------------------------" );
	//printf( "Rearrange particles call" );

	const uint uParticleId = u3ThreadId.x;
	const uint uGridId = GridGetValue( GridRO[uParticleId] );

	/*printf( "Grid id %d", uGridId );
	printf( "Particle id %d", uParticleId );
	printf( "Old Particle[%d].Position.x = %f", uParticleId, ParticlesRW[uParticleId].Position.x );
	printf( "Old Particle[%d].Velocity.x = %f", uParticleId, ParticlesRW[uParticleId].Velocity.x );
	printf( "Old Particle[%d].Position.y = %f", uParticleId, ParticlesRW[uParticleId].Position.y );
	printf( "Old Particle[%d].Velocity.y = %f", uParticleId, ParticlesRW[uParticleId].Velocity.y );*/

	ParticlesRW[uParticleId] = ParticlesRO[uGridId];

	/*printf( "New Particle[%d].Position.x = %f", uParticleId, ParticlesRW[uParticleId].Position.x );
	printf( "New Particle[%d].Velocity.x = %f", uParticleId, ParticlesRW[uParticleId].Velocity.x );
	printf( "New Particle[%d].Position.y = %f", uParticleId, ParticlesRW[uParticleId].Position.y );
	printf( "New Particle[%d].Velocity.y = %f", uParticleId, ParticlesRW[uParticleId].Velocity.y );
	printf( "------------------------------------------------------" );*/
}