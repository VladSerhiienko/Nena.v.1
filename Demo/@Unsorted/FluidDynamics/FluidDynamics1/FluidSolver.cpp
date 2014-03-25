#include "Precompiled.h"
#include "FluidSimulator.h"

void Sandbox::FluidSimulator::ConfigureSimulationPipeline( )
{
	m_d3dContext->CSSetSamplers( 0, 1, m_linearSampler.GetAddressOf( ) );
	m_d3dContext->CSSetConstantBuffers( 0, 2, m_cbuffers );
}

/*
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
Next Step : Calculate divergent velocity
u1 = advection(u0)
-------------------------------------------------------------------------------------
bind advection cs
bind m_velocityPingPong0 to srv 0
bind m_velocityPingPong1 to uav 0
dispatch [ w-2, h-2, 1 ]
swap v
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
*/
void Sandbox::FluidSimulator::ComputeAdvection( )
{
	m_d3dContext->CSSetShader( m_fluidAdvectionCs.Get( ), nullptr, 0u );

	m_d3dContext->UpdateSubresource( m_problemParams.Get( ), 0u, nullptr, &m_problemParamsData, 0u, 0u );
	m_d3dContext->CSSetUnorderedAccessViews( 0u, 1u, m_velocityPingPong1Uav.GetAddressOf( ), m_zeroUavInitCounts );
	m_d3dContext->CSSetShaderResources( 0u, 1u, m_velocityPingPong0Srv.GetAddressOf( ) );
	m_d3dContext->Dispatch( _To_uint ProblemWidth( ) - 2, _To_uint ProblemHeight( ) - 2, 1 );

	UnbindResourcesFromPipeline( );
	SwapVelocities( );
}

/*
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
Next Step : Calculate divergent velocity againts viscous diffusion
-------------------------------------------------------------------------------------
bind jacobi cs
update poisson equation solver parameters for Poisson-Diffusion
-------------------------------------------------------------------------------------
while convergence is not reached (or just 21-51 (or other even number) iterations) do
-- bind m_velocityPingPong1 to uav 0
-- bind m_velocityPingPong0 to srv 0
-- bind m_velocityPingPong0 to srv 1
-- dispatch [ w-2, h-2, 1 ]
-- swap v
end
-------------------------------------------------------------------------------------
u0 is up-to-date velocity resource now
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
*/
void Sandbox::FluidSimulator::ComputeViscousDiffusion( )
{
	PoissonSolverAlpha( PoissonSolverGridStep( ) * PoissonSolverGridStep( ) / ( ProblemTimeStep( ) * Viscosity( ) ) ); // *ProblemWidth( ) * ProblemHeight( ) ) );
	//PoissonSolverAlpha( PoissonSolverGridStep( ) * PoissonSolverGridStep( ) / ProblemTimeStep( ) / Viscosity( ) );
	PoissonSolverReciprocalBeta( 1.0f / ( 4.0f + PoissonSolverAlpha( ) ) );
	ProblemBoundaryScale( -1.0f );

	m_d3dContext->CSSetShader( m_diffusionJacobiCs.Get( ), nullptr, 0u );

	m_d3dContext->UpdateSubresource( m_problemParams.Get( ), 0u, nullptr, &m_problemParamsData, 0u, 0u );
	m_d3dContext->CSSetUnorderedAccessViews( 0u, 1u, m_velocityPingPong1Uav.GetAddressOf( ), m_zeroUavInitCounts );
	m_d3dContext->CSSetShaderResources( 0u, 1u, m_velocityPingPong0Srv.GetAddressOf( ) );

	for ( auto i = 0; i < 15; i++ )
	{
		m_d3dContext->Dispatch( _To_uint ProblemWidth( ) - 2, _To_uint ProblemHeight( ) - 2, 1 );
		SwapVelocities( );
	}

	UnbindResourcesFromPipeline( );
}

