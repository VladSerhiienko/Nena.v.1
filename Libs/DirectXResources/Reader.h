
#ifndef __NENA_RESOURCES_READER_H__
#define __NENA_RESOURCES_READER_H__

#include <wrl.h>
#include <fileapi.h>
#include <ppltasks.h>

namespace Nena
{
	namespace Utility
	{

		struct Reader
		{
			enum class ErrorCode
			{
				Success,
				Empty,
				FailedToOpenFile,
				FailedToGetFileInfo,
				FailedToReadFileData,
				FileIsTooBig,
			};

			struct Buffer
			{
				::BYTE *Data;
				::SIZE_T Length;

				Buffer();

				::BOOL AllocateStorage(size_t size);
				::BOOL ReleaseStorage();

				ErrorCode Error;
			};

			typedef concurrency::task<Buffer> BufferTask;

			Reader::Buffer ReadData(std::wstring path);
			Reader::BufferTask ReadDataAsync(std::wstring path);
		};
	}
}

#endif // !__NENA_RESOURCES_READER_H__
