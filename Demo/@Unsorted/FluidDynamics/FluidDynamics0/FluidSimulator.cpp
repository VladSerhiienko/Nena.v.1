#include "Precompiled.h"
#include "FluidSimulator.h"

void Sandbox::FluidSimulator::CreateWindowSizeDependentResources( _No_args_ )
{
}

void Sandbox::FluidSimulator::CreateDeviceIndependentResources( _No_args_ )
{
}

void Sandbox::FluidSimulator::CreateDeviceResources( _No_args_ )
{
	Sehnsucht::IO::BasicLoader loader( m_d3dDevice, m_wicFactory ); auto loaderPtr = &loader;
	{ // Create shaders
		loaderPtr->LoadShader( L"DiffusionJacobiCs2.cso", m_diffusionJacobiCs.GetAddressOf( ) );
		loaderPtr->LoadShader( L"PressureJacobiCs2.cso", m_pressureJacobiCs.GetAddressOf( ) );
		loaderPtr->LoadShader( L"ProjectionCs1.cso", m_fluidProjectionCs.GetAddressOf( ) );
		loaderPtr->LoadShader( L"DivergenceCs0.cso", m_fluidDivergenceCs.GetAddressOf( ) );
		loaderPtr->LoadShader( L"AdvectionCs1.cso", m_fluidAdvectionCs.GetAddressOf( ) );
		loaderPtr->LoadShader( L"GaussianSplatCs0.cso", m_gaussianSplatCs.GetAddressOf( ) );
		loaderPtr->LoadShader( L"ZeroTextureCs0.cso", m_zeroCs.GetAddressOf( ) );
		loaderPtr->LoadShader( L"CopyTextureCs0.cso", m_copyCs.GetAddressOf( ) );
	}
	{ // Create constant buffers
		loaderPtr->CreateConstantBuffer( m_problemParams.GetAddressOf( ), &m_problemParamsData );
		loaderPtr->CreateConstantBuffer( m_forcePerFrame.GetAddressOf( ), &m_forcePerFrameData );
		m_cbuffers[ 0 ] = m_problemParams.Get( );
		m_cbuffers[ 1 ] = m_forcePerFrame.Get( );
	}
	{ // Create textures
		Rendering::Texture2Description problemTextureDesc; ZeroValue( &problemTextureDesc );
		problemTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		problemTextureDesc.Height = _To_uint GetProblemParameters( )->ProblemFactors.y;
		problemTextureDesc.Width = _To_uint GetProblemParameters( )->ProblemFactors.x;
		problemTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		problemTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		problemTextureDesc.SampleDesc.Quality = 0;
		problemTextureDesc.SampleDesc.Count = 1;
		problemTextureDesc.MipLevels = 1;
		problemTextureDesc.ArraySize = 1;

		m_d3dDevice->CreateTexture2D( &problemTextureDesc, nullptr, m_velocityPingPong0.GetAddressOf( ) );
		m_d3dDevice->CreateTexture2D( &problemTextureDesc, nullptr, m_velocityPingPong1.GetAddressOf( ) );
		m_d3dDevice->CreateTexture2D( &problemTextureDesc, nullptr, m_pressurePingPong0.GetAddressOf( ) );
		m_d3dDevice->CreateTexture2D( &problemTextureDesc, nullptr, m_pressurePingPong1.GetAddressOf( ) );
		m_d3dDevice->CreateTexture2D( &problemTextureDesc, nullptr, m_divergence.GetAddressOf( ) );

		Rendering::ShaderResourceViewDescription problemTextureSrvDesc; ZeroValue( &problemTextureSrvDesc );
		problemTextureSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		problemTextureSrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		problemTextureSrvDesc.Texture2D.MostDetailedMip = 0;
		problemTextureSrvDesc.Texture2D.MipLevels = 1;

		m_d3dDevice->CreateShaderResourceView( m_velocityPingPong0.Get( ), &problemTextureSrvDesc, m_velocityPingPong0Srv.GetAddressOf( ) );
		m_d3dDevice->CreateShaderResourceView( m_velocityPingPong1.Get( ), &problemTextureSrvDesc, m_velocityPingPong1Srv.GetAddressOf( ) );
		m_d3dDevice->CreateShaderResourceView( m_pressurePingPong0.Get( ), &problemTextureSrvDesc, m_pressurePingPong0Srv.GetAddressOf( ) );
		m_d3dDevice->CreateShaderResourceView( m_pressurePingPong1.Get( ), &problemTextureSrvDesc, m_pressurePingPong1Srv.GetAddressOf( ) );
		m_d3dDevice->CreateShaderResourceView( m_divergence.Get( ), &problemTextureSrvDesc, m_divergenceSrv.GetAddressOf( ) );

		Rendering::UnorderedAccessViewDescription problemTextureUavDesc; ZeroValue( &problemTextureUavDesc );
		problemTextureUavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		problemTextureUavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		problemTextureUavDesc.Texture2D.MipSlice = 0;

		m_d3dDevice->CreateUnorderedAccessView( m_velocityPingPong0.Get( ), &problemTextureUavDesc, m_velocityPingPong0Uav.GetAddressOf( ) );
		m_d3dDevice->CreateUnorderedAccessView( m_velocityPingPong1.Get( ), &problemTextureUavDesc, m_velocityPingPong1Uav.GetAddressOf( ) );
		m_d3dDevice->CreateUnorderedAccessView( m_pressurePingPong0.Get( ), &problemTextureUavDesc, m_pressurePingPong0Uav.GetAddressOf( ) );
		m_d3dDevice->CreateUnorderedAccessView( m_pressurePingPong1.Get( ), &problemTextureUavDesc, m_pressurePingPong1Uav.GetAddressOf( ) );
		m_d3dDevice->CreateUnorderedAccessView( m_divergence.Get( ), &problemTextureUavDesc, m_divergenceUav.GetAddressOf( ) );

		m_d3dContext->CSSetShader( m_zeroCs.Get( ), nullptr, 0u );

		m_d3dContext->CSSetUnorderedAccessViews( 0, 1, m_velocityPingPong0Uav.GetAddressOf( ), m_zeroUavInitCounts );
		m_d3dContext->Dispatch( problemTextureDesc.Width, problemTextureDesc.Height, 1 );
		m_d3dContext->CSSetUnorderedAccessViews( 0, 1, m_velocityPingPong1Uav.GetAddressOf( ), m_zeroUavInitCounts );
		m_d3dContext->Dispatch( problemTextureDesc.Width, problemTextureDesc.Height, 1 );
		m_d3dContext->CSSetUnorderedAccessViews( 0, 1, m_pressurePingPong0Uav.GetAddressOf( ), m_zeroUavInitCounts );
		m_d3dContext->Dispatch( problemTextureDesc.Width, problemTextureDesc.Height, 1 );
		m_d3dContext->CSSetUnorderedAccessViews( 0, 1, m_pressurePingPong1Uav.GetAddressOf( ), m_zeroUavInitCounts );
		m_d3dContext->Dispatch( problemTextureDesc.Width, problemTextureDesc.Height, 1 );
		m_d3dContext->CSSetUnorderedAccessViews( 0, 1, m_divergenceUav.GetAddressOf( ), m_zeroUavInitCounts );
		m_d3dContext->Dispatch( problemTextureDesc.Width, problemTextureDesc.Height, 1 );
		m_d3dContext->CSSetUnorderedAccessViews( 0, 1, m_nullUavs, m_zeroUavInitCounts );

		m_d3dContext->CSSetShader( nullptr, nullptr, 0u );
	}
	{ // Create sampler states

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroValue( &samplerDesc );

		// Create a texture sampler state description.
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.MaxAnisotropy = 1;

		m_d3dDevice->CreateSamplerState( &samplerDesc, m_linearSampler.GetAddressOf( ) );
	}
}


