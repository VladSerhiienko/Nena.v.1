#include "pch.h"
#include "shafalphabet.h"

ShannonFanoAlphabet::ShannonFanoAlphabet() : BasicAlphabet()
{
}

ShannonFanoAlphabet::ShannonFanoAlphabet(_In_ Platform::Collections::Vector<BasicInputData^>^ data) : 
	BasicAlphabet(data)
{
}

void ShannonFanoAlphabet::GenerateEncoding()
{
	BasicAlphabet::SortNonIncreasing();

	GenerateEncoding(0, m_data->Size);
}

void ShannonFanoAlphabet::GenerateEncoding(_In_ UINT32 startIndex, _In_ UINT32 endIndex)
{
	if ((endIndex - startIndex) == 1) { return; }

	UINT32 middleIndex = Middle(startIndex, endIndex);

	AddBit(startIndex, middleIndex);
	AddZero(middleIndex, endIndex);

	GenerateEncoding(startIndex, middleIndex);
	GenerateEncoding(middleIndex, endIndex);
}

void ShannonFanoAlphabet::AddBit(_In_ UINT32 startIndex, _In_ UINT32 endIndex)
{
	for (UINT32 i = startIndex; i < endIndex; i++)
	{
		this[i]->AddBit();
	}
}

void ShannonFanoAlphabet::AddZero(_In_ UINT32 startIndex, _In_ UINT32 endIndex)
{
	for (UINT32 i = startIndex; i < endIndex; i++)
	{
		this[i]->AddZero();
	}
}