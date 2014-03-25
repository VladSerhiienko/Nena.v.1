#define DepthMaxX 512
#define DepthMaxY 512
#define DepthMaxXDiv2 256
#define DepthMaxYDiv2 256
#define Radius 256.0f
#define RadiusSq (256.0f * 256.0f)

RWTexture2D<float4> g_DepthMap : register(u0);
static const float4 g_Near = float4(0.0f, 0.0f, 0.0f, 0.0f);
static const float4 g_Far = float4(1.5f, 1.5f, 1.5f, 1.5f);

[numthreads(256, 2, 1)]
void main( uint3 threadId : SV_DispatchThreadID )
{
	const float2 xy = float2(
		((float)threadId.x - (float)DepthMaxXDiv2), 
		((float)DepthMaxYDiv2 - (float)threadId.y)
		);

	const float lsq = dot(xy,xy);

	//printf( "Thread Id x %d y %d", threadId.x, threadId.y );
	//printf( "Coords x %f y %f", xy.x, xy.y );
	//printf( "RSq %f", RadiusSq );
	//printf( "LSq %f", lsq );

	//if (l > Radius) g_DepthMap[xy.xy] = g_Near;
	if (lsq > RadiusSq) 
	//{
		//printf( "Writing" );
		g_DepthMap[threadId.xy] = g_Near;
	//}
	//if (l > Radius) g_DepthMap[xy.xy] = g_Far;
	//if (l > Radius) g_DepthMap[threadId.xy] = g_Far;
}