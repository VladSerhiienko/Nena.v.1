#pragma once
#include "Precompiled.h"
#include "FluidSimulator.h"

namespace Demo
{
	ref class FluidRenderer : 
		protected Sehnsucht::DirectXBase,
		protected Demo::FluidSimulator
	{
		typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTView;
		typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSView;
		typedef Microsoft::WRL::ComPtr<ID3D11GeometryShader> GShader;
		typedef Microsoft::WRL::ComPtr<ID3D11VertexShader> VShader;
		typedef Microsoft::WRL::ComPtr<ID3D11PixelShader> PShader;
		typedef GShader::InterfaceType *GShaderPtr;
		typedef VShader::InterfaceType *VShaderPtr;
		typedef PShader::InterfaceType *PShaderPtr;
		typedef RTView::InterfaceType *RTViewPtr;
		typedef DSView::InterfaceType *DSViewPtr;
		typedef DirectX::XMFLOAT4X4A DirectXFloat32x4x4;

		FuildSimulator16BitAlignment
		struct RenderConstants
		{
			DirectXFloat32x4x4 mViewProj;
			Float32 fParticleSize;
		};

	internal:

		FluidRenderer() : DirectXBase(), FluidSimulator() { }

		using Sehnsucht::DirectXBase::UpdateForWindowSizeChange;
		using Sehnsucht::DirectXBase::CreateWindowSizeDependentResources;
		using Sehnsucht::DirectXBase::CreateDeviceIndependentResources;
		using Sehnsucht::DirectXBase::CreateDeviceResources;
		using Sehnsucht::DirectXBase::HandleDeviceLost;
		using Sehnsucht::DirectXBase::Initialize;

		void CreateBuffers( );
		void CreateShaders( );
		void OnFrameMove(  );
		void OnFrame( );

	private:

#pragma region Containers to store the resources to

		Buffer m_RenderConstants;
		VShader m_Vs;
		GShader m_Gs;
		PShader m_Ps;

#pragma endregion 

#pragma region Actual shortcuts for fast direct access

		/*BufferPtr m_RenderConstantsPtr;
		VShaderPtr m_VsPtr;
		GShaderPtr m_GsPtr;
		PShaderPtr m_PsPtr;*/

#pragma endregion 

	};
};