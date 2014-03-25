#include "Reader.h"

Nena::Utility::Reader::Buffer::Buffer() : 
	Data(nullptr), Length(0), 
	Error(ErrorCode::Empty)
{
}

::BOOL Nena::Utility::Reader::Buffer::AllocateStorage(size_t size)
{
	ReleaseStorage();

	::HANDLE memoryHandle = ::malloc(size);

	if (!memoryHandle) return FALSE;
	else ::memset(memoryHandle, 0, size);

	Data = (::BYTE*)memoryHandle;
	Length = size;

	return TRUE;
}

::BOOL Nena::Utility::Reader::Buffer::ReleaseStorage()
{
	if (Data) { ::free(Data); Length = 0; Error = ErrorCode::Empty; return TRUE; }
	else return FALSE;
}

Nena::Utility::Reader::BufferTask Nena::Utility::Reader::ReadDataAsync(std::wstring path)
{
	std::wstring name = path; return concurrency::create_task(
		[this, name] { return ReadData(name.c_str()); }
		);
}

Nena::Utility::Reader::Buffer Nena::Utility::Reader::ReadData(std::wstring path)
{
	using namespace Microsoft::WRL;

	Reader::Buffer buffer;
	::FILE_STANDARD_INFO info = { 0 };
	::CREATEFILE2_EXTENDED_PARAMETERS fileDesc = { 0 };

	fileDesc.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
	fileDesc.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	fileDesc.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
	fileDesc.dwSecurityQosFlags = SECURITY_ANONYMOUS;
	fileDesc.lpSecurityAttributes = nullptr;
	fileDesc.hTemplateFile = nullptr;

	Wrappers::FileHandle handle(CreateFile2(
		path.c_str(), GENERIC_READ, FILE_SHARE_READ,
		OPEN_EXISTING, &fileDesc
		));

	if (handle.Get() == INVALID_HANDLE_VALUE)
	{
		buffer.Error = ErrorCode::FailedToOpenFile;
		return buffer;
	}

	if (!GetFileInformationByHandleEx(
		handle.Get(), FileStandardInfo,
		&info, sizeof FILE_STANDARD_INFO
		))
	{
		buffer.Error = ErrorCode::FailedToGetFileInfo;
		return buffer;
	}

	if (info.EndOfFile.HighPart)
	{
		buffer.Error = ErrorCode::FileIsTooBig;
		return buffer;
	}

	if (!buffer.AllocateStorage(info.EndOfFile.LowPart))
	{
		buffer.Error = ErrorCode::FileIsTooBig;
		return buffer;
	}

	if (!ReadFile(
		handle.Get(), buffer.Data,
		(DWORD) buffer.Length, nullptr, nullptr
		))
	{
		buffer.Error = ErrorCode::FailedToReadFileData;
		return buffer;
	}

	buffer.Error = ErrorCode::Success;
	return buffer;
}