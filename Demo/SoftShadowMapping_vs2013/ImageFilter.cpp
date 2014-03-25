#include "ImageFilter.h"
#include "Engine.h"

#include <Nena\AppVars.h>
#include <Nena\PlatformAssist.h>

Demo::ImageFilter *Demo::ImageFilter::CreateFilterFromVars()
{
	auto vars = Nena::App::Vars::GetForCurrentThread();
	if (!vars->GetRecordsCount())
	{
		vars->Read(TRUE, "app.vars.xml");
#if _DEBUG
		vars->Read(TRUE, "app.vars.overrides.xml");
#endif
	}

	if (vars->HasRecord("Demo/ShadowMapping/Shader/Depth/Filter"))
	{
		auto filterName = vars->GetRecord("Demo/ShadowMapping/Shader/Depth/Filter");
		if (filterName == "SeparableBilateral") return new SeparableBilateral();
		else if (filterName == "SeparableGaussian") return new SeparableGaussian();
		else if (filterName == "CachedGaussian") return new CachedGaussian();
		else Nena::Platform::DebugTrace("Cannot find depth filter %s\n", filterName.c_str());
	}

	return nullptr;
}

void Demo::ImageFilter::CreateDeviceResources(
	::Nena::Graphics::Resources::ITexture2 *source,
	std::wstring procx,	std::wstring procy
	)
{
	using namespace ::Nena::Graphics::Resources;

	if (!source) return;


	auto engine = Demo::Engine::GetForCurrentThread();
	D3DDvc = engine->D3D.Device.Get();
	D3DCtx = engine->D3D.Context.Get();

	Texture2Description desc; source->GetDesc(&desc);
	SRVDescription srvDesc; ZeroValue(&srvDesc);
	UAVDescription uavDesc; ZeroValue(&uavDesc);

	_ASSERT_EXPR(desc.Width == SizeX && desc.Height == SizeY, L"Image dimensions mismatch");

	srvDesc.Format = desc.Format;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	uavDesc.Format = desc.Format;
	uavDesc.Texture2D.MipSlice = 0;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	
	HRESULT result = S_OK;

	result = engine->D3D.Device->CreateTexture2D(&desc, nullptr, Copy.Rsrc.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create copy image texture");
	result = engine->D3D.Device->CreateShaderResourceView(Copy.Rsrc.Get(), &srvDesc, Copy.Read.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create copy image texture read view");
	result = engine->D3D.Device->CreateUnorderedAccessView(Copy.Rsrc.Get(), &uavDesc, Copy.Write.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create copy image texture write view");

	if (procx.size())
	{
		result = Assist::LoadShader(engine->D3D.Device.Get(), procx, ProcX.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create image horizontal processor");
	}
	if (procy.size())
	{
		result = Assist::LoadShader(engine->D3D.Device.Get(), procy, ProcY.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create image vertical processor");
	}

}

void Demo::SeparableGaussian::CreateDeviceResources(
	::Nena::Graphics::Resources::ITexture2 *source
	)
{
	using namespace ::Nena::Graphics::Resources;
	auto vars = Nena::App::Vars::GetForCurrentThread();

	if (!vars->GetRecordsCount()) 
	{ 
		vars->Read(TRUE, "app.vars.xml"); 
#if _DEBUG
		vars->Read(TRUE, "app.vars.overrides.xml");
#endif
	}

	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Shader/Filter/SeparableGaussian/CS/X/CSO"), 
		L"Cannot find image horizontal processor file in vars");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Shader/Filter/SeparableGaussian/CS/Y/CSO"),
		L"Cannot find image horizontal processor file in vars");

	ImageFilter::CreateDeviceResources(source,
		vars->GetRecord<std::wstring>("Demo/ShadowMapping/Shader/Filter/SeparableGaussian/CS/X/CSO"),
		vars->GetRecord<std::wstring>("Demo/ShadowMapping/Shader/Filter/SeparableGaussian/CS/Y/CSO")
		);
}

void Demo::SeparableBilateral::CreateDeviceResources(
	::Nena::Graphics::Resources::ITexture2 *source
	)
{
	using namespace ::Nena::Graphics::Resources;
	auto vars = Nena::App::Vars::GetForCurrentThread();

	if (!vars->GetRecordsCount())
	{
		vars->Read(TRUE, "app.vars.xml");
#if _DEBUG
		vars->Read(TRUE, "app.vars.overrides.xml");
#endif
	}

	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Shader/Filter/SeparableBilateral/CS/X/CSO"),
		L"Cannot find image horizontal processor file in vars");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Shader/Filter/SeparableBilateral/CS/Y/CSO"),
		L"Cannot find image horizontal processor file in vars");

	ImageFilter::CreateDeviceResources(source,
		vars->GetRecord<std::wstring>("Demo/ShadowMapping/Shader/Filter/SeparableBilateral/CS/X/CSO"),
		vars->GetRecord<std::wstring>("Demo/ShadowMapping/Shader/Filter/SeparableBilateral/CS/Y/CSO")
		);

}

void Demo::CachedGaussian::CreateDeviceResources(
	::Nena::Graphics::Resources::ITexture2 *source
	)
{
	using namespace ::Nena::Graphics::Resources;
	auto vars = Nena::App::Vars::GetForCurrentThread();

	if (!vars->GetRecordsCount())
	{
		vars->Read(TRUE, "app.vars.xml");
#if _DEBUG
		vars->Read(TRUE, "app.vars.overrides.xml");
#endif
	}

	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Shader/Filter/CachedGaussian/CS/X/CSO"),
		L"Cannot find image horizontal processor file in vars");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Shader/Filter/CachedGaussian/CS/Y/CSO"),
		L"Cannot find image horizontal processor file in vars");

	ImageFilter::CreateDeviceResources(source,
		vars->GetRecord<std::wstring>("Demo/ShadowMapping/Shader/Filter/CachedGaussian/CS/X/CSO"),
		vars->GetRecord<std::wstring>("Demo/ShadowMapping/Shader/Filter/CachedGaussian/CS/Y/CSO")
		);
}

