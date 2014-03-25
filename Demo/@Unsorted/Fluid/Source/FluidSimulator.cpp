#include "Precompiled.h"
#include "FluidSimulator.h"

void Demo::FluidSimulator::CreateBuffers( Demo::FluidSimulator::DevicePtr pDevice )
{
	{
		m_Particles = nullptr;
		m_ParticlesRO = nullptr;
		m_ParticlesRW = nullptr;

		m_SortedParticles = nullptr;
		m_SortedParticlesRO = nullptr;
		m_SortedParticlesRW = nullptr;

		m_ParticlesAccel = nullptr;
		m_ParticlesAccelRO = nullptr;
		m_ParticlesAccelRW = nullptr;

		m_ParticlesDensity = nullptr;
		m_ParticlesDensityRO = nullptr;
		m_ParticlesDensityRW = nullptr;

		m_GridIndices = nullptr;
		m_GridIndicesRO = nullptr;
		m_GridIndicesRW = nullptr;

		m_GridTemp = nullptr;
		m_GridTempRO = nullptr;
		m_GridTempRW = nullptr;

		m_Grid = nullptr;
		m_GridRO = nullptr;
		m_GridRW = nullptr;
	}

	{
		const UInt32 uStartWidth = (UInt32)(sqrt((Float32)(SimulatorNumParticles)));
		Particle ParticleArray[SimulatorNumParticles];
		Reign::Standard::ZeroValueArray(
			ParticleArray,
			SimulatorNumParticles
			); 

		UInt32 uParticleId = 0;
		for ( auto &Particle : ParticleArray )
		{
			Particle.vPosition = DirectXFloat32x2(
				(SimulatorInitialParticleSpacingAsFloat * (uParticleId % uStartWidth)) - 0.01, // X
				SimulatorInitialParticleSpacingAsFloat * (uParticleId / uStartWidth) - 0.025  // Y
				);

			uParticleId++;
		}

		CreateStructuredBuffer<Particle>(
			pDevice, SimulatorNumParticles, 
			m_Particles.GetAddressOf(),
			m_ParticlesRO.GetAddressOf(),
			m_ParticlesRW.GetAddressOf(),
			ParticleArray
			);
		
		SetDebugName( m_ParticlesRO.Get(), "FluidParticlesBufferSRV" );
		SetDebugName( m_ParticlesRW.Get(), "FluidParticlesBufferUAV" );
		SetDebugName( m_Particles.Get(), "FluidParticlesBuffer" );

		CreateStructuredBuffer<Demo::FluidSimulator::Particle>(
			pDevice, SimulatorNumParticles, 
			m_SortedParticles.GetAddressOf(),
			m_SortedParticlesRO.GetAddressOf(),
			m_SortedParticlesRW.GetAddressOf(),
			ParticleArray
			);

		SetDebugName( m_SortedParticlesRO.Get(), "SortedFluidParticlesBufferSRV" );
		SetDebugName( m_SortedParticlesRW.Get(), "SortedFluidParticlesBufferUAV" );
		SetDebugName( m_SortedParticles.Get(), "SortedFluidParticlesBuffer" );
	}

	{
		CreateStructuredBuffer<Demo::FluidSimulator::Acceleration>(
			pDevice, SimulatorNumParticles, 
			m_ParticlesAccel.GetAddressOf(),
			m_ParticlesAccelRO.GetAddressOf(),
			m_ParticlesAccelRW.GetAddressOf()
			);

		SetDebugName( m_ParticlesAccelRO.Get(), "FluidParticlesAccelerationSRV" );
		SetDebugName( m_ParticlesAccelRW.Get(), "FluidParticlesAccelerationUAV" );
		SetDebugName( m_ParticlesAccel.Get(), "FluidParticlesAcceleration" );
	}

	{
		CreateStructuredBuffer<Demo::FluidSimulator::Density>(
			pDevice, SimulatorNumParticles, 
			m_ParticlesDensity.GetAddressOf(),
			m_ParticlesDensityRO.GetAddressOf(),
			m_ParticlesDensityRW.GetAddressOf()
			);

		SetDebugName( m_ParticlesDensityRO.Get(), "FluidParticlesDensitySRV" );
		SetDebugName( m_ParticlesDensityRW.Get(), "FluidParticlesDensityUAV" );
		SetDebugName( m_ParticlesDensity.Get(), "FluidParticlesDensity" );
	}

	{
		CreateStructuredBuffer<Reign::Standard::UInt32>(
			pDevice, SimulatorNumParticles, 
			m_Grid.GetAddressOf(),
			m_GridRO.GetAddressOf(),
			m_GridRW.GetAddressOf()
			);

		SetDebugName( m_GridRO.Get(), "FluidParticlesGridSRV" );
		SetDebugName( m_GridRW.Get(), "FluidParticlesGridUAV" );
		SetDebugName( m_Grid.Get(), "FluidParticlesGrid" );
	}

	{
		CreateStructuredBuffer<Reign::Standard::UInt32>(
			pDevice, SimulatorNumParticles, 
			m_GridTemp.GetAddressOf(),
			m_GridTempRO.GetAddressOf(),
			m_GridTempRW.GetAddressOf()
			);

		SetDebugName( m_GridTempRW.Get(), "FluidParticlesGridTempSRV" );
		SetDebugName( m_GridTempRO.Get(), "FluidParticlesGridTempUAV" );
		SetDebugName( m_GridTemp.Get(), "FluidParticlesGridTemp" );
	}

	{
		CreateStructuredBuffer<Demo::FluidSimulator::UInt32x2>(
			pDevice, SimulatorNumParticles, 
			m_GridIndices.GetAddressOf(),
			m_GridIndicesRO.GetAddressOf(),
			m_GridIndicesRW.GetAddressOf()
			);

		SetDebugName( m_GridIndicesRW.Get(), "FluidParticlesGridIndicesSRV" );
		SetDebugName( m_GridIndicesRO.Get(), "FluidParticlesGridIndicesUAV" );
		SetDebugName( m_GridIndices.Get(), "FluidParticlesGridIndices" );
	}

	{
		SimulationConstants BufferData;
		Reign::Standard::ZeroValue( &BufferData );
		BufferData.fSmoothlen = SimulatorSmoothLengthAsFloat;
		BufferData.fLapViscosityCoef = SimulatorParticleMassAsFloat * SimulatorViscosityAsFloat * 45.0f / (DirectX::XM_PI * pow(SimulatorSmoothLengthAsFloat, 6)); 
		BufferData.fDensityCoef = SimulatorParticleMassAsFloat * 315.0f / (64.0f * DirectX::XM_PI * pow(SimulatorSmoothLengthAsFloat, 9));
		BufferData.fGradPressureCoef = SimulatorParticleMassAsFloat * -45.0f / (DirectX::XM_PI * pow(SimulatorSmoothLengthAsFloat, 6)); 
		BufferData.fPressureStiffness = SimulatorPressureStiffnessAsFloat;
		BufferData.fWallStiffness = SimulatorWallStiffnessAsFloat;
		BufferData.vGridDim.x = 1.0f / SimulatorSmoothLengthAsFloat;
		BufferData.vGridDim.y = 1.0f / SimulatorSmoothLengthAsFloat;
		BufferData.fRestDensity = SimulatorRestDensityAsFloat;
		BufferData.fTimeStep = SimulatorMaxTimeStepAsFloat;
		BufferData.iNumParticles = SimulatorNumParticles;
		BufferData.vGravity.x = SimulatorGravityXAsFloat;
		BufferData.vGravity.y = SimulatorGravityYAsFloat;
		BufferData.vPlanes[0].x = 1.0f;		BufferData.vPlanes[0].y = 0.0f;		BufferData.vPlanes[0].z = 0.0f;
		BufferData.vPlanes[1].x = 0.0f;		BufferData.vPlanes[1].y = 1.0f;		BufferData.vPlanes[1].z = 0.0f;
		BufferData.vPlanes[2].x = -1.0f;	BufferData.vPlanes[2].y = 0.0f;		BufferData.vPlanes[2].z = SimulationMapWidthAsFloat;
		BufferData.vPlanes[3].x = 0.0f;		BufferData.vPlanes[3].y = -1.0f;	BufferData.vPlanes[3].z = SimulationMapHeightAsFloat;

		CreateConstantBuffer<SimulationConstants>(
			pDevice, m_SimulationConstants.GetAddressOf(),
			&BufferData
			);
		CreateConstantBuffer<SortConstants>(
			pDevice, m_SortConstants.GetAddressOf()
			);

		SetDebugName( m_SimulationConstants.Get(), "FluidSimulatorConstantsCBuffer" );
		SetDebugName( m_SortConstants.Get(), "FluidParticleSorterConstantsCBuffer" );
	}
}

