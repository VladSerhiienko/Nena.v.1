#define SimulationBlockSize			(256)
#define SimulationBlockSizeSqr		(256*256)
#define SimulationBlockSizeCube		(256*256*256)
#define SimulationGridIndexMin		(0.0f)
#define SimulationGridIndexMax		(255.0f)
#define GridGetValue( uPair )		(uint)(uPair & 0xffff) // & 0xffff
#define GridGetKey( uPair )			(uint)(uPair >> 0x10) // (>> 16) & 65k

struct Particle
{
    float2 Position;
    float2 Velocity;
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

RWStructuredBuffer<uint2> GridIndicesRW : register( u0 );
StructuredBuffer<uint> GridRO : register( t0 );

[numthreads(SimulationBlockSize, 1, 1)]
void main( 
	in uint3 u3GroupId			: SV_GroupID, 
	in uint3 u3ThreadId			: SV_DispatchThreadID, 
	in uint3 u3GroupThreadId	: SV_GroupThreadID, 
	in uint uGroupIndex			: SV_GroupIndex 
	)
{

	//printf( "------------------------------------------------------" );
	//printf( "Build grid indices call" );

	const uint uThisGridId = u3ThreadId.x; // This is the grid id we will operate on for now
	const uint uPrevGridId = (uThisGridId == 0) // If this is the first cell 
		? g_iNumParticles - 1 // then the previous one is the last one
		: uThisGridId - 1; // otherwise its just the previous one  
	const uint uNextGridId = ((uThisGridId + 1) != g_iNumParticles) // If its not the last id 
		? uThisGridId + 1 // then the next one it trivially just the next grid id 
		: 0; // otherwise its the first grid id 

	printf( "Grid id: this %d prev %d next %d", uThisGridId, uPrevGridId, uNextGridId );

	// Extract the grid cell keys
	const uint uThisCellKey = GridGetKey(GridRO[uThisGridId]);
	const uint uPrevCellKey = GridGetKey(GridRO[uPrevGridId]);
	const uint uNextCellKey = GridGetKey(GridRO[uNextGridId]);

	printf( "Grid key: this %d prev %d next %d", uThisCellKey, uPrevCellKey, uNextCellKey );

	if ( uThisCellKey != uPrevCellKey ) // If the keys a distinct
		GridIndicesRW[uThisCellKey].x = // then the current cell 
			uThisGridId; // is the cell start 

	if ( uThisCellKey != uNextCellKey ) // If the keys a distinct
		GridIndicesRW[uThisCellKey].y = // then the current cell
			uThisGridId + 1; // is the cell end

	printf( "Grid indices: x %d y %d", GridIndicesRW[uThisCellKey].x, GridIndicesRW[uThisCellKey].y );
	//printf( "------------------------------------------------------" );
}