/*
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
Next Step : Calculate pressure based on unstable velocity
-------------------------------------------------------------------------------------
bind jacobi cs
update poisson equation solver parameters for Poisson-Pressure
-------------------------------------------------------------------------------------
while convergence is not reached (or just 21-51 (or other even number) iterations) do
-- bind m_pressurePingPong1 to uav 0
-- bind m_pressurePingPong0 to srv 0
-- bind divergence to t1
-- dispatch [ w-2, h-2, 1 ]
-- swap p
end
-------------------------------------------------------------------------------------
p0 is up-to-date pressure resource now
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
*/
void Sandbox::FluidSimulator::ComputePressure( )
{
	PoissonSolverAlpha( -1.0f * PoissonSolverGridStep( ) * PoissonSolverGridStep( ) );
	PoissonSolverReciprocalBeta( 1.0f / 4.0f );
	ProblemBoundaryScale( 1.0f );

	m_d3dContext->CSSetShader( m_pressureJacobiCs.Get( ), nullptr, 0u );

	m_d3dContext->UpdateSubresource( m_problemParams.Get( ), 0u, nullptr, &m_problemParamsData, 0u, 0u );
	m_d3dContext->CSSetUnorderedAccessViews( 0u, 1u, m_pressurePingPong1Uav.GetAddressOf( ), m_zeroUavInitCounts );
	m_d3dContext->CSSetShaderResources( 0u, 1u, m_pressurePingPong0Srv.GetAddressOf( ) );
	m_d3dContext->CSSetShaderResources( 1u, 1u, m_divergenceSrv.GetAddressOf( ) );

	for ( auto i = 0; i < 15; i++ )
	{
		m_d3dContext->Dispatch( _To_uint ProblemWidth( ) - 2, _To_uint ProblemHeight( ) - 2, 1 );
		SwapPressures( );
	}

	UnbindResourcesFromPipeline( );
}

#define _Sanbox_fluid_solver_option_External_forces_impulse_position_is_in_the_center 0
#define _Sanbox_fluid_solver_option_External_forces_impulse_position_is_random 1

#define _Sanbox_fluid_solver_option_External_forces_magnitude_is_random_variable 0
#define _Sanbox_fluid_solver_option_External_forces_magnitude_is_random_static 1
#define _Sanbox_fluid_solver_option_External_forces_magnitude_is_static 0

#define _Sanbox_fluid_solver_option_External_forces_impulse_position_moves_along_the_circle 1

void Sandbox::FluidSimulator::ComputeExternalForces( )
{
	static uint32 s_iteration = 0u;
	static float32 s_rotationAngle = Reign::XM_PIDIV2; // radians
	static float32 s_circleRadius = 10.0f; // num cells
	static float32 s_positionX = 0.0f; // num cells
	static float32 s_positionY = 0.0f; // num cells

	if ( s_rotationAngle > Reign::XM_2PI ) s_rotationAngle = 0.0f;
	s_positionX = s_circleRadius * cosf( s_rotationAngle );
	s_positionY = s_circleRadius * sinf( s_rotationAngle );
	s_rotationAngle += 0.15f;
	++s_iteration;

#if _Sanbox_fluid_solver_option_External_forces_impulse_position_moves_along_the_circle

	SplatYImpulse( ProblemHeight( ) * 0.5f + s_positionY * _To_single rand( ) / _To_single RAND_MAX );
	SplatXImpulse( ProblemWidth( ) * 0.5f + s_positionX * _To_single rand( ) / _To_single RAND_MAX );
	SplatRadius( 4.0f );

	SplatXOffset( SplatXImpulse( ) - SplatRadius( ) );
	SplatYOffset( SplatYImpulse( ) - SplatRadius( ) );
	m_forcePerFrameData.Force.x = ( -s_positionX * 100.0f );
	m_forcePerFrameData.Force.y = ( -s_positionY * 100.0f );

	m_d3dContext->UpdateSubresource( m_forcePerFrame.Get( ), 0u, nullptr, &m_forcePerFrameData, 0u, 0u );
	m_d3dContext->CSSetUnorderedAccessViews( 0u, 1u, m_velocityPingPong1Uav.GetAddressOf( ), m_zeroUavInitCounts );
	m_d3dContext->CSSetShaderResources( 0u, 1u, m_velocityPingPong0Srv.GetAddressOf( ) );

	m_d3dContext->CSSetShader( m_copyCs.Get( ), nullptr, 0u );
	m_d3dContext->Dispatch( _To_uint ProblemWidth( ), _To_uint ProblemHeight( ), 1 );

	m_d3dContext->CSSetShader( m_gaussianSplatCs.Get( ), nullptr, 0u );
	m_d3dContext->Dispatch( _To_uint SplatRadius( ) * 2 + 1, _To_uint SplatRadius( ) * 2 + 1, 1 );

	UnbindResourcesFromPipeline( );
	SwapVelocities( );

#else

	if ( s_iteration < 200u )
	{
#if _Sanbox_fluid_solver_option_External_forces_impulse_position_is_in_the_center
		SplatYImpulse( ProblemHeight( ) * 0.5f );
		SplatXImpulse( ProblemWidth( ) * 0.5f );
#endif
#if _Sanbox_fluid_solver_option_External_forces_impulse_position_is_random
		SplatYImpulse( _To_single rand( ) / _To_single RAND_MAX * ProblemHeight( ) );
		SplatXImpulse( _To_single rand( ) / _To_single RAND_MAX * ProblemWidth( ) );
#endif
		SplatRadius( 5.0f );

		SplatXOffset( SplatXImpulse( ) - SplatRadius( ) );
		SplatYOffset( SplatYImpulse( ) - SplatRadius( ) );

#if _Sanbox_fluid_solver_option_External_forces_magnitude_is_static
		ForceXMagnitude( s_positionX * 100.0f );
		ForceYMagnitude( s_positionY * 100.0f );
#endif

#if _Sanbox_fluid_solver_option_External_forces_magnitude_is_random_static
		auto randx = _To_single rand( ) / _To_single RAND_MAX;
		auto randy = _To_single rand( ) / _To_single RAND_MAX;
		randx *= rand( ) % 2 ? 1.0f : -1.0f;
		randy *= rand( ) % 3 ? 1.0f : -1.0f;
		ForceXMagnitude( randx * 1000.0f );
		ForceYMagnitude( randy * 1000.0f );
#endif

#if _Sanbox_fluid_solver_option_External_forces_magnitude_is_random_variable
		auto randx = _To_single rand( ) / _To_single RAND_MAX;
		auto randy = _To_single rand( ) / _To_single RAND_MAX;
		randx *= rand( ) % 2 ? 1.0f : -1.0f;
		randy *= rand( ) % 3 ? 1.0f : -1.0f;
		ForceXMagnitude( randx * 10.0f * s_iteration );
		ForceYMagnitude( randy * 10.0f * s_iteration );
#endif

		m_d3dContext->UpdateSubresource( m_forcePerFrame.Get( ), 0u, nullptr, &m_forcePerFrameData, 0u, 0u );
		m_d3dContext->CSSetUnorderedAccessViews( 0u, 1u, m_velocityPingPong1Uav.GetAddressOf( ), m_zeroUavInitCounts );
		m_d3dContext->CSSetShaderResources( 0u, 1u, m_velocityPingPong0Srv.GetAddressOf( ) );

		m_d3dContext->CSSetShader( m_copyCs.Get( ), nullptr, 0u );
		m_d3dContext->Dispatch( _To_uint ProblemWidth( ), _To_uint ProblemHeight( ), 1 );

		m_d3dContext->CSSetShader( m_gaussianSplatCs.Get( ), nullptr, 0u );
		m_d3dContext->Dispatch( _To_uint SplatRadius( ) * 2 + 1, _To_uint SplatRadius( ) * 2 + 1, 1 );

		UnbindResourcesFromPipeline( );
		SwapVelocities( );
	}
	else if ( s_iteration > 700u )
	{
		s_iteration = 1u;
	}

#endif


}

