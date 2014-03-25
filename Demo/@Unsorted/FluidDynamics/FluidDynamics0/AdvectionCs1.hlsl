
// --------------------------------------------------------------------------------------
// Dispatch call: (x = width - 2, y = height - 2, 1)
// --------------------------------------------------------------------------------------
// dispatchThreadId.x [0 .. width - 3] => [1 .. texture width - 2]
// dispatchThreadId.y [0 .. height - 3] => [1 .. texture height - 2]
// 
// Example: w = 512 h = 256
// Dispatch(510,254,1)
// dispatchThreadId.x [ 0 .. 509 ] 
// dispatchThreadId.y [ 0 .. 253 ] 
// thus, total threads = [510]x[254] < 65k (good)
//
// --------------------------------------------------------------------------------------
// Usage: 
// --------------------------------------------------------------------------------------
// g_ProblemFactors.x = texture width
// g_ProblemFactors.y = texture height
// g_ProblemFactors.z = time step
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Globals: 
// --------------------------------------------------------------------------------------

cbuffer ProblemParameters : register ( cb0 )
{
	float4 g_ProblemFactors;
	float4 g_PoissonFactors;
};

static const int3 g_LeftOffset = int3( -1, 0, 0 );
static const int3 g_RightOffset = int3( 1, 0, 0 );
static const int3 g_UpOffset = int3( 0, 1, 0 );
static const int3 g_DownOffset = int3( 0, -1, 0 );

RWTexture2D<float4> g_VelocityPingPong1 : register ( u0 ); // New velocity state vector field
Texture2D<float4> g_VelocityPingPong0 : register ( t0 ); // Last velocity state vector field
SamplerState g_LinearSampler : register ( s0 ); // Color sampler

// --------------------------------------------------------------------------------------
// Shader: Fluid Advection 
// --------------------------------------------------------------------------------------
// b = boundary node
// i = interior node
//
// Velocity vector field scheme (texture space):
//			o ----> x
//			| bbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//			| biiiiiiiiiiiiiiiiiiiiiiiiiiib
//		  y v biiiiiiiiiiiiiiiiiiiiiiiiiiib
// 			  biiiiiiiiiiiiiiiiiiiiiiiiiiib
// 			  biiiiiiiiiiiiiiiiiiiiiiiiiiib
// 			  biiiiiiiiiiiiiiiiiiiiiiiiiiib
// 			  bbbbbbbbbbbbbbbbbbbbbbbbbbbbb
// 
//			x [0 .. width - 3]	
//			y [0 .. height - 3]	
//
// This shader program executes for each interior node. 
// (1) Central cell = Dispatch thread index + (1, 1)
// (2) Travel back in time through the fluid velocity vector field and get the previous cell position 
// (3) Calculate the new velocity values based on bilerp value on cell previous position (see 2 step).
// (3.1) Previous position to texture space.
// (3.2) Sample with linear or point texture sampler.
//
// --------------------------------------------------------------------------------------

