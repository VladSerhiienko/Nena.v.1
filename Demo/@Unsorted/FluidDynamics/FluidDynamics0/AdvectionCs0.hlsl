
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

RWTexture2D<float4> g_VelocityPingPong1 : register ( u0 ); // New velocity state vector field
Texture2D<float4> g_VelocityPingPong0 : register ( t0 ); // Last velocity state vector field

//SamplerState g_PointClampSampler : register ( s0 ); // Color sampler
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

[ numthreads( 1, 1, 1 ) ] void main( in uniform uint3 DispatchThreadId : SV_DispatchThreadID )
{
	const float2 CentralCell = DispatchThreadId.xy + 1.0f;
	const int3 CellCoords = int3( CentralCell, 0 );

	const float TimeDelta = g_ProblemFactors.z;
	const float2 PreviousPosition = ( CentralCell - TimeDelta * g_VelocityPingPong0.Load( CellCoords ).xy ) / g_ProblemFactors.xy;
	const float4 NewVelocity = g_VelocityPingPong0.SampleLevel( g_LinearSampler, PreviousPosition, 0 );

	g_VelocityPingPong1[ CentralCell.xy ] = NewVelocity;
}