void Demo::FluidSimulator::CreateShaders( Demo::FluidSimulator::DevicePtr pDevice )
{
#ifdef REIGN_DESKTOP
	std::unique_ptr<Sehnsucht::IO::BasicLoader> Loader(
		new Sehnsucht::IO::BasicLoader(pDevice)
		);
#else
	Sehnsucht::IO::BasicLoader^ Loader = 
		ref new Sehnsucht::IO::BasicLoader(
			pDevice
			);
#endif

	Loader->LoadShader( 
		L"FluidSimulatorBitonicSort.cso", 
		m_SortBitonicCs.GetAddressOf() 
		);
	Loader->LoadShader( 
		L"FluidSimulatorMatrixTranspose.cso", 
		m_SortTransposeCs.GetAddressOf() 
		);

	Loader->LoadShader( 
		L"FluidSimulatorParticleAcceleration.cso", 
		m_AccelCs.GetAddressOf() 
		);
	Loader->LoadShader( 
		L"FluidSimulatorAccumulateParticles.cso", 
		m_AccumCs.GetAddressOf() 
		);
	Loader->LoadShader( 
		L"FluidSimulatorParticleDensity.cso", 
		m_DensityCs.GetAddressOf() 
		);

	Loader->LoadShader( 
		L"FluidSimulatorBuildParticleGrid.cso", 
		m_BuildGridCs.GetAddressOf() 
		);
	Loader->LoadShader( 
		L"FluidSimulatorBuildParticleGridIndices.cso", 
		m_BuildGridIndicesCs.GetAddressOf() 
		);
	Loader->LoadShader( 
		L"FluidSimulatorClearParticleGridIndices.cso", 
		m_ClearGridIndicesCs.GetAddressOf() 
		);
	Loader->LoadShader( 
		L"FluidSimulatorRearrangeParticles.cso", 
		m_RearrangeParticlesCs.GetAddressOf() 
		);
}

