#ifndef JUNIHERZ_TSP_STORAGE_H
#define JUNIHERZ_TSP_STORAGE_H
#include "bscmatrix.h"

namespace Juniherz
{
	template <typename T>
	class TpsStorage;
	
	typedef TpsStorage<float32> Storage32;
	typedef TpsStorage<float64> Storage64;

	template <typename T>
	class HVASolver;

	template <typename T>
	class TpsStorage
	{
		friend HVASolver<T>;
	public:
		// Initialize Tps storage.
		TpsStorage( _In_ uint32 nSenders, _In_ uint32 nRecievers );
		// Release used inner resources.
		void Release( );
		virtual ~TpsStorage();

	public:
		bool SetPriceValues( _In_ int32 offset, _In_ uint32 count, _In_ T* values );
		bool SetSupplyValues( _In_ int32 offset, _In_ uint32 count, _In_ T* values );
		bool SetDemandValues( _In_ int32 offset, _In_ uint32 count, _In_ T* values );
		uint32 GetWidth( );
		uint32 GetHeight( );

	protected:
		bool SetValues( _In_ int32 offset, _In_ uint32 srcCount, _In_ T* src, _In_ uint32 descCount, _In_ T* dest );

	private:
		Matrix<uint32> *m_vtRecieverIndices;
		Matrix<uint32> *m_vtSenderIndices;
		Matrix<T> *m_mtPriceValues;
		Matrix<T> *m_vtSupplyValues;
		Matrix<T> *m_vtDemandValues;

	};

#include "bsctspstorage.inl"
}

#endif