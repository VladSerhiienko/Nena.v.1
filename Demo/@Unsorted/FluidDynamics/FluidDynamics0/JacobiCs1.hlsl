
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

cbuffer Offsets	: register ( cb0 )
{
	static const int3 g_LeftOffset = int3( -1, 0, 0 );
	static const int3 g_RightOffset = int3( 1, 0, 0 );
	static const int3 g_UpOffset = int3( 0, -1, 0 );
	static const int3 g_DownOffset = int3( 0, 1, 0 );
};

cbuffer ProblemParameters : register ( cb1 )
{
	float4 g_ProblemFactors;
	float4 g_PoissonFactors;
};

RWTexture2D<float4> g_XVectorPingPong1 : register ( u0 );	//
Texture2D<float4> g_XVectorPingPong0 : register ( t0 ); //
Texture2D<float4> g_BVector : register ( t1 ); //

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

[ numthreads( 1, 1, 1 ) ] void main( in uniform uint3 DispatchThreadId : SV_DispatchThreadID )
{
	const float2 CentralCell = DispatchThreadId.xy + 1.0f; //
	const int3 CellCoords = int3( CentralCell, 0 ); //
	const float Scale = g_ProblemFactors.w;

	const int3 rnc = CellCoords + g_RightOffset; //
	const int3 lnc = CellCoords + g_LeftOffset; //
	const int3 unc = CellCoords + g_UpOffset; //
	const int3 dnc = CellCoords + g_DownOffset; //

	const float4 pip1jk = g_XVectorPingPong0.Load( rnc ); //
	const float4 pis1jk = g_XVectorPingPong0.Load( lnc ); //
	const float4 pijp1k = g_XVectorPingPong0.Load( unc ); //
	const float4 pijs1k = g_XVectorPingPong0.Load( dnc ); //

	const float4 bij = g_BVector.Load( CellCoords ); //
	const float alpha = g_PoissonFactors.x; //
	const float beta = g_PoissonFactors.y; //

	const float4 pijkp1 = ( pip1jk + pis1jk + pijp1k + pijs1k + bij * alpha ) * beta; //

	g_XVectorPingPong1[ CentralCell ] = pijkp1; //

	if ( unc.y == 0 ) g_XVectorPingPong1[ unc.xy ] = Scale * pijkp1;
	if ( lnc.x == 0 ) g_XVectorPingPong1[ lnc.xy ] = Scale * pijkp1;
	if ( dnc.x == g_ProblemFactors.y - 1 ) g_XVectorPingPong1[ dnc.xy ] = Scale * pijkp1;
	if ( rnc.x == g_ProblemFactors.x - 1 ) g_XVectorPingPong1[ rnc.xy ] = Scale * pijkp1;
}