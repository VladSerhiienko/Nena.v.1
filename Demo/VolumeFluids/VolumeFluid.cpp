#include "VolumeFluid.h"

::UINT32 Demo::VolumeFluid::s_nullUAVCounts[s_maxSlotsToUnbind] = { 0u };
::Nena::Graphics::Resources::ISRView *Demo::VolumeFluid::s_nullSRVs[s_maxSlotsToUnbind] = { nullptr };
::Nena::Graphics::Resources::IUAView *Demo::VolumeFluid::s_nullUAVs[s_maxSlotsToUnbind] = { nullptr };

#define _shaderArgs nullptr, 0u
#define _unbind1x1 UnbindViews(1, 1)
#define _unbind2x1 UnbindViews(2, 1)
#define _unbind3x1 UnbindViews(3, 1)
#define _unbind _unbind3x1

#define _d3ddvc D3DResources->Device.Get()
#define _d3dctx D3DResources->Context.Get()

#define _speed0texs m_speedTxs[0].Rsrc.GetAddressOf()
#define _speed1texs m_speedTxs[1].Rsrc.GetAddressOf()
#define _speed2texs m_speedTxs[2].Rsrc.GetAddressOf()

#define _speed0srvs m_speedTxs[0].Read.GetAddressOf()
#define _speed1srvs m_speedTxs[1].Read.GetAddressOf()
#define _speed2srvs m_speedTxs[2].Read.GetAddressOf()

#define _speed0uavs m_speedTxs[0].Write.GetAddressOf(), s_nullUAVCounts
#define _speed1uavs m_speedTxs[1].Write.GetAddressOf(), s_nullUAVCounts
#define _speed2uavs m_speedTxs[2].Write.GetAddressOf(), s_nullUAVCounts

#define _divsrvs m_divergenceTx.Read.GetAddressOf()
#define _divuavs m_divergenceTx.Write.GetAddressOf(), s_nullUAVCounts

#define _pressure0srvs m_pressureTxs[0].Read.GetAddressOf()
#define _pressure1srvs m_pressureTxs[1].Read.GetAddressOf()

#define _pressure0uavs m_pressureTxs[0].Write.GetAddressOf(), s_nullUAVCounts
#define _pressure1uavs m_pressureTxs[1].Write.GetAddressOf(), s_nullUAVCounts

#define _renderOutput m_targetUAV.GetAddressOf(), s_nullUAVCounts
#define _volumeThreadGroups m_actualVolumeSize.x, m_actualVolumeSize.y, m_actualVolumeSize.z
#define _renderThreadGroups m_actualRenderSize.x, m_actualRenderSize.y, m_actualRenderSize.z

void Demo::VolumeFluid::SwapSpeeds()
{
	using namespace ::Nena::Graphics::Resources;

	m_speedTxs[1].Read.Swap(m_speedTxs[0].Read);
	m_speedTxs[1].Write.Swap(m_speedTxs[0].Write);
}

void Demo::VolumeFluid::UnbindViews(
	::UINT32 slotsSRV,
	::UINT32 slotsUAV
	)
{
	_d3dctx->CSSetShaderResources(0, slotsSRV, s_nullSRVs);
	_d3dctx->CSSetUnorderedAccessViews(0, slotsUAV, s_nullUAVs, s_nullUAVCounts);
}

void Demo::VolumeFluid::Update()
{
	::HRESULT result;
	m_perFrameData.EyeTransform = Viewer->View;
	result = m_perFrameBff.SetData(_d3dctx, m_perFrameData);

	_d3dctx->CSSetConstantBuffers(
		0, 1, m_perFrameBff.GetBufferAddress()
		);
	_d3dctx->CSSetSamplers(
		0, 1, m_linearSampler.GetAddressOf()
		);

	Update_Advect(); // -> speed 1 advected
	Update_ApplyForces(); // -> speed 0 advected
	Update_Divergence(); // -> div
	Update_Pressure(); // -> pressure 0
	Update_Project(); // -> speed 1

}

