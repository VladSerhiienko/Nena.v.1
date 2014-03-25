#pragma once

#include "pch.h"
#include "basicalphabet.h"
#include "basicmatrix.h"

ref class HuffmanAlphabet sealed : public BasicAlphabet
{
internal:
	HuffmanAlphabet();
	HuffmanAlphabet(_In_ Platform::Collections::Vector<BasicInputData^>^ data);

public:
	void GenerateEncoding();

	/*operator BasicAlphabet^()
	{
		return ref new BasicAlphabet(m_data);
	}*/

protected private:
	void GenerateEncoding(
		_In_ UINT32 currentRow, 
		_In_ UINT32 **bar, _In_ ByteMatrix **matrix
		);

	void GenerateEncoding(_In_ ByteMatrix matrix);

private:
	void Unite(
		_In_ UINT32 currentRow,
		_In_ UINT32 leftIndex, _In_ UINT32 rightIndex, 
		_In_ UINT32 **bar, _In_ ByteMatrix **matrix
		);

	void Fill(
		_In_ UINT32 index, _In_ UINT32 row, 
		_In_ UINT32 **bar, _In_ ByteMatrix **matrix,
		_In_ BYTE value
		);

	void MapFrequencies(UINT32 **bar);
	void UpdateIndicies(_Out_ UINT32 *currentIndex, _Out_ UINT32 *neightbourIndex, _In_ UINT32* bar);

};