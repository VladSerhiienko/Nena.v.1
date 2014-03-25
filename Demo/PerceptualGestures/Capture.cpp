
#include "Capture.h"
#include "Pipe.h"

Nena::Video::Capture::Capture(Pipe *pipe) : Host(pipe)
{
	pxcStatus result = PXC_STATUS_NO_ERROR;
	ZeroMemory(&m_captureFilter, sizeof PXCSession::ImplDesc);
	ZeroMemory(&DeviceFilter, sizeof PXCCapture::DeviceInfo);
	m_captureFilter.subgroup = PXCSession::IMPL_SUBGROUP_VIDEO_CAPTURE;
	m_captureFilter.group = PXCSession::IMPL_GROUP_SENSOR;
}

void Nena::Video::Capture::Close()
{
	Base.ReleaseRef();
	Scheduler.ReleaseRef();
	Service = nullptr;

	Device.ReleaseRef();
}

HRESULT Nena::Video::Capture::Init()
{
	HRESULT hresult = E_POINTER;

	if (!Host) return hresult;
	if (!Host->Session) return hresult;

	hresult = S_OK;
	pxcStatus result;

	result = Host->Session->CreateImpl<PXCScheduler>(Scheduler.ReleaseRef());
	Service = Host->Session->DynamicCast<PXCSessionService>();

	pxcU32 moduleIndex = UINT_MAX;
	while (moduleIndex++, TRUE)
	{
		result = Host->Session->QueryImpl(
			&m_captureFilter, moduleIndex, &CaptureFilter
			);

		if (result < PXC_STATUS_NO_ERROR) break; else
		{
			result = Host->Session->CreateImpl<PXCCapture>(
				&CaptureFilter, Base.ReleaseRef()
				);

			if (result < PXC_STATUS_NO_ERROR) continue; else
			{
				pxcU32 deviceIndex = UINT_MAX;
				while (deviceIndex++, TRUE)
				{
					result = Base->QueryDevice(deviceIndex, &m_deviceFilter);
					if (result < PXC_STATUS_NO_ERROR) break;

					if (DeviceFilter.name[0]) if (!wcsstr(m_deviceFilter.name, DeviceFilter.name)) continue;
					if (DeviceFilter.didx > 0) if (m_deviceFilter.didx != DeviceFilter.didx) continue;
					if (DeviceFilter.did[0]) if (!wcsstr(m_deviceFilter.did, DeviceFilter.did)) continue;

					result = Base->CreateDevice(deviceIndex, Device.ReleaseRef());
					if (result < PXC_STATUS_NO_ERROR) continue; else break;
				}
			}
		}

		break;
	}

	return hresult;
}