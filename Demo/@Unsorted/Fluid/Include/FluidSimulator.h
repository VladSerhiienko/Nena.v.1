#pragma once
#include "Precompiled.h"

/// Those value must match the ones in the shaders !

#define FuildSimulator16BitAlignment			REIGN_ALIGN(16)
#define TransposeBlockSize						(16) // Numthreads for transpose 
#define SortBlockSize							(512) // Numthreads for sort 
#define SimulatorNumParticles					(32 * 1024)		// 32768
#define SimulatorBlockSize						(256)			// Numthreads size for simulation 
#define SimulatorNumGridIndices					(256 * 256)		// 256 x 256 = 65536 = 32768 * 2
#define SimulatorNumXThreadGroupsForGrid		(SimulatorNumGridIndices / SimulatorBlockSize) // 256
#define SimulatorNumXThreadGroupsForParticles	(SimulatorNumParticles / SimulatorBlockSize) // 256 / 2 = 128
#define SimulatorInitialParticleSpacingAsFloat	(0.0045f)
#define SimulatorPressureStiffnessAsFloat		(200.0f)
#define SimulatorWallStiffnessAsFloat			(3000.0f)
#define SimulatorSmoothLengthAsFloat			(0.012f)
#define SimulatorParticleMassAsFloat			(0.0001f)
#define SimulatorParticleSizeAsFloat			(0.0023f)
#define SimulatorRestDensityAsFloat				(1000.0f)
#define SimulatorMaxTimeStepAsFloat				(0.008f)
#define SimulatorViscosityAsFloat				(0.1877f)
#define SimulatorGravityXAsFloat				(0.0f)
#define SimulatorGravityYAsFloat				(-0.5f)
#define SimulationMapHeightAsFloat				(1.2f)
#define SimulationMapWidthAsFloat				((4.0f / 3.0f) * SimulationMapHeightAsFloat)
#define SortMatrixNumElements					(SimulatorNumParticles) // 32K
#define SortMatrixWidth							(SortBlockSize) // 512
#define SortMatrixHeight						(SortMatrixNumElements / SortBlockSize) // 32K / 512 = 64

namespace Demo
{
	class FluidSimulator
	{
	public:

#pragma region Internal simulator types

		typedef Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UAView;
		typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRView;
		typedef Microsoft::WRL::ComPtr<ID3D11ComputeShader> CShader;
		typedef Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
		typedef CShader::InterfaceType *CShaderPtr;
		typedef UAView::InterfaceType *UAViewPtr;
		typedef SRView::InterfaceType *SRViewPtr;
		typedef Buffer::InterfaceType *BufferPtr;
		typedef ID3D11DeviceContext1 *ContextPtr;
		typedef ID3D11Device1 *DevicePtr;

		typedef DirectX::XMFLOAT4 DirectXFloat32x4;
		typedef DirectX::XMFLOAT3 DirectXFloat32x3;
		typedef DirectX::XMFLOAT2 DirectXFloat32x2;
		typedef Reign::Neon::int32x2_t UInt32x2;

		/*
			uint g_iNumParticles;
			float g_fTimeStep;
			float g_fSmoothlen;
			float g_fPressureStiffness;
			float g_fRestDensity;
			float g_fDensityCoef;
			float g_fGradPressureCoef;
			float g_fLapViscosityCoef;
			float g_fWallStiffness;
			float4 g_vGravity;
			float4 g_vGridDim;
			float3 g_vPlanes[4];
		*/

		FuildSimulator16BitAlignment 
		struct SimulationConstants
		{
			UInt32 iNumParticles;
			Float32 fTimeStep;
			Float32 fSmoothlen;
			Float32 fPressureStiffness;
			Float32 fRestDensity;
			Float32 fDensityCoef;
			Float32 fGradPressureCoef;
			Float32 fLapViscosityCoef;
			Float32 fWallStiffness;
			DirectX::XMFLOAT2A vGravity;
			DirectX::XMFLOAT4A vGridDim;
			DirectX::XMFLOAT3A vPlanes[4];
			//DirectXFloat32x4 vPadding;
		};

		FuildSimulator16BitAlignment 
		struct SortConstants
		{
			UInt32 iLevel;
			UInt32 iLevelMask;
			UInt32 iWidth;
			UInt32 iHeight;
		};

