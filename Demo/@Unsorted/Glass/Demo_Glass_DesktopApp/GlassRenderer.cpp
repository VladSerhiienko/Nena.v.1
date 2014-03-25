#include "Precompiled.h"
#include "GlassRenderer.h"

void Sandbox::GlassRenderer::Converter::FillVertex(
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

Sandbox::GlassRenderer::GlassRenderer( _No_args_ )
{
	ZeroValue( &m_glassSpatialData );
	ZeroValue( &m_glassPerturbData );
}

Sandbox::GlassRenderer::~GlassRenderer( _No_args_ )
{
}

void Sandbox::GlassRenderer::Initialize( _No_args_ )
{
	CreateDeviceIndependentResources( );
	CreateDeviceResources( );
	CreateWindowSizeDependentResources( );
}

void Sandbox::GlassRenderer::CreateWindowSizeDependentResources( _No_args_ )
{

}

void Sandbox::GlassRenderer::CreateDeviceIndependentResources( _No_args_ )
{

}

void Sandbox::GlassRenderer::CreateDeviceResources( _No_args_ )
{
	Sehnsucht::IO::BasicLoader loader( m_d3dDevice, m_wicFactory );
	auto loaderPtr = &loader;
	{ // Load vertex shader
		D3D11_INPUT_ELEMENT_DESC polygonLayout[ 2 ];
		ZeroValueArray( polygonLayout, ARRAYSIZE( polygonLayout ) );
		polygonLayout[ 0 ].SemanticName = "POSITION";
		polygonLayout[ 0 ].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[ 0 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[ 0 ].AlignedByteOffset = 0;
		polygonLayout[ 1 ].SemanticName = "TEXCOORD";
		polygonLayout[ 1 ].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[ 1 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[ 1 ].AlignedByteOffset = 16;

		loaderPtr->LoadShader(
			L"GlassVertexShader.cso",
			polygonLayout, ARRAYSIZE( polygonLayout ),
			m_glassVertexShader.GetAddressOf( ),
			m_glassLayout.GetAddressOf( )
			);
	}
	{ // Load pixel shader
		loaderPtr->LoadShader(
			L"GlassPixelShader.cso",
			m_glassPixelShader.GetAddressOf( )
			);
	}
	{
		loaderPtr->LoadTexture(
			L"Assets/ice01.dds",
			m_glassColorTexture.GetAddressOf( ),
			m_glassColorTextureView.GetAddressOf( )
			);
		loaderPtr->LoadTexture(
			L"Assets/icebump01.dds",
			m_glassNormalTexture.GetAddressOf( ),
			m_glassNormalTextureView.GetAddressOf( )
			);

		m_glassPixelShaderTextureViews[ 0 ] = m_glassColorTextureView.Get( );
		m_glassPixelShaderTextureViews[ 1 ] = m_glassNormalTextureView.Get( );
		m_glassPixelShaderTextureViews[ 2 ] = m_sceneRenderTarget->GetReadViewPtr( );
	}

	{ // Create constant buffers
		loaderPtr->CreateConstantBuffer( m_glassSpatial.GetAddressOf( ), &m_glassSpatialData );
		loaderPtr->CreateConstantBuffer( m_glassPerturb.GetAddressOf( ), &m_glassPerturbData );
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

		m_d3dDevice->CreateSamplerState( &samplerDesc, m_glassLinearSampler.GetAddressOf( ) );
	}
	{
		Converter converter;
		Sehnsucht::BasicShapes::CreateVerticalRect(
			&converter, Reign::Vector2( 5.f, 5.f ),
			m_glassVerticesData, m_glassIndicesData
			);
		Sehnsucht::BasicShapes::CreateVertexBuffer(
			m_d3dDevice, _To_uint m_glassVerticesData.size( ), m_glassVerticesData.data( ),
			m_glassVertices.GetAddressOf( )
			);
		Sehnsucht::BasicShapes::CreateIndexBuffer(
			m_d3dDevice, _To_uint m_glassIndicesData.size( ), m_glassIndicesData.data( ),
			m_glassIndices.GetAddressOf( )
			);
	}
}

void Sandbox::GlassRenderer::UpdateForWindowSizeChange( _In_ Sandbox::EventArgsForWindowSizeChange args )
{
}