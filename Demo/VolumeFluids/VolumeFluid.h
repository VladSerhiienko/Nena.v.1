#pragma once

#include "Camera.h"
#include <Nena\Assist.h>
#include <Nena\PlatformAssist.h>
#include <Nena\DeviceResources.h>

#define __Decl_aligned(_Cls, _Algn) typedef _declspec(align(_Algn)) _Cls : public Nena::Platform::AlignedNew<_Cls>

namespace Demo
{
	using namespace ::Nena::Graphics;

	struct VolumeFluid
	{
		typedef struct INT32x2 
		{
			union 
			{ 
				struct { ::INT32 x, y; }; 
				struct { ::INT32 w, h; }; 
			};

			INT32x2(::INT32 x = 0) { w = x; h = x; }
			INT32x2(::INT32 x, ::INT32 y) { w = x; h = y; }

			operator ::Nena::Vector2()
			{
				::Nena::Vector2 result;
				result.x = (::FLOAT) x;
				result.y = (::FLOAT) y;
				return result;
			}

			INT32x2 operator-(INT32x2 const &other)
			{
				INT32x2 sub;
				sub.x = x - other.x;
				sub.y = y - other.y;
				return sub;
			}
			INT32x2 operator+(INT32x2 const &other)
			{
				INT32x2 add;
				add.x = x + other.x;
				add.y = y + other.y;
				return add;
			}
			INT32x2 operator-=(INT32x2 const &other)
			{
				INT32x2 sub;
				sub.x = x - other.x;
				sub.y = y - other.y;
				return sub;
			}
			INT32x2 operator+=(INT32x2 const &other)
			{
				INT32x2 add;
				add.x = x + other.x;
				add.y = y + other.y;
				return add;
			}
		} Int32x2;
		typedef struct INT32x3 : public INT32x2 
		{ 
			union { ::UINT32 z, d; };

			INT32x3(::INT32 x = 0) { w = x; h = x; d = x; }
			INT32x3(::INT32 x, ::INT32 y) { w = x; h = y; d = 0; }
			INT32x3(::INT32 x, ::INT32 y, ::INT32 z) { w = x; h = y; d = z; }

			operator ::Nena::Vector3()
			{
				::Nena::Vector3 result;
				result.x = (::FLOAT) x;
				result.y = (::FLOAT) y;
				result.z = (::FLOAT) z;
				return result;
			}

			INT32x3 operator-(INT32x3 const &other)
			{
				INT32x3 sub;
				sub.x = x - other.x;
				sub.y = y - other.y;
				sub.z = z - other.z;
				return sub;
			}
			INT32x3 operator+(INT32x3 const &other)
			{
				INT32x3 add;
				add.x = x + other.x;
				add.y = y + other.y;
				add.z = z + other.z;
				return add;
			}
			INT32x3 operator-=(INT32x3 const &other)
			{
				INT32x3 sub;
				sub.x = x - other.x;
				sub.y = y - other.y;
				sub.z = z - other.z;
				return sub;
			}
			INT32x3 operator+=(INT32x3 const &other)
			{
				INT32x3 add;
				add.x = x + other.x;
				add.y = y + other.y;
				add.z = z + other.z;
				return add;
			}

		} Int32x3;

		typedef ::Nena::Matrix float4x4;
		typedef ::Nena::Vector4 float4;

		__Decl_aligned(struct PerFrame, 16)
		{
			float4 VolumeDims; // xyz
			float4 ViewportDims; // xy
			float4 Mouse; // xy
			float4 Drag; // xy
			float4 ForceImpact; // x
			float4x4 EyeTransform; // [4][4]
			float4 FocusZoom; // x

		} PerFrame;

		typedef Resources::ConstantBuffer<VolumeFluid::PerFrame> PerFrameResources;
		typedef Resources::ActiveResource<Resources::Texture3> ActiveTexture3;

		Camera *Viewer = nullptr;
		DeviceResources *D3DResources;

		::HRESULT CreateDeviceDependentResources();
		::HRESULT CreateDeviceIndependentResources();
		::HRESULT CreateWindowSizeDependentResources();

		void Update();
		void Render();
		void OnMouseWheel(INT16 mouseWheel);
		void OnMouseMoved(::Nena::Vector2 mousePos);
		void OnMouseRPressed(::Nena::Vector2 mousePos);
		void OnMouseLPressed(::Nena::Vector2 mousePos);
		void OnMouseRReleased(::Nena::Vector2 mousePos);
		void OnMouseLReleased(::Nena::Vector2 mousePos);
		void SetVolumeSize(INT32x3 volumeSize);
		void GetVolumeSize(INT32x3 &volumeSize);
		void SetImpactRadius(::FLOAT radius);

		VolumeFluid();

	private:

		VolumeFluid(const VolumeFluid&);
		VolumeFluid operator=(const VolumeFluid&);

	private:

		::HRESULT CreateShaders();
		::HRESULT CreateBuffers();
		::HRESULT CreateTextures();

		void Update_Advect();
		void Update_ApplyForces();
		void Update_Divergence();
		void Update_Pressure();
		void Update_PressureIteration();
		void Update_Project();
		void Render_Volume();
		void SwapSpeeds();

		::BOOL m_mbRPressed = FALSE;
		::BOOL m_mbLPressed = FALSE;
		::Nena::Vector2 m_prevMousePos;
		VolumeFluid::INT32x3 m_actualVolumeSize;
		VolumeFluid::INT32x3 m_actualRenderSize;

		Resources::UAView m_targetUAV;
		Resources::SamplerState m_linearSampler;
		VolumeFluid::PerFrame m_perFrameData;
		VolumeFluid::PerFrameResources m_perFrameBff;
		VolumeFluid::ActiveTexture3 m_speedTxs[3];
		VolumeFluid::ActiveTexture3 m_pressureTxs[2];
		VolumeFluid::ActiveTexture3 m_divergenceTx;

		static const ::UINT32 s_volumeThreadsX = 4;
		static const ::UINT32 s_volumeThreadsY = 4;
		static const ::UINT32 s_volumeThreadsZ = 4;
		static const ::UINT32 s_renderThreadsX = 8;
		static const ::UINT32 s_renderThreadsY = 8;
		static const ::UINT32 s_renderThreadsz = 1;

		Resources::CShader m_advectCS;
		Resources::CShader m_advectRCS;
		Resources::CShader m_advectMCS;
		Resources::CShader m_divergenceCS;
		Resources::CShader m_applyForcesCS;
		Resources::CShader m_pressureCS;
		Resources::CShader m_projectCS;
		Resources::CShader m_renderCS;

		static const ::UINT32 s_maxSlotsToUnbind = 3;
		static UINT32 s_nullUAVCounts[s_maxSlotsToUnbind];
		static Resources::ISRView *s_nullSRVs[s_maxSlotsToUnbind];
		static Resources::IUAView *s_nullUAVs[s_maxSlotsToUnbind];

		void UnbindViews(
			::UINT32 slotsSRV = s_maxSlotsToUnbind, 
			::UINT32 slotsUAV = s_maxSlotsToUnbind
			);

	};
}
