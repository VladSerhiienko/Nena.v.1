#pragma once

#include "FluidSimulator.h"

namespace Sandbox
{
	class FluidRenderer
	{
	public_shared_access:

		struct Vertex
		{
			Reign::Vector4 PositionLocal;
			Reign::Vector2 TexCoord;
		};

		struct Converter : public Sehnsucht::BasicShapes::IVertexDataProvider<Vertex>
		{
			virtual void FillVertex(
				_In_ Reign::Vector3 const &positionLocal, // Vertex position in the model space.
				_In_ Reign::Vector3 const &normalLocal, // Vertex normal in the model space.
				_In_ Reign::Vector2 const &texCoord, // Vertex texture coordinates.
				_In_ Sehnsucht::BasicShapes::ProvidedData providedDataMask, // Specifies what data is actually provided.
				_Inout_ Vertex *targetVertex // Vertex to fill the data in.
				) override;
		};

		_Reign_apply_dx_cb_alignment struct Spatial
		{
			Reign::Matrix World;
			Reign::Matrix View;
			Reign::Matrix Proj;
		};

	public_dtor_access:

		~FluidRenderer( _No_args_ );

	public_shared_access:

		FluidRenderer( _No_args_ );

		void CreateWindowSizeDependentResources( _No_args_ );
		void CreateDeviceIndependentResources( _No_args_ );
		void CreateDeviceResources( _No_args_ );
		void Initialize( _No_args_ );

		void UpdateForWindowSizeChange( _In_ Sandbox::EventArgsForWindowSizeChange args );
		bool OnFrameMove( _In_ Sehnsucht::BasicTimerViewPtr timer );
		bool OnFrame( );

	public_shared_access:

		_Decl_value_prop_hinl1_( m_wicFactory, Rendering::WICImagingFactoryPtr, WICFactory, _No_actions, { m_fluidSolver.WICFactory( value ); } );
		_Decl_value_prop_hinl1_( m_d3dContext, Rendering::Context3Ptr, Context3, _No_actions, { m_fluidSolver.Context3( value ); } );
		_Decl_value_prop_hinl1_( m_d3dDevice, Rendering::Device3Ptr, Device3, _No_actions, { m_fluidSolver.Device3( value ); } );

		REIGN_HINTINLINING FluidSimulator const *GetSolver( ) const { return &m_fluidSolver; }
		REIGN_HINTINLINING FluidSimulator *EditSolver( ) { return &m_fluidSolver; }
		_Decl_value_prop_hinl2( m_fluidBoxSpatialData, Spatial, FluidBoxSpatial );

		REIGN_HINTINLINING void UnbindResourcesFromPipeline( )
		{
			m_d3dContext->VSSetShaderResources( 0, 2, m_nullSrvs );
			m_d3dContext->PSSetShaderResources( 0, 2, m_nullSrvs );
		}

	protected_shared_access:

		Rendering::WICImagingFactoryPtr m_wicFactory;
		Rendering::Context3Ptr m_d3dContext;
		Rendering::Device3Ptr m_d3dDevice;

		Rendering::VSLayout m_fluidBoxLayout;
		Rendering::VShader m_fluidBoxVs;
		Rendering::PShader m_fluidBoxPs;

		Rendering::Buffer m_fluidBoxMeshVertices;
		Rendering::Buffer m_fluidBoxMeshIndices;
		Rendering::Buffer m_fluidBoxSpatial;

		std::vector<Vertex> m_fluidBoxMeshVerticesData;
		std::vector<uint16> m_fluidBoxMeshIndicesData;
		Spatial m_fluidBoxSpatialData;

		Rendering::SamplerState m_linearSampler;

		Rendering::SRViewPtr m_nullSrvs[ 2 ];
		Rendering::UAViewPtr m_nullUavs[ 1 ];
		uint32 m_zeroUavInitCounts[ 1 ];

		FluidSimulator m_fluidSolver;

	};
}