void Demo::FluidSimulator::UpdateParticles( _In_ Demo::FluidSimulator::ContextPtr pContext )
{
	static const UInt32 uUAVNullInitCount = 0u;

	// Set the simulator const buffer
	// cb0 = SimConsts
	pContext->CSSetConstantBuffers( 0, 1, m_SimulationConstants.GetAddressOf() ); // cb0
	
	// Build grid 
	// t0 = ParticlesRO
	// u0 = GridRW

	//PrintToDebugWindowOnly8( "BUILD GRID START" );

	pContext->CSSetUnorderedAccessViews( 0, 1, m_GridRW.GetAddressOf(), &uUAVNullInitCount ); // u0
	pContext->CSSetShaderResources( 0, 1, m_ParticlesRO.GetAddressOf() ); // t0
	pContext->CSSetShader( m_BuildGridCs.Get(), nullptr, 0 );
	pContext->Dispatch( SimulatorNumXThreadGroupsForParticles, 1, 1 );

	//PrintToDebugWindowOnly8( "BUILD GRID END" );
	//PrintToDebugWindowOnly8( "SORT GRID START" );

	// Sort grid 
	SortParticles( pContext );

	//PrintToDebugWindowOnly8( "SORT GRID END" );
	//PrintToDebugWindowOnly8( "CLEAR GRID INDICES START" );

	// Set the simulator const buffer back since it was detached when sorting
	pContext->CSSetConstantBuffers( 0, 1, m_SimulationConstants.GetAddressOf() ); // cb0

	// Clear and then build grid indices 
	// t0 = GridRO
	// u0 = GridIndicesRW

	pContext->CSSetUnorderedAccessViews( 0, 1, m_GridIndicesRW.GetAddressOf(), &uUAVNullInitCount );
	pContext->CSSetShaderResources( 0, 1, m_GridRO.GetAddressOf() );
	pContext->CSSetShader( m_ClearGridIndicesCs.Get(), nullptr, 0 );
	pContext->Dispatch( SimulatorNumXThreadGroupsForGrid, 1, 1 );

	//PrintToDebugWindowOnly8( "CLEAR GRID INDICES END" );
	//PrintToDebugWindowOnly8( "BUILD GRID INDICES START" );

	pContext->CSSetShader( m_BuildGridIndicesCs.Get(), nullptr, 0 );
	pContext->Dispatch( SimulatorNumXThreadGroupsForParticles, 1, 1 );

	//PrintToDebugWindowOnly8( "BUILD GRID INDICES END" );
	//PrintToDebugWindowOnly8( "REARRANGE GRID INDICES START" );


	// Rearrange 
	// t0 = ParticlesRO
	// t1 = GridRO
	// u0 = ParticlesRW

	// Write to sorted particles buffer
	// Read from particles buffer 
	pContext->CSSetUnorderedAccessViews( 0, 1, m_SortedParticlesRW.GetAddressOf(), &uUAVNullInitCount );
	pContext->CSSetShaderResources( 0, 1, m_ParticlesRO.GetAddressOf() );
	pContext->CSSetShaderResources( 1, 1, m_GridRO.GetAddressOf() );
	pContext->CSSetShader( m_RearrangeParticlesCs.Get(), nullptr, 0 );
	pContext->Dispatch( SimulatorNumXThreadGroupsForParticles, 1, 1 );

	//PrintToDebugWindowOnly8( "REARRANGE GRID INDICES END" );
	//PrintToDebugWindowOnly8( "DENSITY START" );


	// Update fluid density 
	// t0 = ParticlesRO
	// t1 = GridIndicesRO
	// u0 = ParticlesDensityRW

	pContext->CSSetUnorderedAccessViews( 0, 1, m_ParticlesDensityRW.GetAddressOf(), &uUAVNullInitCount );
	pContext->CSSetShaderResources( 0, 1, m_SortedParticlesRO.GetAddressOf() );
	pContext->CSSetShaderResources( 1, 1, m_GridIndicesRO.GetAddressOf() );
	pContext->CSSetShader( m_DensityCs.Get(), nullptr, 0 );
	pContext->Dispatch( SimulatorNumXThreadGroupsForParticles, 1, 1 );

	//PrintToDebugWindowOnly8( "DENSITY END" );
	//PrintToDebugWindowOnly8( "ACCELERATION START" );


	// Update particles` acceleration 
	// t0 = ParticlesRO
	// t1 = ParticlesDensityRO
	// u0 = ParticlesAccelerationRW

	pContext->CSSetUnorderedAccessViews( 0, 1, m_ParticlesAccelRW.GetAddressOf(), &uUAVNullInitCount );
	pContext->CSSetShaderResources( 0, 1, m_SortedParticlesRO.GetAddressOf() );
	pContext->CSSetShaderResources( 1, 1, m_ParticlesDensityRO.GetAddressOf() );
	pContext->CSSetShaderResources( 2, 1, m_GridIndicesRO.GetAddressOf() );
	pContext->CSSetShader( m_AccelCs.Get(), nullptr, 0 );
	pContext->Dispatch( SimulatorNumXThreadGroupsForParticles, 1, 1 );

	//PrintToDebugWindowOnly8( "ACCELERATION END" );
	//PrintToDebugWindowOnly8( "ACCUMULATION START" );


	// Update particles
	// t0 = ParticlesRO
	// t1 = ParticlesAccelerationRO
	// u0 = ParticlesRW

	// Write to particles buffer 
	// Read from sorted particle buffer 
	pContext->CSSetUnorderedAccessViews( 0, 1, m_ParticlesRW.GetAddressOf(), &uUAVNullInitCount );
	pContext->CSSetShaderResources( 0, 1, m_SortedParticlesRO.GetAddressOf() );
	pContext->CSSetShaderResources( 1, 1, m_ParticlesAccelRO.GetAddressOf() );
	pContext->CSSetShader( m_AccumCs.Get(), nullptr, 0 );
	pContext->Dispatch( SimulatorNumXThreadGroupsForParticles, 1, 1 );

	//PrintToDebugWindowOnly8( "ACCUMULATION END" );
}

