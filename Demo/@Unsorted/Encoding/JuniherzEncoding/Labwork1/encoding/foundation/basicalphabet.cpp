#include "pch.h"
#include "basicalphabet.h"
#include <string>
#include <iostream>
#include <vector>

#define BUFFER_SIZE 100

BasicAlphabet::BasicAlphabet()
{
	m_data = ref new Platform::Collections::Vector<BasicInputData^>();
}

BasicAlphabet::BasicAlphabet(
	_In_ Platform::Collections::Vector<BasicInputData^>^ data
	)
{
	m_data = data;
}

void BasicAlphabet::Map(
	_In_ Platform::String^ mapping
	)
{
	std::wstring data(mapping->Data());

	WCHAR *context;
	WCHAR *part = wcstok_s(&data[0], L"\n", &context);

	while (part != NULL)
	{
		m_data->Append(ref new BasicInputData(part));
		part = wcstok_s(context, L"\n", &context);
	}
}

void BasicAlphabet::ReadFrom(_In_ Platform::String^ text)
{
	std::wstring data(text->Data());

	WCHAR *context;
	WCHAR *part = wcstok_s(&data[0], L"\r\n", &context);

	WCHAR *code = nullptr;
	WCHAR *symbol = wcstok_s(part, L"\t", &code);

	while (part != NULL)
	{
		BasicInputData^ input = ref new BasicInputData(*symbol, 0);
		input->Code = ref new Platform::String(code);

		m_data->Append(input);

		part = wcstok_s(context, L"\r\n", &context);
		symbol = wcstok_s(part, L"\t", &code);
	}
}

INT32 BasicAlphabet::SumFrequencies(_In_ UINT32 startIndex, _In_ UINT32 endIndex)
{
	UINT32 result (0);

	for (UINT32 i = startIndex; i < endIndex; i++)
	{
		result += this[i]->Frequency;
	}

	return result;
}

UINT32 BasicAlphabet::Middle(_In_ UINT32 startIndex, _In_ UINT32 endIndex)
{
	if ((endIndex - startIndex) == 1)
	{
		return (UINT32)1;
	}

	UINT32 leftSum (this[startIndex]->Frequency);
	UINT32 rightSum (SumFrequencies(startIndex + 1, endIndex));

	INT32 currentDelta (abs((INT32)(rightSum - leftSum)));
	INT32 previousDelta (0);

	for (UINT32 i = startIndex + 1; i < endIndex; i++)
	{
		previousDelta = currentDelta;

		leftSum += this[i]->Frequency;
		rightSum -= this[i]->Frequency;

		currentDelta = rightSum - leftSum;

		if (abs(currentDelta) >= abs(previousDelta))
		{
			return i;
		}
	}

	throw ref new Platform::FailureException();
}


WCHAR BasicAlphabet::Decode(Platform::String^ encoded)
{
	for (UINT32 i = 0; i < Size; i++)
	{
		if (this[i]->Code == encoded)
		{
			return this[i]->Symbol;
		}
	}

	return DECODED_NOT_FOUND;
}

Platform::String^ BasicAlphabet::Encode(WCHAR decoded)
{
	for (UINT32 i = 0; i < Size; i++)
	{
		if (this[i]->Symbol == decoded)
		{
			return this[i]->Code;
		}
	}

	return ENCODED_NOT_FOUND;
}

Platform::String^ BasicAlphabet::Save()
{
	WCHAR wcArray[BUFFER_SIZE];
	ZeroMemory(&wcArray, sizeof(WCHAR) * BUFFER_SIZE);

	std::wstring wcszResult(wcArray);

	for (UINT32 i = 0; i < Size; i++)
	{
		wcszResult += this[i]->Symbol;
		wcszResult.append(L"\t");
		wcszResult.append(this[i]->Code->Data());
		wcszResult.append(L"\n");
	}

	return ref new Platform::String(wcszResult.data());
}

void BasicAlphabet::SortNonIncreasing()
{
	for (UINT32 i = 0; i < m_data->Size - 2; i++)
	{
		for (UINT32 j = i; j < m_data->Size - 1; j++)
		{
			if (this[i]->Frequency < this[i]->Frequency)
			{
				Swap(i, j);
			}
		}
	}
}

void BasicAlphabet::Swap(_In_ UINT32 indexSrc, _In_ UINT32 indexDest)
{
	auto temp = this[indexDest];
	m_data->SetAt(indexDest, this[indexSrc]);
	m_data->SetAt(indexSrc, temp);
}

BasicInputData^ BasicAlphabet::operator[](_In_ UINT32 index)
{
	return m_data->GetAt(index);
}

Platform::Collections::Vector<BasicInputData^>^ BasicAlphabet::Data::get()
{
	return m_data;
}

UINT32 BasicAlphabet::Size::get()
{
	return m_data->Size;
}