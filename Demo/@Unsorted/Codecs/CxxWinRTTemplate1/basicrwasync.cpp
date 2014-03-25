#include "basicrwasync.h"
#include <ppltasks.h>

using namespace Platform;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Concurrency;

BasicReaderWriter::BasicReaderWriter()
{
	m_location = Package::Current->InstalledLocation;
	m_locationPath = Platform::String::Concat(m_location->Path, "\\");
}

BasicReaderWriter::BasicReaderWriter(
	_In_ StorageFolder^ folder
	)
{
	m_location = folder;
	auto path = m_location->Path;

	if (path->Length() == 0)
	{
		throw ref new Platform::FailureException();
	}

	m_locationPath = Platform::String::Concat(path, "\\");
}

void BasicReaderWriter::ReadDataAsync(
	_In_ Platform::String^ fileName,
	_In_ ReadDataAsyncCallback^ callback
	)
{
	task<StorageFile^> getFileTask(m_location->GetFileAsync(fileName));

	auto readFileTask = getFileTask.then([=](StorageFile^ file)
	{
		return FileIO::ReadBufferAsync(file);
	});

	auto readBufferTask = readFileTask.then([=](IBuffer^ buffer)
	{
		auto fileData = ref new Array<byte>(buffer->Length);
		DataReader::FromBuffer(buffer)->ReadBytes(fileData);
		callback->Invoke(fileData, AsyncStatus::Completed);
	});
}

void BasicReaderWriter::ReadTextDataAsync(
	_In_ Platform::String^ fileName,
	_In_ ReadTextDataAsyncCallback^ callback
	)
{
	task<StorageFile^> getFileTask(m_location->GetFileAsync(fileName));

	auto readFileTask = getFileTask.then([=](StorageFile^ file)
	{
		return FileIO::ReadTextAsync(file);
	});

	auto readTextTask = readFileTask.then([=](String^ text)
	{
		callback->Invoke(text, AsyncStatus::Completed);
	});
}

void BasicReaderWriter::WriteDataAsync(
	_In_ Platform::String^ fileName,
	_In_ const Platform::Array<byte>^ fileData,
	_In_ WriteDataAsyncCallback^ callback
	)
{
	auto createFileOperation = 
		m_location->CreateFileAsync(
		fileName, 
		CreationCollisionOption::ReplaceExisting
		);

	task<StorageFile^> createFileTask(createFileOperation);

	auto writeToFileTask = createFileTask.then([=](StorageFile^ file)
	{
		return FileIO::WriteBytesAsync(file, fileData);
	});

	writeToFileTask.then([=]()
	{
		callback->Invoke(AsyncStatus::Completed);
	});
}

void BasicReaderWriter::WriteTextDataAsync(
	_In_ Platform::String^ fileName,
	_In_ Platform::String^ fileData,
	_In_ WriteDataAsyncCallback^ callback
	)
{
	auto createFileOperation = 
		m_location->CreateFileAsync(
		fileName, 
		CreationCollisionOption::ReplaceExisting
		);

	task<StorageFile^> createFileTask(createFileOperation);

	auto writeToFileTask = createFileTask.then([=](StorageFile^ file)
	{
		return FileIO::WriteTextAsync(file, fileData);
	});

	writeToFileTask.then([=]()
	{
		callback->Invoke(AsyncStatus::Completed);
	});
}