void Demo::VolumeFluid::Render()
{
	Render_Volume();
	SwapSpeeds();
}

void Demo::VolumeFluid::Update_Advect() // => speed 1 advected
{
	_d3dctx->CSSetShader(m_advectCS.Get(), _shaderArgs);
	_d3dctx->CSSetShaderResources(0, 1, _speed0srvs); // reads 0 speed
	_d3dctx->CSSetUnorderedAccessViews(0, 1, _speed1uavs); // writes 1 advected
	_d3dctx->Dispatch(_volumeThreadGroups); _unbind1x1;

#if 0 // MacCormack

	_d3dctx->CSSetShader(m_advectRCS.Get(), _shaderArgs);
	_d3dctx->CSSetShaderResources(0, 1, _speed0srvs); // reads 0 speed
	_d3dctx->CSSetShaderResources(1, 1, _speed1srvs); // reads 1 advected
	_d3dctx->CSSetUnorderedAccessViews(0, 1, _speed2uavs); // writes 2 advectedR
	_d3dctx->Dispatch(_volumeThreadGroups); _unbind2x1;

	_d3dctx->CSSetShader(m_advectMCS.Get(), _shaderArgs);
	_d3dctx->CSSetShaderResources(0, 1, _speed0srvs); // reads 0 speed
	_d3dctx->CSSetShaderResources(1, 1, _speed2srvs); // reads 2 advectedR
	_d3dctx->CSSetUnorderedAccessViews(0, 1, _speed1uavs); // writes 1 advectedF
	_d3dctx->Dispatch(_volumeThreadGroups); _unbind2x1;

#endif

}

void Demo::VolumeFluid::Update_ApplyForces() // => speed 0
{
	_d3dctx->CSSetShader(m_applyForcesCS.Get(), _shaderArgs);
	_d3dctx->CSSetShaderResources(0, 1, _speed1srvs);
	_d3dctx->CSSetUnorderedAccessViews(0, 1, _speed0uavs);
	_d3dctx->Dispatch(_volumeThreadGroups); _unbind;

}

void Demo::VolumeFluid::Update_Divergence()
{
	_d3dctx->CSSetShader(m_divergenceCS.Get(), _shaderArgs);
	_d3dctx->CSSetShaderResources(0, 1, _speed0srvs);
	_d3dctx->CSSetUnorderedAccessViews(0, 1, _divuavs);
	_d3dctx->Dispatch(_volumeThreadGroups); _unbind1x1;

}

void Demo::VolumeFluid::Update_Pressure()
{
	_d3dctx->CSSetShader(m_pressureCS.Get(), _shaderArgs);

	// unrolled 20=10x2 iterations

	Update_PressureIteration(); // 0 => pressure 1
	Update_PressureIteration(); // 1 => pressure 1
	Update_PressureIteration(); // 2 => pressure 1
	Update_PressureIteration(); // 3 => pressure 1
	Update_PressureIteration(); // 4 => pressure 1
	Update_PressureIteration(); // 5 => pressure 1
	Update_PressureIteration(); // 6 => pressure 1
	Update_PressureIteration(); // 7 => pressure 1
	Update_PressureIteration(); // 8 => pressure 1
	Update_PressureIteration(); // 9 => pressure 1
	
}

void Demo::VolumeFluid::Update_PressureIteration() // => pressure 1
{
	_d3dctx->CSSetShaderResources(0, 1, _divsrvs);
	_d3dctx->CSSetShaderResources(1, 1, _pressure1srvs);
	_d3dctx->CSSetUnorderedAccessViews(0, 1, _pressure0uavs);
	_d3dctx->Dispatch(_volumeThreadGroups); _unbind2x1;

	_d3dctx->CSSetShaderResources(0, 1, _divsrvs);
	_d3dctx->CSSetShaderResources(1, 1, _pressure0srvs);
	_d3dctx->CSSetUnorderedAccessViews(0, 1, _pressure1uavs);
	_d3dctx->Dispatch(_volumeThreadGroups); _unbind2x1;
}

