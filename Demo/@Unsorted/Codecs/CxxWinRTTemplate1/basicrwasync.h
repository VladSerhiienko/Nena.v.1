#pragma once
#include "pch.h"

delegate void ReadDataAsyncCallback(
	Platform::Array<byte>^, 
	Windows::Foundation::AsyncStatus
	);

delegate void ReadTextDataAsyncCallback(
	Platform::String^, 
	Windows::Foundation::AsyncStatus
	);

delegate void WriteDataAsyncCallback(
	Windows::Foundation::AsyncStatus
	);

ref class BasicReaderWriter
{
internal:
	BasicReaderWriter();
	BasicReaderWriter(
		_In_ Windows::Storage::StorageFolder^ folder
		);
public:
	void ReadDataAsync(
		_In_ Platform::String^ fileName,
		_In_ ReadDataAsyncCallback^ callback
		);

	void ReadTextDataAsync(
		_In_ Platform::String^ fileName,
		_In_ ReadTextDataAsyncCallback^ callback
		);

	void WriteDataAsync(
		_In_ Platform::String^ fileName,
		_In_ const Platform::Array<byte>^ fileData,
		_In_ WriteDataAsyncCallback^ callback
		);

	void WriteTextDataAsync(
		_In_ Platform::String^ fileName,
		_In_ Platform::String^ fileData,
		_In_ WriteDataAsyncCallback^ callback
		);

private:
	Windows::Storage::StorageFolder^ m_location;
	Platform::String^ m_locationPath;

};
