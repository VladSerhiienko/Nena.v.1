#pragma once

#include <Nena\DirectXTypes.h>
/// From DirectX Toolkit

namespace Nena
{
	namespace Utility
	{

		HRESULT CreateWICTextureFromMemory(
			_In_ Nena::Graphics::Resources::IDirect3DDevice* d3dDevice,
			_In_ Nena::Graphics::Resources::IDirect3DContext* d3dContext,
			_In_reads_bytes_(wicDataSize) const uint8_t* wicData,
			_In_ size_t wicDataSize,
			_Outptr_opt_ Nena::Graphics::Resources::IDirect3DResource** texture,
			_Outptr_opt_ Nena::Graphics::Resources::ISRView** textureView,
			_In_ size_t maxsize = 0
			);

		HRESULT CreateWICTextureFromFile(
			_In_ Nena::Graphics::Resources::IDirect3DDevice* d3dDevice,
			_In_ Nena::Graphics::Resources::IDirect3DContext* d3dContext,
			_In_z_ const wchar_t* szFileName,
			_Outptr_opt_ Nena::Graphics::Resources::IDirect3DResource** texture,
			_Outptr_opt_ Nena::Graphics::Resources::ISRView** textureView,
			_In_ size_t maxsize = 0
			);

		HRESULT CreateWICTextureFromMemoryEx(
			_In_ Nena::Graphics::Resources::IDirect3DDevice* d3dDevice,
			_In_ Nena::Graphics::Resources::IDirect3DContext* d3dContext,
			_In_reads_bytes_(wicDataSize) const uint8_t* wicData,
			_In_ size_t wicDataSize,
			_In_ size_t maxsize,
			_In_ D3D11_USAGE usage,
			_In_ unsigned int bindFlags,
			_In_ unsigned int cpuAccessFlags,
			_In_ unsigned int miscFlags,
			_In_ bool forceSRGB,
			_Outptr_opt_ Nena::Graphics::Resources::IDirect3DResource** texture,
			_Outptr_opt_ Nena::Graphics::Resources::ISRView** textureView
			);

		HRESULT CreateWICTextureFromFileEx(
			_In_ Nena::Graphics::Resources::IDirect3DDevice* d3dDevice,
			_In_ Nena::Graphics::Resources::IDirect3DContext* d3dContext,
			_In_z_ const wchar_t* szFileName,
			_In_ size_t maxsize,
			_In_ D3D11_USAGE usage,
			_In_ unsigned int bindFlags,
			_In_ unsigned int cpuAccessFlags,
			_In_ unsigned int miscFlags,
			_In_ bool forceSRGB,
			_Outptr_opt_ Nena::Graphics::Resources::IDirect3DResource** texture,
			_Outptr_opt_ Nena::Graphics::Resources::ISRView** textureView
			);
	}
}