void Demo::FluidSimulator::SortParticles( _In_ Demo::FluidSimulator::ContextPtr pContext )
{
	static const UInt32 uUAVNullInitCount = 0u;
	static const SRViewPtr pSRVNull = nullptr;

	pContext->CSSetConstantBuffers( 0, 1, m_SortConstants.GetAddressOf() );

	for ( auto uLevel = 2u; uLevel <= SortBlockSize; uLevel <<= 1 )
	{
		SortConstants SortData = { 0 };
		SortData.iLevel = uLevel; 
		SortData.iLevelMask = uLevel;
		SortData.iWidth = SortMatrixHeight; 
		SortData.iHeight = SortMatrixWidth;
		pContext->UpdateSubresource( 
			m_SortConstants.Get(), 0u, 
			nullptr, &SortData, 0u, 0u 
			);

		pContext->CSSetUnorderedAccessViews( 0, 1, m_GridRW.GetAddressOf(), &uUAVNullInitCount );
		pContext->CSSetShaderResources( 0, 1, m_GridTempRO.GetAddressOf() );
		pContext->CSSetShader( m_SortBitonicCs.Get(), nullptr, 0u );
		pContext->Dispatch( SortMatrixHeight, 1, 1 );
	}

	for ( auto uLevel = (SortMatrixWidth << 1); uLevel <= SortMatrixNumElements; uLevel <<= 1 )
	{
		SortConstants SortData;
		SortData.iLevel = (uLevel / SortBlockSize); 
		SortData.iLevelMask = (uLevel & ~SortMatrixNumElements) / SortBlockSize;
		SortData.iWidth = SortMatrixWidth; 
		SortData.iHeight = SortMatrixHeight;
		pContext->UpdateSubresource( 
			m_SortConstants.Get(), 0u, 
			nullptr, &SortData, 0u, 0u 
			);

		pContext->CSSetShaderResources( 0, 1, &pSRVNull );
		pContext->CSSetUnorderedAccessViews( 0, 1, m_GridTempRW.GetAddressOf(), &uUAVNullInitCount ); 
		pContext->CSSetShaderResources( 0, 1, m_GridRO.GetAddressOf() );
		pContext->CSSetShader( m_SortTransposeCs.Get(), nullptr, 0 );
		pContext->Dispatch( SortMatrixWidth / TransposeBlockSize, SortMatrixHeight / TransposeBlockSize, 1 );
		pContext->CSSetShader( m_SortBitonicCs.Get(), nullptr, 0 );
		pContext->Dispatch( SortMatrixHeight, 1, 1 );

		Reign::Standard::ZeroValue( &SortData );
		SortData.iLevel = SortBlockSize; 
		SortData.iLevelMask = uLevel;
		SortData.iWidth = SortMatrixHeight; 
		SortData.iHeight = SortMatrixWidth;
		pContext->UpdateSubresource( 
			m_SortConstants.Get(), 0u, nullptr, 
			&SortData, 0u, 0u 
			);

		pContext->CSSetShaderResources( 0, 1, &pSRVNull );
		pContext->CSSetUnorderedAccessViews( 0, 1, m_GridRW.GetAddressOf(), &uUAVNullInitCount ); 
		pContext->CSSetShaderResources( 0, 1, m_GridTempRO.GetAddressOf() );
		pContext->CSSetShader( m_SortTransposeCs.Get(), nullptr, 0 );
		pContext->Dispatch( SortMatrixHeight / TransposeBlockSize, SortMatrixWidth / TransposeBlockSize, 1 );
		pContext->CSSetShader( m_SortBitonicCs.Get(), nullptr, 0 );
		pContext->Dispatch( SortMatrixHeight, 1, 1 );
	}
}

void Demo::FluidSimulator::OnFrameMove( Demo::FluidSimulator::ContextPtr pContext )
{
	static const UInt32 uUAVNullInitCount = 0u;
	static const SRViewPtr pSRVNull = nullptr;
	static const UAViewPtr pUAVNull = nullptr;

	// Update

	UpdateParticles(pContext);

	// Clean up

	pContext->CSSetUnorderedAccessViews( 0, 1, &pUAVNull, &uUAVNullInitCount );
	pContext->CSSetUnorderedAccessViews( 1, 1, &pUAVNull, &uUAVNullInitCount );
	pContext->CSSetShaderResources( 0, 1, &pSRVNull );
	pContext->CSSetShaderResources( 1, 1, &pSRVNull );
	pContext->CSSetShaderResources( 2, 1, &pSRVNull );
	pContext->CSSetShader( nullptr, nullptr, 0u );
}

void Demo::FluidSimulator::OnFrame( Demo::FluidSimulator::ContextPtr pContext )
{
}


