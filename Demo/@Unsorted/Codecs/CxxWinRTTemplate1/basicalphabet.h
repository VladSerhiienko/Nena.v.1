#pragma once
#include "pch.h"
#include "basicindata.h"

#define DECODED_NOT_FOUND ((WCHAR)(0));
#define ENCODED_NOT_FOUND (L"");

ref class BasicAlphabet
{
internal:
	BasicAlphabet();
	BasicAlphabet(_In_ Platform::Collections::Vector<BasicInputData^>^ data);

public:
	void Map(_In_ Platform::String^ mapping);
	void SortNonIncreasing();
	INT32 SumFrequencies(_In_ UINT32 startIndex, _In_ UINT32 endIndex);

public:
	WCHAR Decode(Platform::String^ encoded);
	Platform::String^ Encode(WCHAR decoded);
	void ReadFrom(_In_ Platform::String^ text);
	Platform::String^ Save();

protected private:
	void Swap(_In_ UINT32 indexSrc, _In_ UINT32 indexDesc);
	UINT32 Middle(_In_ UINT32 startIndex, _In_ UINT32 endIndex);

internal:
	BasicInputData^ operator[](_In_ UINT32 index);

	property Platform::Collections::Vector<BasicInputData^>^ Data
	{
		Platform::Collections::Vector<BasicInputData^>^ get();
	}

	property UINT32 Size
	{
		UINT32 get();
	}

protected private:
	Platform::Collections::Vector<BasicInputData^>^ m_data;
};