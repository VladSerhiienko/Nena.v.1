#define SimulationBlockSize	(256)

RWStructuredBuffer<uint2> GridIndicesRW : register( u0 );

[numthreads(SimulationBlockSize, 1, 1)]
void main( in uint3 u3ThreadId : SV_DispatchThreadID )
{
	//printf( "Clear grid indices call" );
	GridIndicesRW[u3ThreadId.x] = uint2(0, 0);
}