#include "Precompiled.h"
#include "FluidRenderer.h"

void Demo::FluidRenderer::CreateBuffers( )
{
	{
		// Simple orthographic projection to display the entire map
		DirectX::XMMATRIX mView = DirectX::XMMatrixTranslation( -SimulationMapWidthAsFloat / 2.0f, -SimulationMapHeightAsFloat / 2.0f, 0 );
		DirectX::XMMATRIX mProjection = DirectX::XMMatrixOrthographicLH( SimulationMapWidthAsFloat, SimulationMapHeightAsFloat, 0, 1 );
		DirectX::XMMATRIX mViewProjection = mView * mProjection;

		RenderConstants BufferData;
		Reign::Standard::ZeroValue( &BufferData );
		//DirectX::XMStoreFloat4x4( &BufferData.mViewProj, XMMatrixTranspose( mViewProjection ) );
		DirectX::XMStoreFloat4x4( &BufferData.mViewProj, mViewProjection );
		BufferData.fParticleSize = SimulatorParticleSizeAsFloat;

		CreateConstantBuffer<RenderConstants>(
			m_Device3.Get(), m_RenderConstants.GetAddressOf(), 
			&BufferData
			);

		SetDebugName( m_RenderConstants.Get(), "FluidRendererCBuffer" );
	}

	Demo::FluidSimulator::CreateBuffers(m_Device3.Get());

	m_Context3.Get()->OMSetRenderTargets(
		1, m_RenderTargetView3.GetAddressOf(), 
		m_DepthStencilView3.Get()
		);
}

void Demo::FluidRenderer::CreateShaders( )
{
	Demo::FluidSimulator::CreateShaders(m_Device3.Get());

#ifdef REIGN_DESKTOP
	std::unique_ptr<Sehnsucht::IO::BasicLoader> Loader(
		new Sehnsucht::IO::BasicLoader(m_Device3.Get())
		);
#else
	Sehnsucht::IO::BasicLoader^ Loader = 
		ref new Sehnsucht::IO::BasicLoader(
			pDevice
			);
#endif

	Loader->LoadShader(
		L"FluidSimulatorVertexShader.cso",
		nullptr, 0, m_Vs.GetAddressOf(),
		nullptr
		);
	Loader->LoadShader( 
		L"FluidSimulatorGeometryShader.cso", 
		m_Gs.GetAddressOf() 
		);
	Loader->LoadShader( 
		L"FluidSimulatorPixelShader.cso", 
		m_Ps.GetAddressOf() 
		);
}

void Demo::FluidRenderer::OnFrameMove( )
{
	static const BufferPtr pNullBuffer = nullptr;
	static const SRViewPtr pNullSRV = nullptr;
	static const UInt32 uNullUInt = 0u;

	m_Context3.Get()->VSSetShaderResources( 0, 1, &pNullSRV );
	m_Context3.Get()->VSSetShaderResources( 1, 1, &pNullSRV );

	m_Context3.Get()->VSSetShader( nullptr, nullptr, 0u );
	m_Context3.Get()->GSSetShader( nullptr, nullptr, 0u );
	m_Context3.Get()->PSSetShader( nullptr, nullptr, 0u );

	Demo::FluidSimulator::OnFrameMove(m_Context3.Get());

	m_Context3.Get()->VSSetShader( m_Vs.Get(), nullptr, 0u );
	m_Context3.Get()->GSSetShader( m_Gs.Get(), nullptr, 0u );
	m_Context3.Get()->PSSetShader( m_Ps.Get(), nullptr, 0u );

	//m_Context3.Get()->VSSetConstantBuffers( 0, 1, m_RenderConstants.GetAddressOf() );
	m_Context3.Get()->GSSetConstantBuffers( 0, 1, m_RenderConstants.GetAddressOf() );
	//m_Context3.Get()->PSSetConstantBuffers( 0, 1, m_RenderConstants.GetAddressOf() );

	m_Context3.Get()->VSSetShaderResources( 0, 1, m_ParticlesRO.GetAddressOf() );
	m_Context3.Get()->VSSetShaderResources( 1, 1, m_ParticlesDensityRO.GetAddressOf() );

	m_Context3.Get()->IASetVertexBuffers( 0, 1, &pNullBuffer, &uNullUInt, &uNullUInt );
	m_Context3.Get()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
}

void Demo::FluidRenderer::OnFrame( )
{
	//Demo::FluidSimulator::OnFrame(pContext);

	m_Context3.Get()->OMSetRenderTargets(
		1, m_RenderTargetView3.GetAddressOf(), 
		m_DepthStencilView3.Get()
		);

    Float32 ClearColor[4] = { 0.0f };

	m_Context3.Get()->ClearRenderTargetView(
		m_RenderTargetView3.Get(), 
		ClearColor
		);

	m_Context3.Get()->ClearDepthStencilView(
		m_DepthStencilView3.Get(), 
		D3D11_CLEAR_DEPTH, 
		1.0f, 0ui8
		);

	m_Context3.Get()->Draw( 
		SimulatorNumParticles, 
		0u 
		);

	Present();

}