void Sandbox::FluidSimulator::UpdateForWindowSizeChange( _In_ Sandbox::EventArgsForWindowSizeChange args )
{
	(void) args;
}

bool Sandbox::FluidSimulator::OnFrameMove( _In_ Sehnsucht::BasicTimerViewPtr timer )
{
	(void) timer;
	ProblemTimeStep( 0.1f );
	return true;
}

bool Sandbox::FluidSimulator::OnFrame( )
{
	ConfigureSimulationPipeline( );
	ComputeAdvection( );
	ComputeViscousDiffusion( );
	ComputeExternalForces( );
	ComputeVelocityDivergence( );
	ComputePressure( );
	MakeIncompressible( );

	return true;
}

void Sandbox::FluidSimulator::Initialize( _No_args_ )
{
	CreateDeviceIndependentResources( );
	CreateDeviceResources( );
	CreateWindowSizeDependentResources( );
}

Sandbox::FluidSimulator::~FluidSimulator( _No_args_ )
{
}

Sandbox::FluidSimulator::FluidSimulator( _No_args_ )
{
	ZeroValueArray( m_zeroUavInitCounts, ARRAYSIZE( m_zeroUavInitCounts ) );
	ZeroValueArray( m_nullUavs, ARRAYSIZE( m_nullUavs ) );
	ZeroValueArray( m_nullSrvs, ARRAYSIZE( m_nullSrvs ) );
	ZeroValue( &m_problemParamsData );
}
