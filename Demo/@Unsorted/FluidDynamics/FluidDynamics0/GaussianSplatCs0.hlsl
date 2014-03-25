
// --------------------------------------------------------------------------------------
// Globals: 
// --------------------------------------------------------------------------------------

cbuffer ProblemParameters : register ( cb0 )
{
	float4 g_ProblemFactors;
	float4 g_PoissonFactors;
};

cbuffer ForcePerFrame : register ( cb1 )
{
	// x - force proj x
	// y - force proj y
	float4 g_Force;

	// x - force impulse pos x
	// y - force impulse pos y
	// z - desired radius
	float4 g_Splat;

	// Precomputed:
	// x - x offset = g_Splat.x - g_Splat.z
	// y - y offset = g_Splat.y - g_Splat.z
	float4 g_Offset;
}

static const int3 g_LeftOffset = int3( -1, 0, 0 );
static const int3 g_RightOffset = int3( 1, 0, 0 );
static const int3 g_UpOffset = int3( 0, 1, 0 );
static const int3 g_DownOffset = int3( 0, -1, 0 );

RWTexture2D<float4> g_VelocityPingPong1 : register ( u0 ); // New velocity state vector field
Texture2D<float4> g_VelocityPingPong0 : register ( t0 ); // Last velocity state vector field

// --------------------------------------------------------------------------------------
// Shader:
// --------------------------------------------------------------------------------------
// 
//
//
//
//
// --------------------------------------------------------------------------------------
[ numthreads( 1, 1, 1 ) ] void main( in const uint3 DispatchThreadId : SV_DispatchThreadID )
{
	const float2 offset = g_Offset.xy;
	const float dt = g_ProblemFactors.z;
	const float2 cellIndex = (DispatchThreadId.xy) + offset;

	const float r = g_Splat.z;
	const float r2 = r * r;

	const float2 d = cellIndex - g_Splat.xy;
	const float d2 = dot( d, d );

	if ( d2 < r2 )
	{
		const float2 ForceInfluence = g_Force.xy * dt * exp( d2 / r2 );
		//const float2 ForceInfluence = ( -d * 800.0f ) * dt * exp( d2 / r2 );

		const int3 cnc = int3( cellIndex, 0 );
		const float2 Velocity = g_VelocityPingPong0.Load( cnc ).xy;

		const float4 NewVelocity = float4( Velocity + ForceInfluence, 0.0f, 0.0f );
		g_VelocityPingPong1[ cnc.xy ] = NewVelocity;

		const int3 unc = cnc + g_UpOffset;
		const int3 dnc = cnc + g_DownOffset;
		const int3 lnc = cnc + g_LeftOffset;
		const int3 rnc = cnc + g_RightOffset;
		if ( dnc.y == 0 ) { g_VelocityPingPong1[ dnc.xy ] = -NewVelocity; } // printf( "UpperBoundaryUpdated" ); } //
		if ( lnc.x == 0 ) { g_VelocityPingPong1[ lnc.xy ] = -NewVelocity; } // printf( "LeftBoundaryUpdated" ); } //
		if ( unc.y == g_ProblemFactors.y - 1 ) { g_VelocityPingPong1[ unc.xy ] = -NewVelocity; } // printf( "LowerBoundaryUpdated" ); } // 
		if ( rnc.x == g_ProblemFactors.x - 1 ) { g_VelocityPingPong1[ rnc.xy ] = -NewVelocity; } // printf( "RightBoundaryUpdated" ); } // 
	}
}