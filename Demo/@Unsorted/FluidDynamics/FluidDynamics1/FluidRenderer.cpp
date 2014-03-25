#include "Precompiled.h"
#include "FluidRenderer.h"

Sandbox::FluidRenderer::~FluidRenderer( _No_args_ )
{
}

Sandbox::FluidRenderer::FluidRenderer( _No_args_ ) : m_fluidSolver( )
{
	ZeroValueArray( m_zeroUavInitCounts, ARRAYSIZE( m_zeroUavInitCounts ) );
	ZeroValueArray( m_nullUavs, ARRAYSIZE( m_nullUavs ) );
	ZeroValueArray( m_nullSrvs, ARRAYSIZE( m_nullSrvs ) );
}

void Sandbox::FluidRenderer::CreateWindowSizeDependentResources( _No_args_ )
{
}

void Sandbox::FluidRenderer::CreateDeviceIndependentResources( _No_args_ )
{
}

void Sandbox::FluidRenderer::CreateDeviceResources( _No_args_ )
{
	Sehnsucht::IO::BasicLoader loader( m_d3dDevice, m_wicFactory );
	auto loaderPtr = &loader;
	{ // Load vertex shader
		D3D11_INPUT_ELEMENT_DESC polygonLayout[ 2 ];
		ZeroValueArray( polygonLayout, ARRAYSIZE( polygonLayout ) );
		polygonLayout[ 0 ].SemanticName = "Position";
		polygonLayout[ 0 ].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[ 0 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[ 0 ].AlignedByteOffset = 0;
		polygonLayout[ 1 ].SemanticName = "TexCoord";
		polygonLayout[ 1 ].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[ 1 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[ 1 ].AlignedByteOffset = 16;

		loaderPtr->LoadShader(
			L"FluidBoxVs0.cso",
			polygonLayout, ARRAYSIZE( polygonLayout ),
			m_fluidBoxVs.GetAddressOf( ),
			m_fluidBoxLayout.GetAddressOf( )
			);
	}
	{ // Vertex and index buffers
		Converter converter;
		Sehnsucht::BasicShapes::CreateVerticalRect(
			&converter, Reign::Vector2( 5.f, 5.f ),
			m_fluidBoxMeshVerticesData, m_fluidBoxMeshIndicesData
			);
		Sehnsucht::BasicShapes::CreateVertexBuffer(
			m_d3dDevice, _To_uint m_fluidBoxMeshVerticesData.size( ), m_fluidBoxMeshVerticesData.data( ),
			m_fluidBoxMeshVertices.GetAddressOf( )
			);
		Sehnsucht::BasicShapes::CreateIndexBuffer(
			m_d3dDevice, _To_uint m_fluidBoxMeshIndicesData.size( ), m_fluidBoxMeshIndicesData.data( ),
			m_fluidBoxMeshIndices.GetAddressOf( )
			);
	}
	{ // Create constant buffers
		loaderPtr->CreateConstantBuffer( m_fluidBoxSpatial.GetAddressOf( ), &m_fluidBoxSpatialData );
	}
	{ // Load pixel shader
		loaderPtr->LoadShader(
			L"FluidBoxPs0.cso",
			m_fluidBoxPs.GetAddressOf( )
			);
	}
	{ // Create sampler states

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroValue( &samplerDesc );

		// Create a texture sampler state description.
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_d3dDevice->CreateSamplerState( &samplerDesc, m_linearSampler.GetAddressOf( ) );
	}
}

void Sandbox::FluidRenderer::UpdateForWindowSizeChange( _In_ Sandbox::EventArgsForWindowSizeChange args )
{
	// ...

	m_fluidSolver.UpdateForWindowSizeChange( args );
	m_d3dContext->PSSetShaderResources( 0, 1, m_fluidSolver.VelocityField( ) );
}

bool Sandbox::FluidRenderer::OnFrameMove( _In_ Sehnsucht::BasicTimerViewPtr timer )
{
	UnbindResourcesFromPipeline( );
	m_fluidSolver.OnFrameMove( timer );
	return true;
}

bool Sandbox::FluidRenderer::OnFrame( )
{
	m_fluidSolver.OnFrame( );

	m_d3dContext->VSSetShader( m_fluidBoxVs.Get( ), nullptr, 0u );

	m_d3dContext->UpdateSubresource( m_fluidBoxSpatial.Get( ), 0u, nullptr, &m_fluidBoxSpatialData, 0u, 0u );
	m_d3dContext->VSSetConstantBuffers( 0, 1, m_fluidBoxSpatial.GetAddressOf( ) );

	m_d3dContext->PSSetShader( m_fluidBoxPs.Get( ), nullptr, 0u );
	m_d3dContext->PSSetSamplers( 0, 1, m_linearSampler.GetAddressOf( ) );
	//m_d3dContext->PSSetShaderResources( 0, 1, m_fluidSolver.DivergenceField( ) );
	//m_d3dContext->PSSetShaderResources( 0, 1, m_fluidSolver.PressureField( ) );
	m_d3dContext->PSSetShaderResources( 0, 1, m_fluidSolver.VelocityField( ) );

	m_d3dContext->DrawIndexed( _To_uint m_fluidBoxMeshIndicesData.size( ), 0, 0 );

	UnbindResourcesFromPipeline( );

	return true;
}

void Sandbox::FluidRenderer::Initialize( _No_args_ )
{
	CreateDeviceIndependentResources( );
	CreateDeviceResources( );
	CreateWindowSizeDependentResources( );

	m_fluidSolver.Initialize( );

	uint32 strides[ 1 ] = { sizeof Vertex };
	uint32 offsets[ 1 ] = { 0u };

	m_d3dContext->IASetInputLayout( m_fluidBoxLayout.Get( ) );
	m_d3dContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_d3dContext->IASetVertexBuffers( 0, 1, m_fluidBoxMeshVertices.GetAddressOf( ), strides, offsets );
	m_d3dContext->IASetIndexBuffer( m_fluidBoxMeshIndices.Get( ), SEHNSUCHT_DEFAULT16_INDEX_BUFFER_DXGI_FORMAT, 0u );

	m_d3dContext->VSSetShader( m_fluidBoxVs.Get( ), nullptr, 0u );
	m_d3dContext->VSSetConstantBuffers( 0, 1, m_fluidBoxSpatial.GetAddressOf( ) );

	m_d3dContext->PSSetShader( m_fluidBoxPs.Get( ), nullptr, 0u );
	m_d3dContext->PSSetSamplers( 0, 1, m_linearSampler.GetAddressOf( ) );
	m_d3dContext->PSSetShaderResources( 0, 1, m_fluidSolver.VelocityField( ) );

}

void Sandbox::FluidRenderer::Converter::FillVertex(
	_In_ Reign::Vector3 const &positionLocal, // Vertex position in the model space.
	_In_ Reign::Vector3 const &normalLocal, // Vertex normal in the model space.
	_In_ Reign::Vector2 const &texCoord, // Vertex texture coordinates.
	_In_ Sehnsucht::BasicShapes::ProvidedData providedDataMask, // Specifies what data is actually provided.
	_Inout_ Vertex *targetVertex // Vertex to fill the data in.
	)
{
	using namespace Sehnsucht;

	const bool writePosition = IsFlagChecked( providedDataMask, Sehnsucht::BasicShapes::ProvidedData::PositionLocal );
	if ( writePosition ) targetVertex->PositionLocal = Reign::Vector4( positionLocal.x, positionLocal.y, positionLocal.z, 1.0f );
	const bool writeTex = IsFlagChecked( providedDataMask, Sehnsucht::BasicShapes::ProvidedData::TexCoord );
	if ( writeTex ) targetVertex->TexCoord = Reign::Vector2( texCoord.x, texCoord.y );
}
