#pragma once
#include "pch.h"

template <class T>
struct BasicMatrix
{
public:
	BasicMatrix(UINT32 width, UINT32 height) : m_width(width), m_height(height)
	{
		
		m_matrix = new T*[m_height];

		for (UINT32 i = 0; i < m_height; i++)
		{
			m_matrix[i] = new T[m_width];
			ZeroMemory(m_matrix[i], sizeof(T) * m_width);
		}
	}

public:
	void Release()
	{
		for (UINT32 i = 0; i < m_height; i++)
		{
			ZeroMemory(&m_matrix[i], sizeof(T) * m_width);
			delete m_matrix[i];
		}

		m_width = NULL;
		m_height = NULL;
	}

	UINT32 Width()
	{
		return m_width;
	}

	UINT32 Height()
	{
		return m_height;
	}

public:
	T* operator[](UINT32 index)
	{
		return m_matrix[index];
	}


protected:
	T			**m_matrix;
	UINT32		m_width;
	UINT32		m_height;
};

#define EMPTY			((BYTE)('\0'))
#define ZERO			((BYTE)('0'))
#define BIT				((BYTE)('1'))

struct ByteMatrix : public BasicMatrix<BYTE>
{
	ByteMatrix(UINT32 width, UINT32 height) : 
		BasicMatrix(width, height)
	{
	}

	void Row(_Out_ BYTE* row, _In_ UINT32 rowIndex)
	{
		row = m_matrix[rowIndex];
	}

	void Column(_Out_ BYTE **column, _In_ UINT32 columnIndex)
	{
		(*column) = new BYTE[m_height];
		UINT32 i = 0, j = 0;
		for (/* i, j */; i < m_height; i++)
		{
			if (m_matrix[i][columnIndex])
			{
				(*column)[j++] = m_matrix[i][columnIndex];
			}
		}

		(*column)[j] = EMPTY;
	}

	void ReversedColumn(_Out_ BYTE **column, _In_ UINT32 columnIndex)
	{
		(*column) = new BYTE[m_height];
		INT32 i = m_height - 1, j = 0;

		for (/* i, j */; i >= 0; i--)
		{
			if (m_matrix[i][columnIndex])
			{
				(*column)[j++] = m_matrix[i][columnIndex];
			}
		}

		(*column)[j] = EMPTY;
	}
};

#define BUFFER_SIZE		100

namespace StringConvertions
{
	inline void Char8ToChar16(
		_In_ CHAR *src, _Out_ WCHAR *dest, 
		_Out_opt_ UINT32 *converted
		)
	{
		size_t srcSize = strlen(src) + 1;
		errno_t err;

		err = mbstowcs_s(
			converted,
			dest, srcSize, 
			src, _TRUNCATE
			);
	}

	inline void UChar8ToChar16(
		_In_ BYTE *src, _Out_ WCHAR *dest, 
		_Out_opt_ UINT32 *converted
		)
	{

		Char8ToChar16(reinterpret_cast<CHAR*>(src), dest, converted);
	}
}