
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

RWTexture2D<float4> g_TargetStatePingPong1 : register ( u0 ); // State to apply boundaries
Texture2D<float4> g_TargetStatePingPong0 : register ( t0 ); // State to apply boundaries

// --------------------------------------------------------------------------------------
// Shader: Horizontal dirichlet boundary conditions for velocity vector field
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
// For example for left boundaries: (u(0j) + u(1j)) / 2 = 0 => u(0j) = -u(1j)
//
// --------------------------------------------------------------------------------------

[ numthreads( 1, 1, 1 ) ] void main( in uniform uint3 DispatchThreadId : SV_DispatchThreadID )
{
	const int3 UpperBoundaryCell = int3( DispatchThreadId.x + 1, 0, 0 );
	const int3 LowerBoundaryCell = int3( DispatchThreadId.x + 1, g_ProblemFactors.y - 1, 0 );

	const float2 UpperBoundaryCoords = UpperBoundaryCell.xy;
	const float2 LowerBoundaryCoords = LowerBoundaryCell.xy;

	g_TargetStatePingPong1[ UpperBoundaryCoords ] = g_TargetStatePingPong0.Load( UpperBoundaryCell );
	g_TargetStatePingPong1[ LowerBoundaryCoords ] = g_TargetStatePingPong0.Load( LowerBoundaryCell );
}