void Demo::VolumeFluid::Update_Project()
{
	_d3dctx->CSSetShader(m_projectCS.Get(), _shaderArgs);
	_d3dctx->CSSetShaderResources(0, 1, _speed0srvs);
	_d3dctx->CSSetShaderResources(1, 1, _pressure1srvs);
	_d3dctx->CSSetUnorderedAccessViews(0, 1, _speed1uavs);
	_d3dctx->Dispatch(_volumeThreadGroups); _unbind2x1;
}

void Demo::VolumeFluid::Render_Volume()
{
	_d3dctx->CSSetShader(m_renderCS.Get(), _shaderArgs);
	_d3dctx->CSSetShaderResources(0, 1, _speed1srvs);
	_d3dctx->CSSetShaderResources(1, 1, _pressure1srvs);
	_d3dctx->CSSetShaderResources(2, 1, _divsrvs);
	_d3dctx->CSSetUnorderedAccessViews(0, 1, _renderOutput);
	_d3dctx->Dispatch(_renderThreadGroups); _unbind3x1;
}

::HRESULT Demo::VolumeFluid::CreateDeviceIndependentResources() { return S_OK; }

Demo::VolumeFluid::VolumeFluid()
{
	ZeroMemory(&m_perFrameData, sizeof PerFrame);

	m_mbLPressed = FALSE;
	m_mbRPressed = FALSE;
	//m_mbLPressed = TRUE;
	//m_mbRPressed = TRUE;

	m_perFrameData.FocusZoom.x = 1.0f;
	SetImpactRadius(2);
}

::HRESULT Demo::VolumeFluid::CreateWindowSizeDependentResources()
{
	using namespace ::Nena::Platform;
	using namespace Resources;

	::HRESULT result;

	ScopedObject<ITexture2> targetTx;
	result = D3DResources->Swapchain->GetBuffer(0, IID_PPV_ARGS(&targetTx));
	result = _d3ddvc->CreateUnorderedAccessView(targetTx.Get(), nullptr, m_targetUAV.ReleaseAndGetAddressOf());

	m_perFrameData.ViewportDims.x = D3DResources->RenderTargetSize.Width;
	m_perFrameData.ViewportDims.y = D3DResources->RenderTargetSize.Height;
	result = m_perFrameBff.SetData(_d3dctx, m_perFrameData);

	return result;
}

::HRESULT Demo::VolumeFluid::CreateDeviceDependentResources()
{
	::HRESULT result;

	result = CreateTextures();
	result = CreateBuffers();
	result = CreateShaders();

	return result;
}

::HRESULT Demo::VolumeFluid::CreateShaders()
{
	using namespace Resources;

	::HRESULT result;

	result = Assist::LoadShader(_d3ddvc, L"Shaders/Advect3CS.cso", m_advectCS.ReleaseAndGetAddressOf());
	result = Assist::LoadShader(_d3ddvc, L"Shaders/AdvectR3CS.cso", m_advectRCS.ReleaseAndGetAddressOf());
	result = Assist::LoadShader(_d3ddvc, L"Shaders/AdvectMC3CS.cso", m_advectMCS.ReleaseAndGetAddressOf());
	result = Assist::LoadShader(_d3ddvc, L"Shaders/ApplyForces3CS.cso", m_applyForcesCS.ReleaseAndGetAddressOf());
	result = Assist::LoadShader(_d3ddvc, L"Shaders/Divergence3CS.cso", m_divergenceCS.ReleaseAndGetAddressOf());
	result = Assist::LoadShader(_d3ddvc, L"Shaders/Pressure3CS.cso", m_pressureCS.ReleaseAndGetAddressOf());
	result = Assist::LoadShader(_d3ddvc, L"Shaders/Project3CS.cso", m_projectCS.ReleaseAndGetAddressOf());
	result = Assist::LoadShader(_d3ddvc, L"Shaders/RenderVolumeCS.cso", m_renderCS.ReleaseAndGetAddressOf());

	return result;
}

