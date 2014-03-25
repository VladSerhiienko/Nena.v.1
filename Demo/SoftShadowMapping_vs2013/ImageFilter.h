

#include <Nena\Math.h>
#include <Nena\Assist.h>
#include <Nena\DirectXTypes.h>
#include <Nena\PlatformAssist.h>

#ifndef __RASTERTEK_DEMO_IMAGEFILTER_INCLUDED__
#define __RASTERTEK_DEMO_IMAGEFILTER_INCLUDED__

namespace Demo
{
	struct ImageFilter
	{
		typedef ::Nena::Graphics::Resources::ActiveResource<::Nena::Graphics::Resources::Texture2> Texture2DObject;

		static const UINT32 SizeG = 32;
		static const UINT32 ThreadsX = 20, ThreadsY = 15;
		static const UINT32 SizeX = SizeG * ThreadsX, SizeY = SizeG * ThreadsY;

		::Demo::ImageFilter::Texture2DObject Copy;
		::Nena::Graphics::Resources::CShader ProcX;
		::Nena::Graphics::Resources::CShader ProcY;
		::Nena::Graphics::Resources::IDirect3DDevice *D3DDvc;
		::Nena::Graphics::Resources::IDirect3DContext *D3DCtx;

		virtual void CreateDeviceResources(
			_In_::Nena::Graphics::Resources::ITexture2 *source
			) PURE;
		virtual void Render(
			_Inout_::Nena::Graphics::Resources::ISRView *srv,
			_Inout_::Nena::Graphics::Resources::IUAView *uav
			) PURE;

		static ImageFilter *CreateFilterFromVars();

	protected:

		void CreateDeviceResources(
			Nena::Graphics::Resources::ITexture2 *source,
			std::wstring procx, std::wstring procy
			);

		ImageFilter() {}

	};

	struct SeparableGaussian : public ImageFilter
	{
		virtual void CreateDeviceResources(
			_In_ ::Nena::Graphics::Resources::ITexture2 *source
			) override;
		virtual void Render(
			_Inout_::Nena::Graphics::Resources::ISRView *srv,
			_Inout_::Nena::Graphics::Resources::IUAView *uav
			) override;
	};

	struct SeparableBilateral : public ImageFilter
	{
		virtual void CreateDeviceResources(
			_In_::Nena::Graphics::Resources::ITexture2 *source
			) override;
		virtual void Render(
			_Inout_::Nena::Graphics::Resources::ISRView *srv,
			_Inout_::Nena::Graphics::Resources::IUAView *uav
			) override;
	};

	struct CachedGaussian : public ImageFilter
	{
		virtual void CreateDeviceResources(
			_In_::Nena::Graphics::Resources::ITexture2 *source
			) override;
		virtual void Render(
			_Inout_::Nena::Graphics::Resources::ISRView *srv,
			_Inout_::Nena::Graphics::Resources::IUAView *uav
			) override;
	};
}

#endif // !__RASTERTEK_DEMO_IMAGEFILTER_INCLUDED__
