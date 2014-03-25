#pragma once

#include "pch.h"
#include "encoding\foundation\basicalphabet.h"

ref class ShannonFanoAlphabet sealed : public BasicAlphabet
{
internal:
	ShannonFanoAlphabet();
	ShannonFanoAlphabet(_In_ Platform::Collections::Vector<BasicInputData^>^ data);

public:
	void GenerateEncoding();

	operator BasicAlphabet^()
	{
		return ref new BasicAlphabet(m_data);
	}

protected private:
	void GenerateEncoding(_In_ UINT32 startIndex, _In_ UINT32 endIndex);
	void AddBit(_In_ UINT32 startIndex, _In_ UINT32 endIndex);
	void AddZero(_In_ UINT32 startIndex, _In_ UINT32 endIndex);

};
