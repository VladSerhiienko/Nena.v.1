#pragma once
//#include "SceneRenderer.h"
//#include "GlassRenderer.h"
#include "FluidRenderer.h"
#include "Camera.h"

namespace Sandbox
{
	ref_class_decl Renderer : public Sehnsucht::DirectXBaseDesktop
	{
		typedef CRITICAL_SECTION CriticalSection;

	protected_shared_access:

		void CreateDeviceResources( _No_args_ );
		void CreateDeviceIndependentResources( _No_args_ );
		void CreateWindowSizeDependentResources( _No_args_ );

	public_shared_access:

		Renderer( );
		void Initialize( );
		void UpdateForWindowSizeChange( );

		REIGN_FORCEINLINING void OnFrameMove( _In_ Sehnsucht::BasicTimerViewPtr timer )
		{
			static uint32 s_iteration = 0u;
			static _Short_dbg_msg s_msg;

			if ( s_iteration == 50u )
			{
				sprintf_s( s_msg, "fps = %d", timer->Framerate );
				PrintToDebugWindowOnly8( s_msg );
				s_iteration = 0u;
			}


			// Update time dependent values
			EnterCriticalSection( &m_lock );

			m_fluidRenderer.EditFluidBoxSpatial( )->World = Reign::Matrix::CreateTranslation( 0.0f, 0.0f, 5.0f );
			m_fluidRenderer.EditFluidBoxSpatial( )->View = m_camera.GetViewCpy( );
			m_fluidRenderer.EditFluidBoxSpatial( )->Proj = m_camera.GetProjCpy( );
			m_fluidRenderer.OnFrameMove( timer );

			s_iteration++;
			LeaveCriticalSection( &m_lock );
		}

		REIGN_FORCEINLINING void OnFrame( _No_args_ )
		{
			// Pipeline reconfig + Draw calls

			//static const float32 s_factor [ ] = { 0.0f, 0.0f, 0.0f, 0.0f };
			//static const auto s_sample = 0xffffffff;
			//static const auto s_offset = 0u;

			EnterCriticalSection( &m_lock );
			BindToPipeline( m_d3dContext.Get() );
			Clear( m_d3dContext.Get( ) );

			m_fluidRenderer.OnFrame( );

			PresentVSync( );
			LeaveCriticalSection( &m_lock );
		}

	public_dtor_access:

		~Renderer( _No_args_ );

	protected_shared_access:

		Sandbox::EventArgsForWindowSizeChange m_windowSizeChangedArgs;
		Rendering::RasterizerState m_rasterizerCcw;
		Rendering::BlendState m_blendingEnabled;
		Sandbox::FluidRenderer m_fluidRenderer;

		CriticalSection m_lock;
		Camera m_camera;
	};
};