/*
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
Next Step : Calculate unstable velocity divergence
-------------------------------------------------------------------------------------
bind divergence cs
bind m_velocityPingPong0 to srv 0
bind divergence to uav 0
dispatch [ w-2, h-2, 1 ]
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
*/
void Sandbox::FluidSimulator::ComputeVelocityDivergence( )
{
	m_d3dContext->CSSetShader( m_fluidDivergenceCs.Get( ), nullptr, 0u );

	m_d3dContext->CSSetUnorderedAccessViews( 0u, 1u, m_divergenceUav.GetAddressOf( ), m_zeroUavInitCounts );
	m_d3dContext->CSSetShaderResources( 0u, 1u, m_velocityPingPong0Srv.GetAddressOf( ) );
	m_d3dContext->Dispatch( _To_uint ProblemWidth( ) - 2, _To_uint ProblemHeight( ) - 2, 1 );

	UnbindResourcesFromPipeline( );
}

/*
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
Final Step : Apply projection operator
-------------------------------------------------------------------------------------
bind projection cs
bind m_velocityPingPong1 to uav 0
bind m_velocityPingPong0 to srv 0
bind m_pressurePingPong0 to srv 1
swap v
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
*/
void Sandbox::FluidSimulator::MakeIncompressible( )
{
	m_d3dContext->CSSetShader( m_fluidProjectionCs.Get( ), nullptr, 0u );

	m_d3dContext->CSSetUnorderedAccessViews( 0u, 1u, m_velocityPingPong1Uav.GetAddressOf( ), m_zeroUavInitCounts );
	m_d3dContext->CSSetShaderResources( 0u, 1u, m_velocityPingPong0Srv.GetAddressOf( ) );
	m_d3dContext->CSSetShaderResources( 1u, 1u, m_pressurePingPong0Srv.GetAddressOf( ) );
	m_d3dContext->Dispatch( _To_uint ProblemWidth( ) - 2, _To_uint ProblemHeight( ) - 2, 1 );

	UnbindResourcesFromPipeline( );
	SwapVelocities( );
}