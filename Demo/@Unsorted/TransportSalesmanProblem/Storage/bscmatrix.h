#ifndef JUNIHERZ_BASIC_MATRIX_H
#define JUNIHERZ_BASIC_MATRIX_H
#include "bsctypespch.h"

namespace Juniherz
{
	template <typename T>
	class TpsStorage;

	// Very basic matrix implementation.	
	template <typename T>
	class Matrix
	{
		friend TpsStorage<T>;
	public:
		// Initialize matrix. 
		Matrix( _In_ uint32 uHeight, _In_ uint32 uWidth );
		virtual ~Matrix( );
		// Get matrix value. 
		bool GetAt( _In_ uint32 uRow, _In_ uint32 uColumn, _Out_ T &tValue );
		// Set matrix value. 
		bool SetAt( _In_ uint32 uRow, _In_ uint32 uColumn, _In_ T tValue );
		// Release all the used inner resources. 
		virtual void Release( );

	public:
		// Get column count.
		uint32 GetWidth( );
		// Get row count.
		uint32 GetHeight( );
		// Unsafe. Use GetAt/SetAt instead. 
		T* operator[] ( uint32 uIndex );

	private:
		// Check if queued value is in range.
		bool IsInRange( _In_ uint32 uRow, _In_ uint32 uColumn );

	protected:
		// Matrix actual data. 
		T *m_tData;
		// Matrix row count. 
		uint32 m_uWidth;
		// Matrix column count. 
		uint32 m_uHeight;

	};

	// Implementation.
	#include "bscmatrix.inl"
	// Type definitions
	typedef Matrix<float32> matrix32;
	typedef Matrix<float64> matrix64;
};

#endif