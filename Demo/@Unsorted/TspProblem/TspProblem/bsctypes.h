#ifndef JH_TSP_BASIC_TYPES_H
#define JH_TSP_BASIC_TYPES_H
#include <sal.h>
#include <memory>

typedef int int32;
typedef unsigned int uint32;
typedef unsigned long ulong32;
typedef unsigned long long ulong64;
typedef float float32;
typedef double float64;

namespace Juniherz
{
	template <typename T>
	class Matrix
	{
	public:
		// Initialize matrix. 
		Matrix( _In_ uint32 uWidth, _In_ uint32 uHeight );
		virtual ~Matrix( );
		// Get matrix value. 
		bool GetAt( _In_ uint32 uRow, _In_ uint32 uColumn, _Out_ T &tValue );
		// Set matrix value. 
		bool SetAt( _In_ uint32 uRow, _In_ uint32 uColumn, _In_ T tValue );
		// Release all the used inner resources. 
		void Release( );

	public:
		// Not safe. Use GetAt/SetAt instead. 
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
#include "bsctypes.inl"

	typedef Matrix<float32> matrix;
};



#endif