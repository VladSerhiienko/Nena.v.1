#define TRANSPOSE_BLOCK_SIZE	(16)
#define TransposeBlockSize		(16)
#define TransposeBlockSizeSqr	(16*16)

cbuffer PerFrame				: register (cb0)
{
    uint g_iLevel;
    uint g_iLevelMask;
    uint g_iWidth;
    uint g_iHeight;
};

StructuredBuffer<uint> Input	: register( t0 );
RWStructuredBuffer<uint> Data	: register( u0 );

groupshared uint PerGroupSharedData[TransposeBlockSizeSqr];

[numthreads(TransposeBlockSize, TransposeBlockSize, 1)]
void main( 
	uint3 u3GroupId				: SV_GroupID, 
    uint3 u3ThreadId			: SV_DispatchThreadID, 
    uint3 u3GroupThreadId		: SV_GroupThreadID, 
    uint uGroupIndex			: SV_GroupIndex 
	)
{
	/*printf( "------------------------------------------------------" );
	printf( "Transpose call" );
	printf( "Groud index %d Level %d W %d H %d", uGroupIndex, g_iLevel, g_iWidth, g_iHeight );
	printf( "Thread index %d Init Data %d", u3ThreadId.x, Data[u3ThreadId.x] );*/

	PerGroupSharedData[uGroupIndex] = Input[u3ThreadId.y * g_iWidth + u3ThreadId.x];
    GroupMemoryBarrierWithGroupSync();

    uint2 Coords = u3ThreadId.yx - u3GroupThreadId.yx + u3GroupThreadId.xy;
    Data[Coords.y * g_iHeight + Coords.x] = PerGroupSharedData[u3GroupThreadId.x * TransposeBlockSize + u3GroupThreadId.y];

	/*printf( "Thread index %d Updated Data %d", u3ThreadId.x, Data[u3ThreadId.x] );
	printf( "------------------------------------------------------" );*/
}