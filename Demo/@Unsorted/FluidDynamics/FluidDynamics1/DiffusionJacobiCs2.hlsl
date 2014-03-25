
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

RWTexture2D<float4> g_VelocityPingPong1 : register ( u0 );	//
Texture2D<float4> g_VelocityPingPong0 : register ( t0 ); //

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
	const float2 CentralCell = DispatchThreadId.xy + 1;
	const int3 CellCoords = int3( CentralCell, 0 );						// printf( "central	= %d %d", CellCoords.x, CellCoords.y ); //

	const int3 rnc = CellCoords + g_RightOffset;						//
	const int3 lnc = CellCoords + g_LeftOffset;							//
	const int3 unc = CellCoords + g_UpOffset;							//
	const int3 dnc = CellCoords + g_DownOffset;							//

	const float2 pip1jk = g_VelocityPingPong0.Load( rnc ).xy;			// printf( "pip1jk = %f %f", pip1jk.x, pip1jk.y ); //
	const float2 pis1jk = g_VelocityPingPong0.Load( lnc ).xy;			// printf( "pis1jk = %f %f", pis1jk.x, pis1jk.y ); //
	const float2 pijp1k = g_VelocityPingPong0.Load( unc ).xy;			// printf( "pijp1k = %f %f", pijp1k.x, pijp1k.y ); //
	const float2 pijs1k = g_VelocityPingPong0.Load( dnc ).xy;			// printf( "pijs1k = %f %f", pijs1k.x, pijs1k.y ); //

	const float2 bij = g_VelocityPingPong0.Load( CellCoords ).xy;		// printf( "bij = %f %f", bij.x, bij.y ); //
	const float alpha = g_PoissonFactors.x;								// printf( "alpha = %f", alpha ); //
	const float beta = g_PoissonFactors.y;								// printf( "1/beta = %f", beta ); //

	//const float4 NewVelocity = float4( ( pip1jk + pis1jk + pijp1k + pijs1k + bij * alpha / g_ProblemFactors.xy ) * beta / g_ProblemFactors.xy, 0.0f, 0.0f ); // printf( "new velocity = %f %f", pijkp1.x, pijkp1.y );//
	const float4 NewVelocity = float4( ( pip1jk + pis1jk + pijp1k + pijs1k + bij * alpha ) * beta, 0.0f, 0.0f ); // printf( "new velocity = %f %f", pijkp1.x, pijkp1.y );//

	g_VelocityPingPong1[ CentralCell ] = NewVelocity; //
	if ( dnc.y == 0 ) { g_VelocityPingPong1[ dnc.xy ] = -NewVelocity; }									// printf( "UpperBoundaryUpdated" );
	if ( lnc.x == 0 ) { g_VelocityPingPong1[ lnc.xy ] = -NewVelocity; }									// printf( "LeftBoundaryUpdated" ); } //
	if ( unc.y == g_ProblemFactors.y - 1 ) { g_VelocityPingPong1[ unc.xy ] = -NewVelocity; }			// printf( "LowerBoundaryUpdated" ); } // 
	if ( rnc.x == g_ProblemFactors.x - 1 ) { g_VelocityPingPong1[ rnc.xy ] = -NewVelocity; }			// printf( "RightBoundaryUpdated" ); } // 

	/*if ( dot( pijkp1, pijkp1 ) != 0.0f )
	{
	printf( "DIFF => velocity %f %f to cell %d %d", NewVelocity.x, NewVelocity.y, CellCoords.x, CellCoords.y );
	}*/
}