void Demo::SeparableBilateral::Render(
	_Inout_::Nena::Graphics::Resources::ISRView *srv,
	_Inout_::Nena::Graphics::Resources::IUAView *uav
	)
{
	static UINT32 init[] = { 0, 0 };
	static ::Nena::Graphics::Resources::ISRView *srvnulls[] = { nullptr, nullptr };
	static ::Nena::Graphics::Resources::IUAView *uavnulls[] = { nullptr, nullptr };

	D3DCtx->CSSetShaderResources(0, 1, &srv);
	D3DCtx->CSSetUnorderedAccessViews(0, 1, Copy.Write.GetAddressOf(), init);
	D3DCtx->CSSetShader(ProcX.Get(), nullptr, 0);
	D3DCtx->Dispatch(1, SizeY, 1);

	D3DCtx->CSSetUnorderedAccessViews(0, 1, uavnulls, init);
	D3DCtx->CSSetShaderResources(0, 1, srvnulls);

	D3DCtx->CSSetShaderResources(0, 1, Copy.Read.GetAddressOf());
	D3DCtx->CSSetUnorderedAccessViews(0, 1, &uav, init);
	D3DCtx->CSSetShader(ProcY.Get(), nullptr, 0);
	D3DCtx->Dispatch(SizeX, 1, 1);

	D3DCtx->CSSetUnorderedAccessViews(0, 1, uavnulls, init);
	D3DCtx->CSSetShaderResources(0, 1, srvnulls);

	D3DCtx->CSSetShader(nullptr, nullptr, 0);
}

void Demo::SeparableGaussian::Render(
	_Inout_::Nena::Graphics::Resources::ISRView *srv,
	_Inout_::Nena::Graphics::Resources::IUAView *uav
	)
{
	static UINT32 init[] = { 0, 0 };
	static ::Nena::Graphics::Resources::ISRView *srvnulls[] = { nullptr, nullptr };
	static ::Nena::Graphics::Resources::IUAView *uavnulls[] = { nullptr, nullptr };

	D3DCtx->CSSetShaderResources(0, 1, &srv);
	D3DCtx->CSSetUnorderedAccessViews(0, 1, Copy.Write.GetAddressOf(), init);
	D3DCtx->CSSetShader(ProcX.Get(), nullptr, 0);
	D3DCtx->Dispatch(1, SizeY, 1);

	D3DCtx->CSSetUnorderedAccessViews(0, 1, uavnulls, init);
	D3DCtx->CSSetShaderResources(0, 1, srvnulls);

	D3DCtx->CSSetShaderResources(0, 1, Copy.Read.GetAddressOf());
	D3DCtx->CSSetUnorderedAccessViews(0, 1, &uav, init);
	D3DCtx->CSSetShader(ProcY.Get(), nullptr, 0);
	D3DCtx->Dispatch(SizeX, 1, 1);

	D3DCtx->CSSetUnorderedAccessViews(0, 1, uavnulls, init);
	D3DCtx->CSSetShaderResources(0, 1, srvnulls);

	D3DCtx->CSSetShader(nullptr, nullptr, 0);

}

void Demo::CachedGaussian::Render(
	_Inout_::Nena::Graphics::Resources::ISRView *srv,
	_Inout_::Nena::Graphics::Resources::IUAView *uav
	)
{
	static UINT32 init[] = { 0, 0 };
	static ::Nena::Graphics::Resources::ISRView *srvnulls[] = { nullptr, nullptr };
	static ::Nena::Graphics::Resources::IUAView *uavnulls[] = { nullptr, nullptr };

	D3DCtx->CSSetShaderResources(0, 1, &srv);
	D3DCtx->CSSetUnorderedAccessViews(0, 1, Copy.Write.GetAddressOf(), init);
	D3DCtx->CSSetShader(ProcX.Get(), nullptr, 0);
	D3DCtx->Dispatch(1, SizeY, 1);

	D3DCtx->CSSetUnorderedAccessViews(0, 1, uavnulls, init);
	D3DCtx->CSSetShaderResources(0, 1, srvnulls);

	D3DCtx->CSSetShaderResources(0, 1, Copy.Read.GetAddressOf());
	D3DCtx->CSSetUnorderedAccessViews(0, 1, &uav, init);
	D3DCtx->CSSetShader(ProcY.Get(), nullptr, 0);
	D3DCtx->Dispatch(SizeX, 1, 1);

	D3DCtx->CSSetUnorderedAccessViews(0, 1, uavnulls, init);
	D3DCtx->CSSetShaderResources(0, 1, srvnulls);

	D3DCtx->CSSetShader(nullptr, nullptr, 0);
}

