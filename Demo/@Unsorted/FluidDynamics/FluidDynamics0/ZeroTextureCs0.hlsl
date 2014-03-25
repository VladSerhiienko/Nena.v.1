RWTexture2D<float4> g_TargetTexture : register ( u0 );

[ numthreads( 1, 1, 1 ) ] void main( in const uint3 threadId : SV_DispatchThreadID )
{
	// printf( "Zeroing texture at %d-%d", threadId.x, threadId.y );

	const float2 cellId = threadId.xy;
	g_TargetTexture[ cellId ] = float4( 0.0f, 0.0f, 0.0f, 0.0f );
}