		struct Particle
		{
			DirectXFloat32x2 vPosition;
			DirectXFloat32x2 vVelocity;
		};

		struct Acceleration
		{
			DirectXFloat32x2 vAcceleration;
		};

		struct Density
		{
			Float32 fDensity;
		};

#pragma endregion

	public:

		FluidSimulator() { PrintToDebugWindowOnly8("Fluid simulator created"); }
		virtual ~FluidSimulator() { PrintToDebugWindowOnly8("Fluid simulator destroyed"); }

	private:

		FluidSimulator( const FluidSimulator& );
		FluidSimulator &operator=( FluidSimulator const& );

	public:

		void CreateBuffers( _In_ Demo::FluidSimulator::DevicePtr pDevice );
		void CreateShaders( _In_ Demo::FluidSimulator::DevicePtr pDevice );
		void OnFrameMove( _In_ Demo::FluidSimulator::ContextPtr pContext );
		void OnFrame( _In_ Demo::FluidSimulator::ContextPtr pContext );

	private:

		void UpdateParticles( _In_ Demo::FluidSimulator::ContextPtr pContext );
		void SortParticles( _In_ Demo::FluidSimulator::ContextPtr pContext );

	protected:

#pragma region Shared resources

		SRView m_ParticlesRO;
		SRView m_ParticlesDensityRO;

#pragma endregion

	private:
		
#pragma region Containers to store the resources to

		CShader m_SortBitonicCs;
		CShader m_SortTransposeCs;

		CShader m_BuildGridCs;
		CShader m_BuildGridIndicesCs;
		CShader m_ClearGridIndicesCs;
		CShader m_RearrangeParticlesCs;

		CShader m_DensityCs;
		CShader m_AccelCs;
		CShader m_AccumCs;

		UAView m_ParticlesRW;
		Buffer m_Particles;

		UAView m_SortedParticlesRW;
		SRView m_SortedParticlesRO;
		Buffer m_SortedParticles;

		UAView m_ParticlesDensityRW;
		Buffer m_ParticlesDensity;

		UAView m_ParticlesAccelRW;
		SRView m_ParticlesAccelRO;
		Buffer m_ParticlesAccel;

		UAView m_GridIndicesRW;
		SRView m_GridIndicesRO;
		Buffer m_GridIndices;

		UAView m_GridTempRW;
		SRView m_GridTempRO;
		Buffer m_GridTemp;

		UAView m_GridRW;
		SRView m_GridRO;
		Buffer m_Grid;

		Buffer m_SimulationConstants;
		Buffer m_SortConstants;

#pragma endregion 
		
#pragma region Actual shortcuts for fast direct access

		/*CShaderPtr m_SortBitonicCsPtr;
		CShaderPtr m_SortTransposeCsPtr;

		CShaderPtr m_BuildGridCsPtr;
		CShaderPtr m_BuildGridIndicesCsPtr;
		CShaderPtr m_ClearGridIndicesCsPtr;
		CShaderPtr m_RearrangeParticlesCsPtr;

		CShaderPtr m_DensityCsPtr;
		CShaderPtr m_AccelCsPtr;
		CShaderPtr m_AccumCsPtr;

		UAViewPtr m_ParticlesRWPtr;
		SRViewPtr m_ParticlesROPtr;
		BufferPtr m_ParticlesPtr;

		UAViewPtr m_SortedParticlesRWPtr;
		SRViewPtr m_SortedParticlesROPtr;
		BufferPtr m_SortedParticlesPtr;

		UAViewPtr m_ParticlesDensityRWPtr;
		SRViewPtr m_ParticlesDensityROPtr;
		BufferPtr m_ParticlesDensityPtr;

		UAViewPtr m_ParticlesAccelRWPtr;
		SRViewPtr m_ParticlesAccelROPtr;
		BufferPtr m_ParticlesAccelPtr;

		UAViewPtr m_GridIndicesRWPtr;
		SRViewPtr m_GridIndicesROPtr;
		BufferPtr m_GridIndicesPtr;

		UAViewPtr m_GridTempRWPtr;
		SRViewPtr m_GridTempROPtr;
		BufferPtr m_GridTempPtr;

		UAViewPtr m_GridRWPtr;
		SRViewPtr m_GridROPtr;
		BufferPtr m_GridPtr;

		BufferPtr m_SimulationConstantsPtr;
		BufferPtr m_SortConstantsPtr;*/

#pragma endregion 

	};
};

