#pragma once

#include "Precompiled.h"
#include "RendererUtil.h"

#define _Sandbox_fluid_solver_explicit_boundaries 0
#define _Sandbox_texture2_rw_set(name) \
	Rendering::Texture2 name; \
	Rendering::SRView name##Srv; \
	Rendering::UAView name##Uav;

namespace Sandbox
{


	class FluidSimulator
	{

#pragma region Discussion

		/*

		-------------------------------------------------------------------------------------
		Fluid velocity is represented and updated using two 2d<float4> ping-pong textures.
		Fluid pressure is represented and updated using two 2d<float4> ping-pong textures.
		Initially, the values in this textures are set to zero. Optionally, this initial
		condition can be modified.
		-------------------------------------------------------------------------------------
		Fluid simulation algorithm:
		-------------------------------------------------------------------------------------
		Shortcuts used in here:
		-------------------------------------------------------------------------------------
		m_velocityPingPong0 = u0
		m_velocityPingPong1 = u1
		m_pressurePingPong0 = p0
		m_pressurePingPong1 = p1
		m_divergence = div
		-------------------------------------------------------------------------------------
		swap v = swap[ u0 <-> u1 ]
		swap p = swap[ p0 <-> p1 ]
		-------------------------------------------------------------------------------------
		-------------------------------------------------------------------------------------
		First Step : Configure computation pipeline
		-------------------------------------------------------------------------------------
		bind constant buffers
		bind sampler
		etc
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
		Next Step : Calculate divergent velocity againts viscous diffusion
		-------------------------------------------------------------------------------------
		(jacobi solver is already bound to the pipeline)
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
		Here we can apply force, temperature, buoyancy and convection, vorticity
		confinement for low-viscous fluids (smoke, air, etc)
		-------------------------------------------------------------------------------------
		And then:
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

#pragma endregion

	public_shared_access:

		_Reign_apply_dx_cb_alignment struct Problem
		//_Reign_apply_dx_cb_alignment struct Problem
		{
			Reign::Vector4 ProblemFactors;
			Reign::Vector4 PoissonFactors;
		};

		_Reign_apply_dx_cb_alignment struct ForcePerFrame
		{
			Reign::Vector4 Force;
			Reign::Vector4 Splat;
			Reign::Vector4 Offset;

			// x - force proj x
			// y - force proj y

			// x - force impulse pos x
			// y - force impulse pos y
			// z - desired radius

			// Precomputed:
			// x - x offset = g_Splat.x - g_Splat.z
			// y - y offset = g_Splat.y - g_Splat.z
		};

	public_dtor_access:

		~FluidSimulator( _No_args_ );

	public_shared_access:

		FluidSimulator( _No_args_ );

		void Initialize( _No_args_ );
		void CreateWindowSizeDependentResources( _No_args_ );
		void CreateDeviceIndependentResources( _No_args_ );
		void CreateDeviceResources( _No_args_ );

		void UpdateForWindowSizeChange( _In_ Sandbox::EventArgsForWindowSizeChange args );

		bool OnFrameMove( _In_ Sehnsucht::BasicTimerViewPtr timer );
		bool OnFrame( );

	public_shared_access:

		_Decl_value_prop_hinl1( m_wicFactory, Rendering::WICImagingFactoryPtr, WICFactory );
		_Decl_value_prop_hinl1( m_d3dContext, Rendering::Context3Ptr, Context3 );
		_Decl_value_prop_hinl1( m_d3dDevice, Rendering::Device3Ptr, Device3 );

		_Decl_value_prop_hinl2( m_problemParamsData, Problem, ProblemParameters );
		_Decl_value_prop_finl1( m_problemParamsData.ProblemFactors.w, float32, ProblemBoundaryScale );
		_Decl_value_prop_finl1( m_problemParamsData.ProblemFactors.z, float32, ProblemTimeStep );
		_Decl_value_prop_finl1( m_problemParamsData.ProblemFactors.y, float32, ProblemHeight );
		_Decl_value_prop_finl1( m_problemParamsData.ProblemFactors.x, float32, ProblemWidth );

		_Decl_value_prop_finl1( m_problemParamsData.PoissonFactors.y, float32, PoissonSolverReciprocalBeta );
		_Decl_value_prop_finl1( m_problemParamsData.PoissonFactors.z, float32, PoissonSolverGridStep );
		_Decl_value_prop_finl1( m_problemParamsData.PoissonFactors.x, float32, PoissonSolverAlpha );

		_Decl_value_prop_hinl2( m_forcePerFrameData, ForcePerFrame, ForcePerFrameParameters );
		_Decl_value_prop_finl1( m_forcePerFrameData.Force.x, float32, ForceXMagnitude );
		_Decl_value_prop_finl1( m_forcePerFrameData.Force.y, float32, ForceYMagnitude );
		_Decl_value_prop_finl1( m_forcePerFrameData.Offset.x, float32, SplatXOffset );
		_Decl_value_prop_finl1( m_forcePerFrameData.Offset.y, float32, SplatYOffset );
		_Decl_value_prop_finl1( m_forcePerFrameData.Splat.x, float32, SplatXImpulse );
		_Decl_value_prop_finl1( m_forcePerFrameData.Splat.y, float32, SplatYImpulse );
		_Decl_value_prop_finl1( m_forcePerFrameData.Splat.z, float32, SplatRadius );

		_Decl_value_prop_finl1( m_fluidViscosity, float32, Viscosity );

		REIGN_FORCEINLINING Rendering::SRViewPtr GetVelocityField( ) { return m_velocityPingPong0Srv.Get( ); }
		REIGN_FORCEINLINING Rendering::SRViewPtr GetPressureField( ) { return m_pressurePingPong0Srv.Get( ); }
		REIGN_FORCEINLINING Rendering::SRViewPtr GetDivergenceField( ) { return m_divergenceSrv.Get( ); }

		REIGN_FORCEINLINING Rendering::SRViewPtr *VelocityField( ) { return m_velocityPingPong0Srv.GetAddressOf( ); }
		REIGN_FORCEINLINING Rendering::SRViewPtr *PressureField( ) { return m_pressurePingPong0Srv.GetAddressOf( ); }
		REIGN_FORCEINLINING Rendering::SRViewPtr *DivergenceField( ) { return m_divergenceSrv.GetAddressOf( ); }

		REIGN_FORCEINLINING float32 Height( ) { return m_problemParamsData.ProblemFactors.y; }
		REIGN_FORCEINLINING float32 Width( ) { return m_problemParamsData.ProblemFactors.x; }

	protected_shared_access:

		void ConfigureSimulationPipeline( );

		void ComputeAdvection( );
		void ComputeViscousDiffusion( );
		void ComputePressure( );
		void ComputeExternalForces( );
		void ComputeVelocityDivergence( );
		void MakeIncompressible( );

		REIGN_FORCEINLINING void SwapVelocities( ) { m_velocityPingPong0Srv.Swap( m_velocityPingPong1Srv ); m_velocityPingPong0Uav.Swap( m_velocityPingPong1Uav ); }
		//REIGN_FORCEINLINING void SwapVelocities( ) { m_velocityPingPong0.Swap( m_velocityPingPong1 ); }
		REIGN_FORCEINLINING void SwapPressures( ) { m_pressurePingPong0Srv.Swap( m_pressurePingPong1Srv ); m_pressurePingPong0Uav.Swap( m_pressurePingPong1Uav ); }
		//REIGN_FORCEINLINING void SwapPressures( ) { m_pressurePingPong0.Swap( m_pressurePingPong1 ); }

		REIGN_HINTINLINING void UnbindResourcesFromPipeline( ) 
		{
			m_d3dContext->CSSetUnorderedAccessViews( 0, 1, m_nullUavs, m_zeroUavInitCounts );
			m_d3dContext->CSSetShaderResources( 0, 2, m_nullSrvs );
		}

	protected_shared_access:


		Rendering::WICImagingFactoryPtr m_wicFactory;
		Rendering::Context3Ptr m_d3dContext;
		Rendering::Device3Ptr m_d3dDevice;

		Rendering::CShader m_fluidAdvectionCs;
		Rendering::CShader m_fluidDivergenceCs;
		Rendering::CShader m_fluidProjectionCs;

		Rendering::CShader m_pressureJacobiCs;
		Rendering::CShader m_diffusionJacobiCs;
		Rendering::CShader m_gaussianSplatCs;
		Rendering::CShader m_zeroCs;
		Rendering::CShader m_copyCs;

		Rendering::SamplerState m_linearSampler;
		Rendering::BufferPtr m_cbuffers[ 2 ];
		Rendering::Buffer m_problemParams;
		Rendering::Buffer m_forcePerFrame;

		ForcePerFrame m_forcePerFrameData;
		Problem m_problemParamsData;
		float32 m_fluidViscosity;

		Rendering::SRViewPtr m_nullSrvs[ 2 ];
		Rendering::UAViewPtr m_nullUavs[ 1 ];
		uint32 m_zeroUavInitCounts[ 1 ];


		/// Resources

		_Sandbox_texture2_rw_set( m_velocityPingPong0 );
		_Sandbox_texture2_rw_set( m_velocityPingPong1 );
		_Sandbox_texture2_rw_set( m_pressurePingPong0 );
		_Sandbox_texture2_rw_set( m_pressurePingPong1 );
		_Sandbox_texture2_rw_set( m_divergence );

	};
};