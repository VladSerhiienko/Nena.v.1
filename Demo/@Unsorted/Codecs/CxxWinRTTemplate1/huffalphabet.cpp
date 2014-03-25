#include "huffalphabet.h"

void InspectMatrix(ByteMatrix matrix)
{
	for (int i = matrix.Width() - 1; i >= 0; i--)
	{
		UINT32 converted = 0;
		WCHAR string[100];
		BYTE *column; //= new BYTE[matrix.Height()];

		matrix.Column(&column, i);

		StringConvertions::UChar8ToChar16(column, string, &converted);

		OutputDebugStringW(string);
		OutputDebugStringW(L"\n");
	}
}

HuffmanAlphabet::HuffmanAlphabet() : 
	BasicAlphabet()
{
}

HuffmanAlphabet::HuffmanAlphabet(_In_ Platform::Collections::Vector<BasicInputData^>^ data) : 
	BasicAlphabet(data)
{
}

void HuffmanAlphabet::GenerateEncoding()
{
	BasicAlphabet::SortNonIncreasing();

	UINT32 width = BasicAlphabet::Size;
	UINT32 height = width - 1;

	ByteMatrix *matrix = new ByteMatrix(width, height);

	UINT32 *bar = new UINT32[width];
	ZeroMemory(bar, sizeof(UINT32) * width);
	MapFrequencies(&bar);

	(*matrix)[0][1] = BIT;
	(*matrix)[0][0] = ZERO;

	bar[1] = bar[1] + bar[0];
	bar[0] = 0;

	GenerateEncoding(1, &bar, &matrix);

	GenerateEncoding(*matrix);

	InspectMatrix(*matrix);

	matrix->Release();
	delete matrix;

	width = NULL;
	height = NULL;
}

void HuffmanAlphabet::GenerateEncoding(_In_ ByteMatrix matrix)
{
	INT32 i = matrix.Width() - 1, j (0);

	for (/* i, j */; i >= 0; i--)
	{
		BYTE *column;

		matrix.ReversedColumn(&column, i);

		WCHAR *string = new WCHAR[matrix.Height()];
		UINT32 converted = 0;
		StringConvertions::UChar8ToChar16(column, string, &converted);

		this[j++]->Code = ref new Platform::String(string);
	}
}

void HuffmanAlphabet::GenerateEncoding(
	_In_ UINT32 row, 
	_In_ UINT32 **bar, _In_ ByteMatrix **matrix
	)
{
	for (UINT32 currentRow = row; currentRow < BasicAlphabet:: Size - 1; currentRow++)
	{
		UINT32 currentIndex (0);
		UINT32 neightbourIndex (0);

		UpdateIndicies(&currentIndex, &neightbourIndex, (*bar));
		Unite(currentRow, currentIndex, neightbourIndex, bar, matrix);
	}
}

void HuffmanAlphabet::Unite(
	_In_ UINT32 currentRow,
	_In_ UINT32 leftIndex, _In_ UINT32 rightIndex, 
	_In_ UINT32 **bar, _In_ ByteMatrix **matrix
	)
{
	UINT32 leftValue = (*bar)[leftIndex];
	UINT32 rightValue = (*bar)[rightIndex];
	UINT32 finalValue = leftValue + rightValue;

	if (leftValue >= rightValue)
	{
		Fill(leftIndex, currentRow, bar, matrix, BIT);
		Fill(rightIndex, currentRow, bar, matrix, ZERO);
	}
	else
	{
		Fill(leftIndex, currentRow, bar, matrix, ZERO);
		Fill(rightIndex, currentRow, bar, matrix, BIT);
	}

	(*bar)[rightIndex] = 0;
	(*bar)[leftIndex] = finalValue;
}

void HuffmanAlphabet::Fill(
	_In_ UINT32 i, _In_ UINT32 row, 
	_In_ UINT32 **bar, _In_ ByteMatrix **matrix,
	_In_ BYTE value
	)
{
	INT32 index = (INT32)i;
	(**matrix)[row][index--] = value;

	UINT32 barValue = (*bar)[index];

	while (!barValue && (index >= 0))
	{
		(**matrix)[row][index--] = value;
		barValue = (*bar)[index];
	}
}

void HuffmanAlphabet::UpdateIndicies(
	_Out_ UINT32 *currentIndex, _Out_ UINT32 *neightbourIndex, 
	_In_ UINT32 *bar
	)
{
	(*currentIndex) = 0;
	(*neightbourIndex) = 0;

	for (UINT32 i = (*currentIndex); i < BasicAlphabet::Size; i++)
	{
		if (bar[i])
		{
			(*currentIndex) = i;
			break;
		}
	}

	for (UINT32 i = 0; i < BasicAlphabet::Size; i++)
	{
		if ((bar[i] <= bar[(*currentIndex)]) && (bar[i]))
		{
			(*currentIndex) = i;
		}
	}

	for (UINT32 i = 0; i < BasicAlphabet::Size; i++)
	{
		if (bar[i] && (i != (*currentIndex)))
		{
			(*neightbourIndex) = i;
			break;
		}
	}

	for (UINT32 i = (*neightbourIndex); i < BasicAlphabet::Size; i++)
	{
		if ((bar[i] >= bar[(*currentIndex)]) && 
			(bar[i] <= bar[(*neightbourIndex)]) && 
			(bar[i]) && (i != (*currentIndex)))
		{
			(*neightbourIndex) = i;
		}
	}

	if ((*currentIndex) < (*neightbourIndex))
	{
		UINT32 temp = (*currentIndex);
		(*currentIndex) = (*neightbourIndex);
		(*neightbourIndex) = temp;
	}
}

void HuffmanAlphabet::MapFrequencies(UINT32 **bar)
{
	INT32 i = (INT32)(Size - 1);

	while (i >= 0)
	{
		(*bar)[i] = this[(Size - 1) - i]->Frequency;
		i--;
	}
}