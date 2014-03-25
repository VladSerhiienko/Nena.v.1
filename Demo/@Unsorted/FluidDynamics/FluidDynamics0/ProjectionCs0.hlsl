
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
};

cbuffer PoissonParameters : register ( cb2 )
{
	float4 g_PoissonFactors;
};

RWTexture2D<float4> g_VelocityPingPong1	: register ( u0 ); // New velocity state vector field
Texture2D<float4> g_VelocityPingPong0 : register ( t0 ); // Last velocity state vector field
Texture2D<float4> g_Pressure : register ( t1 ); // Last pressure state field

// --------------------------------------------------------------------------------------
// Shader: Projection operator implementation
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
// (1) Calculate the pressure gradient using finite differences.
// (2) Sample divergent velocity values for the current cell.
// (3) Subtract sampled velocity (w) value from the pressure gradient (grad p)
//	   to obtaine divergent free velocity value.
// --------------------------------------------------------------------------------------

[ numthreads( 1, 1, 1 ) ] void main( in uniform uint3 DispatchThreadId : SV_DispatchThreadID )
{
	const float2 CentralCell = DispatchThreadId.xy + 1.0f;	//
	const int3 CellCoords = int3( CentralCell, 0 );			//

	const int3 rnc = CellCoords + g_RightOffset;	//
	const int3 lnc = CellCoords + g_LeftOffset;		//
	const int3 unc = CellCoords + g_UpOffset;		//
	const int3 dnc = CellCoords + g_DownOffset;		//

	const float4 pip1jk = g_Pressure.Load( rnc );		//
	const float4 pis1jk = g_Pressure.Load( lnc );		//
	const float4 pijp1k = g_Pressure.Load( unc );		//
	const float4 pijs1k = g_Pressure.Load( dnc );		//

	const float dx = g_PoissonFactors.z; // 
	const float2 PressureGradient = float2( pip1jk.x - pis1jk.x, pijp1k.y - pijs1k.y ) / dx; // 

	const float4 UnstableVelocity = g_VelocityPingPong0.Load( CellCoords ); //
	const float4 DivergenceFreeVelocity = float4( UnstableVelocity.xy - PressureGradient, 0.0f, 0.0f ); //

	g_VelocityPingPong1[ CentralCell ] = DivergenceFreeVelocity; //
}