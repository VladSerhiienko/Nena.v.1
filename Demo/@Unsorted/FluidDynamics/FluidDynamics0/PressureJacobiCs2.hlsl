
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
// g_ProblemFactors.x = texture w
// g_ProblemFactors.y = texture h
// g_ProblemFactors.z = dt (time step)
//
// g_PoissonFactors.x = alpha
// g_PoissonFactors.y = reciprocal beta
// g_PoissonFactors.z = dx
//
// g_UnstableVelocityDivergence = del(w)
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

RWTexture2D<float4> g_PressurePingPong1 : register ( u0 );	//
Texture2D<float4> g_PressurePingPong0 : register ( t0 ); //
Texture2D<float4> g_VelocityDivergence : register ( t1 ); //

// --------------------------------------------------------------------------------------
// Shader: Pressure solver iteration
// --------------------------------------------------------------------------------------
// b = (b)oundary node
// i = (i)nterior node
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
// Represents a single iteration for the solver of the Poisson equation.
// (1) Pressure : 
//		alpha = -(dx pow 2) 
//		beta = 1/4
// (2) Viscous diffusion : 
//		alpha = (dx pow 2)/(visc dt) 
//		beta = 1/(4 + alpha)
// --------------------------------------------------------------------------------------

[ numthreads( 1, 1, 1 ) ] void main( in const uint3 DispatchThreadId : SV_DispatchThreadID )
{
	const float2 CentralCell = DispatchThreadId.xy + 1.0f; //
	const int3 CellCoords = int3( CentralCell, 0 ); //

	const int3 rnc = CellCoords + g_RightOffset; //
	const int3 lnc = CellCoords + g_LeftOffset; //
	const int3 unc = CellCoords + g_UpOffset; //
	const int3 dnc = CellCoords + g_DownOffset; //

	const float pip1jk = g_PressurePingPong0.Load( rnc ).x; //
	const float pis1jk = g_PressurePingPong0.Load( lnc ).x; //
	const float pijp1k = g_PressurePingPong0.Load( unc ).x; //
	const float pijs1k = g_PressurePingPong0.Load( dnc ).x; //

	const float bij = g_VelocityDivergence.Load( CellCoords ).x;		//printf( "divergence %f", bij.x ); //
	const float alpha = g_PoissonFactors.x;								//printf( "alpha %f", alpha );  //
	const float beta = g_PoissonFactors.y;								//printf( "1/beta %f", beta );  //

	//const float Pressure = ( pip1jk + pis1jk + pijp1k + pijs1k - bij ) * ( 1.0f / 4.0f );
	const float Pressure = ( pip1jk + pis1jk + pijp1k + pijs1k + bij * alpha ) * beta;
	const float4 NewPressure = float4( Pressure, Pressure, Pressure, 0.0f ); //

	g_PressurePingPong1[ CentralCell ] = NewPressure; //
	if ( dnc.y == 0 ) { g_PressurePingPong1[ dnc.xy ] = NewPressure; } // printf( "UpperBoundaryUpdated" );
	if ( lnc.x == 0 ) { g_PressurePingPong1[ lnc.xy ] = NewPressure; } // printf( "LeftBoundaryUpdated" ); }
	if ( unc.y == g_ProblemFactors.y - 1 ) { g_PressurePingPong1[ unc.xy ] = NewPressure; } // printf( "LowerBoundaryUpdated" ); }
	if ( rnc.x == g_ProblemFactors.x - 1 ) { g_PressurePingPong1[ rnc.xy ] = NewPressure; } // printf( "RightBoundaryUpdated" ); }

	/*if ( dot( pijkp1, pijkp1 ) != 0.0f )
	{
	printf( "PRS => pressure %f to cell %d %d", NewPressure.x, CellCoords.x, CellCoords.y );
	}*/
}