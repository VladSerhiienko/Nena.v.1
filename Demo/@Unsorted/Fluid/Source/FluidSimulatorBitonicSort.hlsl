#define BitonicBlockSize	512

cbuffer PerFrame				: register (cb0)
{
    uint g_iLevel;
    uint g_iLevelMask;
    uint g_iWidth;
    uint g_iHeight;
};

StructuredBuffer<uint> Input	: register( t0 );
RWStructuredBuffer<uint> Data	: register( u0 );

groupshared uint PerGroupSharedData[BitonicBlockSize];

[numthreads(BitonicBlockSize, 1, 1)]
void main( 
	uint3 u3GroupId				: SV_GroupID, 
    uint3 u3ThreadId			: SV_DispatchThreadID, 
    uint3 u3GroupThreadId		: SV_GroupThreadID, 
    uint uGroupIndex			: SV_GroupIndex 
	)
{
	/*printf( "------------------------------------------------------" );
	printf( "Bitonic sort call" );
	printf( "Groud index %d Level %d W %d H %d", uGroupIndex, g_iLevel, g_iWidth, g_iHeight );
	printf( "Thread index %d Init Data %d", u3ThreadId.x, Data[u3ThreadId.x] );*/

	PerGroupSharedData[uGroupIndex] = Data[u3ThreadId.x]; // Data
    GroupMemoryBarrierWithGroupSync();

	for ( uint j = g_iLevel >> 1 ; j > 0 ; j >>= 1)
    {
		bool bResult = ((PerGroupSharedData[uGroupIndex & ~j] <= PerGroupSharedData[uGroupIndex | j]) == (bool)(g_iLevelMask & u3ThreadId.x));
        uint uResult = bResult ? PerGroupSharedData[uGroupIndex ^ j] : PerGroupSharedData[uGroupIndex];
        GroupMemoryBarrierWithGroupSync();
        PerGroupSharedData[uGroupIndex] = uResult;
        GroupMemoryBarrierWithGroupSync();
    }

    Data[u3ThreadId.x] = PerGroupSharedData[uGroupIndex];

	/*printf( "Thread index %d Updated Data %d", u3ThreadId.x, Data[u3ThreadId.x] );
	printf( "------------------------------------------------------" );*/
}