RWTexture2D<float4> g_TargetTexturePingPong1 : register ( u0 );
Texture2D<float4> g_TargetTexturePingPong0 : register ( t0 );

[ numthreads( 1, 1, 1 ) ] void main( in const uint3 threadId : SV_DispatchThreadID )
{
	const float2 cellId = threadId.xy; 
	const int3 cellCoords = int3( threadId.xy, 0 );
	g_TargetTexturePingPong1[ cellId ] = g_TargetTexturePingPong0.Load(cellCoords);
}