::HRESULT Demo::VolumeFluid::CreateBuffers()
{
	::HRESULT result;

	result = m_perFrameBff.Create(_d3ddvc);
	result = m_perFrameBff.SetData(_d3dctx, m_perFrameData);

	return result;
}

void Demo::VolumeFluid::SetVolumeSize(INT32x3 vs)
{
	m_perFrameData.ViewportDims.x = D3DResources->RenderTargetSize.Width;
	m_perFrameData.ViewportDims.y = D3DResources->RenderTargetSize.Height;
	m_perFrameData.VolumeDims.x = (::FLOAT) (vs.x - vs.x % s_renderThreadsX);
	m_perFrameData.VolumeDims.y = (::FLOAT) (vs.y - vs.y % s_renderThreadsY);
	m_perFrameData.VolumeDims.z = (::FLOAT) (vs.z - vs.z % s_volumeThreadsZ);
	m_actualVolumeSize.x = (::INT32) (m_perFrameData.VolumeDims.x / s_volumeThreadsX);
	m_actualVolumeSize.y = (::INT32) (m_perFrameData.VolumeDims.y / s_volumeThreadsY);
	m_actualVolumeSize.z = (::INT32) (m_perFrameData.VolumeDims.z / s_volumeThreadsZ);
	m_actualRenderSize.x = (::INT32) ((m_perFrameData.ViewportDims.x + s_renderThreadsX - 1) / s_renderThreadsX);
	m_actualRenderSize.y = (::INT32) ((m_perFrameData.ViewportDims.y + s_renderThreadsY - 1) / s_renderThreadsY);
	m_actualRenderSize.z = 1;
}

void Demo::VolumeFluid::SetImpactRadius(::FLOAT radius)
{
	m_perFrameData.ForceImpact.x = radius;
}

void  Demo::VolumeFluid::GetVolumeSize(INT32x3 &vs)
{
	vs.x = (::INT32) m_perFrameData.VolumeDims.x;
	vs.y = (::INT32) m_perFrameData.VolumeDims.y;
	vs.z = (::INT32) m_perFrameData.VolumeDims.z;
}

