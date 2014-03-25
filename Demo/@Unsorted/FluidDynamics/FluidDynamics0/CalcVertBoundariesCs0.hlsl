
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
// g_ProblemFactors.w = scale ( +1 | -1 )
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

RWTexture2D<float> g_TargetState : register ( u0 ); // State to apply boundaries

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
	const int3 LeftBoundaryCell = int3( 0, DispatchThreadId.y + 1, 0 );
	const int3 RightBoundaryCell = int3( g_ProblemFactors.x - 1, DispatchThreadId.y + 1, 0 );
	const float Scale = g_ProblemFactors.w;

	const float2 RightBoundaryCoords = RightBoundaryCell.xy * 4.0f;
	const float2 LeftBoundaryCoords = LeftBoundaryCell.xy * 4.0f;

	float2 NeightbourCellCoords = 0.0f;

	// 8 Reads/Writes Per Thread - Ugly, but still valid.
	// At least, there is no need for additional storage.

	NeightbourCellCoords = ( LeftBoundaryCell.xy + g_RightOffset.xy ) * 4.0f;
	[ unroll(4) ] for ( int ii = 0; ii < 4; ii++ )
		g_TargetState[ LeftBoundaryCoords + float2( ii, 0 ) ] =
		Scale * g_TargetState[ NeightbourCellCoords + float2( ii, 0 ) ];

	NeightbourCellCoords = ( RightBoundaryCell.xy + g_LeftOffset.xy ) * 4.0f;
	[ unroll(4) ] for ( int jj = 0; jj < 4; jj++ )
		g_TargetState[ RightBoundaryCoords + float2( jj, 0 ) ] =
		Scale * g_TargetState[ NeightbourCellCoords + float2( jj, 0 ) ];
}