[ numthreads( 1, 1, 1 ) ] void main( in const uint3 DispatchThreadId : SV_DispatchThreadID )
{

	//printf( "buffer : problem x = %f ", g_ProblemFactors.x );
	//printf( "buffer : problem y = %f ", g_ProblemFactors.y );
	//printf( "buffer : problem z = %f ", g_ProblemFactors.z );
	//printf( "buffer : problem w = %f ", g_ProblemFactors.w );
	//printf( "buffer : poisson x = %f ", g_PoissonFactors.x );
	//printf( "buffer : poisson y = %f ", g_PoissonFactors.y );
	//printf( "buffer : poisson z = %f ", g_PoissonFactors.z );
	//printf( "buffer : poisson w = %f ", g_PoissonFactors.w );

	//const float2 TimeDelta = g_ProblemFactors.z; // dt
	const float2 TimeDelta = g_ProblemFactors.z * g_ProblemFactors.xy; // dt
	const float2 CentralCell = DispatchThreadId.xy + 1.0f; // i j
	const int3 CellCoords = int3( CentralCell, 0 ); // i j

	//const float2 ClampMin = float2( 0.5f, 0.5f );
	//const float2 ClampMax = float2( g_ProblemFactors.x - 0.5f, g_ProblemFactors.y - 0.5f );
	const float2 Velocity = g_VelocityPingPong0.Load( CellCoords ).xy; // du dv [ij]

	float2 PreviousPosition = CentralCell - TimeDelta * Velocity; // x y
	//float2 PreviousPositionRanged0 = PreviousPosition /( g_ProblemFactors.xy);
	//float2 PreviousPositionRanged1 = PreviousPosition /( g_ProblemFactors.xy - 2.0f);
	//PreviousPosition = clamp( PreviousPosition, ClampMin, ClampMax );

	float4 AdvectedVelocity = 0.0f;
	{
		const int i0 = (int) PreviousPosition.x;
		const int i1 = i0 + 1;
		const int j0 = (int) PreviousPosition.y;
		const int j1 = j0 + 1;

		const float s1 = PreviousPosition.x - i0;
		const float s0 = 1.0f - s1;
		const float t1 = PreviousPosition.y - j0;
		const float t0 = 1.0f - t1;

		const float2 Qty00 = g_VelocityPingPong0.Load( int3( i0, j0, 0 ) ).xy;
		const float2 Qty01 = g_VelocityPingPong0.Load( int3( i0, j1, 0 ) ).xy;
		const float2 Qty10 = g_VelocityPingPong0.Load( int3( i1, j0, 0 ) ).xy;
		const float2 Qty11 = g_VelocityPingPong0.Load( int3( i1, j1, 0 ) ).xy;

		AdvectedVelocity.xy = s0 * ( t0 * Qty00 + t1 * Qty01 ) + s1 * ( t0 * Qty10 + t1 * Qty11 );
	}

	//const float4 NewVelocity0 = g_VelocityPingPong0.SampleLevel( g_LinearSampler, PreviousPositionRanged0, 0 );
	//const float4 NewVelocity1 = g_VelocityPingPong0.SampleLevel( g_LinearSampler, PreviousPositionRanged1, 0 );
	//const float4 NewVelocity = lerp( NewVelocity0, NewVelocity1, 0.5f );
	//const float4 NewVelocity = NewVelocity0;
	//AdvectedVelocity = lerp( AdvectedVelocity, NewVelocity, 0.5f );

	g_VelocityPingPong1[ CentralCell.xy ] = AdvectedVelocity;
	//g_VelocityPingPong1[ CentralCell.xy ] = NewVelocity;

	const int3 unc = CellCoords + g_UpOffset;
	const int3 dnc = CellCoords + g_DownOffset;
	const int3 lnc = CellCoords + g_LeftOffset;
	const int3 rnc = CellCoords + g_RightOffset;
	if ( dnc.y == 0 ) { g_VelocityPingPong1[ dnc.xy ] = -AdvectedVelocity; } // printf( "UpperBoundaryUpdated" ); }
	if ( lnc.x == 0 ) { g_VelocityPingPong1[ lnc.xy ] = -AdvectedVelocity; } // printf( "LeftBoundaryUpdated" ); }
	if ( unc.y == g_ProblemFactors.y - 1 ) { g_VelocityPingPong1[ unc.xy ] = -AdvectedVelocity; } // printf( "LowerBoundaryUpdated" ); } 
	if ( rnc.x == g_ProblemFactors.x - 1 ) { g_VelocityPingPong1[ rnc.xy ] = -AdvectedVelocity; } // printf( "RightBoundaryUpdated" ); } 	

	/*if ( dot( NewVelocity, NewVelocity ) != 0.0f )
	{
	printf( "ADV => velocity %f %f to cell %d %d", NewVelocity.x, NewVelocity.y, CellCoords.x, CellCoords.y );
	}*/
}