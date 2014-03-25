template <typename T>
TpsStorage<T>::TpsStorage( _In_ uint32 nSenders, _In_ uint32 nRecievers ) : 
	m_vtSenderIndices( NULL ), 
	m_vtRecieverIndices( NULL ),
	m_mtPriceValues( NULL )
{
	m_vtSenderIndices = new Matrix<uint32>( nSenders, 1u );
	m_vtRecieverIndices = new Matrix<uint32>( 1u, nRecievers );

	m_vtSupplyValues = new Matrix<T>( nSenders, 1u );
	m_vtDemandValues = new Matrix<T>( 1u, nRecievers );

	m_mtPriceValues = new Matrix<T>( nSenders, nRecievers );

	for (uint32 i = 0; i < nSenders; i++)
	{
		(*m_vtSenderIndices)[i][0u] = (i + 1);
	}

	for (uint32 i = 0; i < nRecievers; i++)
	{
		(*m_vtRecieverIndices)[0u][i] = (i + 1);
	}
}

template <typename T>
bool TpsStorage<T>::SetPriceValues( _In_ int32 offset, _In_ uint32 count, _In_ T* values )
{
	return SetValues( offset, count, values, (GetHeight() * GetWidth()), m_mtPriceValues->m_tData );
}

template <typename T>
bool TpsStorage<T>::SetSupplyValues( _In_ int32 offset, _In_ uint32 count, _In_ T* values )
{
	return SetValues( offset, count, values, GetHeight(), m_vtSupplyValues->m_tData );
}

template <typename T>
bool TpsStorage<T>::SetDemandValues( _In_ int32 offset, _In_ uint32 count, _In_ T* values )
{
	return SetValues( offset, count, values, GetWidth(), m_vtDemandValues->m_tData );
}

template <typename T>
bool TpsStorage<T>::SetValues( _In_ int32 offset, _In_ uint32 srcCount, _In_ T* src, _In_ uint32 descCount, _In_ T* dest )
{
	if ( (offset + srcCount) > (descCount) )
	{
		return ( false );
	}

	void *destination = (void*)(&dest[offset]);
	void *source = (void*)(src);
	size_t sizeSource = (sizeof(T) * srcCount);
	destination = memcpy( destination, source, sizeSource);
	return ( true );
}

template <typename T>
uint32 TpsStorage<T>::GetWidth( )
{
	return ( m_mtPriceValues->GetWidth() );
}

template <typename T>
uint32 TpsStorage<T>::GetHeight( )
{
	return ( m_mtPriceValues->GetHeight() );
}

template <typename T>
void TpsStorage<T>::Release( )
{
	m_vtSenderIndices->Release();
	m_vtSenderIndices = NULL;

	m_vtRecieverIndices->Release();
	m_vtRecieverIndices = NULL;

	m_mtPriceValues->Release();
	m_mtPriceValues = NULL;
}

template <typename T>
TpsStorage<T>::~TpsStorage( )
{
	Release();
}