namespace Demo
{
	template <class TBufferContent>
	inline void CreateConstantBuffer(
		_In_ Demo::FluidSimulator::DevicePtr pDevice,
		_Outref_result_nullonfailure_ Demo::FluidSimulator::BufferPtr *ppBuffer,
		_Const_ _Maybenull_ TBufferContent const *pInitialData = nullptr
		)
	{
		D3D11_BUFFER_DESC BufferDescriptor = { 0 };
		BufferDescriptor.ByteWidth = GetByteSizeOf(TBufferContent);
		BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;

		if ( pInitialData )
		{
			D3D11_SUBRESOURCE_DATA BufferInitData = { 0 };
			BufferInitData.pSysMem = pInitialData;

			Sehnsucht::ThrowIfFailed(
				//"FluidSimulator: Failed to create constant buffer.", 
				pDevice->CreateBuffer( 
					&BufferDescriptor, 
					&BufferInitData, 
					ppBuffer 
					)
				);
		}
		else
		{
			Sehnsucht::ThrowIfFailed(
				//"FluidSimulator: Failed to create constant buffer.", 
				pDevice->CreateBuffer( 
					&BufferDescriptor, 
					nullptr, 
					ppBuffer 
					)
				);
		}
	}

	template <class TBufferContent>
	inline void CreateStructuredBuffer(
		_In_ Demo::FluidSimulator::DevicePtr pDevice,
		_In_ Reign::Standard::UInt32 uNumBufferElements,
		_Outref_result_nullonfailure_ Demo::FluidSimulator::BufferPtr *ppBuffer,
		_Outref_result_nullonfailure_ Demo::FluidSimulator::SRViewPtr *ppSRView,
		_Outref_result_nullonfailure_ Demo::FluidSimulator::UAViewPtr *ppUAView,
		_Const_ _Maybenull_  TBufferContent const *pInitialData = nullptr
		)
	{
		D3D11_BUFFER_DESC BufferDescriptor;
		Reign::Standard::ZeroValue( &BufferDescriptor );
		BufferDescriptor.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		BufferDescriptor.ByteWidth = uNumBufferElements * GetByteSizeOf(TBufferContent);
		BufferDescriptor.StructureByteStride = GetByteSizeOf(TBufferContent);
		BufferDescriptor.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;

		if ( pInitialData )
		{

			D3D11_SUBRESOURCE_DATA BufferInitData;
			Reign::Standard::ZeroValue( &BufferInitData );
			BufferInitData.pSysMem = pInitialData;

			Sehnsucht::ThrowIfFailed(
				//"FluidSimulator: Failed to create structured buffer.", 
				pDevice->CreateBuffer( 
					&BufferDescriptor, 
					&BufferInitData, 
					ppBuffer 
					)
				);
		}
		else
		{
			Sehnsucht::ThrowIfFailed(
				//"FluidSimulator: Failed to create structured buffer.", 
				pDevice->CreateBuffer( 
					&BufferDescriptor, 
					nullptr, 
					ppBuffer 
					)
				);
		}

		if ( ppSRView ) 
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC BufferSRViewDesc;
			Reign::Standard::ZeroValue( &BufferSRViewDesc );
			BufferSRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			BufferSRViewDesc.Buffer.ElementWidth = uNumBufferElements;

			Sehnsucht::ThrowIfFailed(
				//"FluidSimulator: Failed to create structured buffer srv.", 
				pDevice->CreateShaderResourceView( 
					(*ppBuffer), (&BufferSRViewDesc), 
					(ppSRView)
					)
				);
		}

		if ( ppUAView )
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC BufferUAViewDesc;
			Reign::Standard::ZeroValue( &BufferUAViewDesc );
			BufferUAViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			BufferUAViewDesc.Buffer.NumElements = uNumBufferElements;
		
			Sehnsucht::ThrowIfFailed(
				//"FluidSimulator: Failed to create structured buffer uav.", 
				pDevice->CreateUnorderedAccessView( 
					(*ppBuffer), (&BufferUAViewDesc), 
					(ppUAView)
					)
				);
		}
	}
}