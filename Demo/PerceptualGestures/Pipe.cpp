#include "Pipe.h"

Nena::Video::Pipe::Pipe()
{
}

HRESULT Nena::Video::Pipe::Init()
{
	pxcStatus result;
	result = PXCSession_Create(&Session);
	if (result < PXC_STATUS_NO_ERROR) return E_FAIL;
	return S_OK;
}

void Nena::Video::Pipe::Close()
{
	Session.ReleaseRef();
}