::HRESULT Demo::VolumeFluid::CreateTextures()
{
	using namespace ::Nena::Platform;
	using namespace Resources;

	::HRESULT result;

	CSampleStateDescription linearSamplerDesc(
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		0, 0, D3D11_COMPARISON_NEVER,
		nullptr, 0, D3D11_FLOAT32_MAX
		);

	CTexture3Description speedTxDesc(
		DxgiFormat::DXGI_FORMAT_R16G16B16A16_FLOAT,
		//DxgiFormat::DXGI_FORMAT_R32G32B32A32_FLOAT,
		(::UINT32) m_perFrameData.VolumeDims.x,
		(::UINT32) m_perFrameData.VolumeDims.y,
		(::UINT32) m_perFrameData.VolumeDims.z, 0,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
		);

	CTexture3Description pressureTxDesc(
		DxgiFormat::DXGI_FORMAT_R16_FLOAT,
		//DxgiFormat::DXGI_FORMAT_R32_FLOAT,
		(::UINT32) m_perFrameData.VolumeDims.x,
		(::UINT32) m_perFrameData.VolumeDims.y,
		(::UINT32) m_perFrameData.VolumeDims.z, 0,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
		);

	result = _d3ddvc->CreateSamplerState(&linearSamplerDesc, m_linearSampler.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateTexture3D(&speedTxDesc, nullptr, m_speedTxs[0].Rsrc.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateTexture3D(&speedTxDesc, nullptr, m_speedTxs[1].Rsrc.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateTexture3D(&speedTxDesc, nullptr, m_speedTxs[2].Rsrc.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateTexture3D(&pressureTxDesc, nullptr, m_pressureTxs[0].Rsrc.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateTexture3D(&pressureTxDesc, nullptr, m_pressureTxs[1].Rsrc.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateTexture3D(&pressureTxDesc, nullptr, m_divergenceTx.Rsrc.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateShaderResourceView(m_speedTxs[0].Rsrc.Get(), nullptr, m_speedTxs[0].Read.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateShaderResourceView(m_speedTxs[1].Rsrc.Get(), nullptr, m_speedTxs[1].Read.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateShaderResourceView(m_speedTxs[2].Rsrc.Get(), nullptr, m_speedTxs[2].Read.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateUnorderedAccessView(m_speedTxs[0].Rsrc.Get(), nullptr, m_speedTxs[0].Write.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateUnorderedAccessView(m_speedTxs[1].Rsrc.Get(), nullptr, m_speedTxs[1].Write.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateUnorderedAccessView(m_speedTxs[2].Rsrc.Get(), nullptr, m_speedTxs[2].Write.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateShaderResourceView(m_pressureTxs[0].Rsrc.Get(), nullptr, m_pressureTxs[0].Read.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateShaderResourceView(m_pressureTxs[1].Rsrc.Get(), nullptr, m_pressureTxs[1].Read.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateShaderResourceView(m_divergenceTx.Rsrc.Get(), nullptr, m_divergenceTx.Read.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateUnorderedAccessView(m_divergenceTx.Rsrc.Get(), nullptr, m_divergenceTx.Write.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateUnorderedAccessView(m_pressureTxs[0].Rsrc.Get(), nullptr, m_pressureTxs[0].Write.ReleaseAndGetAddressOf());
	result = _d3ddvc->CreateUnorderedAccessView(m_pressureTxs[1].Rsrc.Get(), nullptr, m_pressureTxs[1].Write.ReleaseAndGetAddressOf());

	return result;
}

void Demo::VolumeFluid::OnMouseWheel(INT16 mouseWheel)
{
	if (mouseWheel > 0i16) m_perFrameData.FocusZoom.x *= 1.1f;
	else m_perFrameData.FocusZoom.x /= 1.1f;

	Viewer->UpdateView({ 0, 0 }, m_perFrameData.FocusZoom.x);
}

void Demo::VolumeFluid::OnMouseMoved(::Nena::Vector2 mousePos)
{
	::Nena::Vector2 delta = mousePos - m_prevMousePos;

	m_perFrameData.Mouse.x = (::FLOAT) mousePos.x;
	m_perFrameData.Mouse.y = (::FLOAT) mousePos.y;
	m_prevMousePos = mousePos;

	if (m_mbLPressed)
	{
		delta.x *= 10.0f; // m_perFrameData.VolumeDims.x;// / m_perFrameData.ViewportDims.x;
		delta.y *= 10.0f; //m_perFrameData.VolumeDims.y;// / m_perFrameData.ViewportDims.y;
		m_perFrameData.Drag.x = delta.x;
		m_perFrameData.Drag.y = delta.y;

		Nena::Platform::DebugTrace("Drag %3.3f %3.3f\n", delta.x, delta.y);

	}
	else
	{
		m_perFrameData.Drag.x = 0;
		m_perFrameData.Drag.y = 0;
	}

	if (m_mbRPressed)
	{
		Viewer->UpdateView(delta, 0);
	}
}

void Demo::VolumeFluid::OnMouseRPressed(::Nena::Vector2 mousePos)
{
	m_perFrameData.Mouse.x = (::FLOAT) mousePos.x;
	m_perFrameData.Mouse.y = (::FLOAT) mousePos.y;
	m_perFrameData.Drag.x = 0;
	m_perFrameData.Drag.y = 0;
	m_prevMousePos = mousePos;
	m_mbRPressed = TRUE;
}

void Demo::VolumeFluid::OnMouseLPressed(::Nena::Vector2 mousePos)
{
	m_mbLPressed = TRUE;
}

void Demo::VolumeFluid::OnMouseRReleased(::Nena::Vector2 mousePos)
{
	m_mbRPressed = FALSE;
}

void Demo::VolumeFluid::OnMouseLReleased(::Nena::Vector2 mousePos)
{
	m_mbLPressed = FALSE;
}
