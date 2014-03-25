
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
// total threads = [510]x[254] < 65k (good)
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

cbuffer Offsets	: register ( cb0 )
{
	static const int3 g_LeftOffset = int3( -1, 0, 0 );
	static const int3 g_RightOffset = int3( 1, 0, 0 );
	static const int3 g_UpOffset = int3( 0, 1, 0 );
	static const int3 g_DownOffset = int3( 0, -1, 0 );
};

cbuffer ProblemParameters : register ( cb1 )
{
	float4 g_ProblemFactors;
	float4 g_PoissonFactors;
};

RWTexture2D<float4> g_Divergence : register ( u0 ); // Unstable velocity divergence
Texture2D<float4> g_Velocity : register ( t0 ); // Last velocity state vector field

// --------------------------------------------------------------------------------------
// Shader: Fluid Velocity Divergence
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
// (2) Neighbours: left, right, upper, lower. General scheme:
// 
//					upper(j-1)
//					  ^  
//			left < central > right
//				      v  
//					lower(j+1)
// 
// (3) div(v) = dv/dx + dv/dy (using central finite differences)
//     div(v) = (v(i+1,j)-v(i-1,j))/2 + (v(i,j+1)-v(i,j-1))/2 = 
//            = (r.x-l.x)0.5 + (d.y-u.y)0.5
// --------------------------------------------------------------------------------------

[ numthreads( 1, 1, 1 ) ] void main( in const uint3 DispatchThreadId : SV_DispatchThreadID )
{
	const float2 CentralCell = DispatchThreadId.xy + 1.0f;
	const int3 CellCoords = int3( CentralCell, 0 ); //

	const int3 rnc = CellCoords + g_RightOffset; //
	const int3 lnc = CellCoords + g_LeftOffset; //
	const int3 unc = CellCoords + g_UpOffset; //
	const int3 dnc = CellCoords + g_DownOffset; //

	const float2 pip1jk = g_Velocity.Load( rnc ).xy; //
	const float2 pis1jk = g_Velocity.Load( lnc ).xy; //
	const float2 pijp1k = g_Velocity.Load( unc ).xy; //
	const float2 pijs1k = g_Velocity.Load( dnc ).xy; //

	const float rdx2 = 1.0f / (g_PoissonFactors.z * g_PoissonFactors.z);
	const float Divergence = ( ( pip1jk.x - pis1jk.x ) + ( pijp1k.y - pijs1k.y ) ) * 0.5f * rdx2;
	g_Divergence[ CentralCell ] = float4( Divergence, Divergence, Divergence, 0.0f );
	//g_Divergence[ CentralCell ] = float4( Divergence, Divergence, 0.0f, 0.0f );

	/*if ( Divergence != 0.0f )
	{
		printf( "DIV => divergence %f to cell %d %d", Divergence, CellCoords.x, CellCoords.y );
	}*/
}