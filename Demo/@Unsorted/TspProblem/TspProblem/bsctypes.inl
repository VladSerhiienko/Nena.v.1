#include <memory>

template <typename T>
Matrix<T>::Matrix( _In_ uint32 height, _In_ uint32 width ) : 
	m_tData ( 0 ), m_uWidth ( 0u ), m_uHeight ( 0u )
{
	m_uWidth = width;
	m_uHeight = height;

	size_t size = width * height * sizeof(T);
	m_tData = (T*)::malloc( size );
	m_tData = (T*)::memset( m_tData, 0, size );
}

template <typename T>
Matrix<T>::~Matrix( )
{
	Release();
}

template <typename T>
bool Matrix<T>::GetAt( _In_ uint32 row, _In_ uint32 column, _Out_ T &value )
{
	bool result = IsInRange( row, column );
	value = result ? (&m_tData[row * m_uWidth])[column] : 0;
	return ( result );
}

template <typename T>
bool Matrix<T>::SetAt( _In_ uint32 row, _In_ uint32 column, _In_ T value )
{
	if ( !IsInRange( row, column ) )
	{
		return ( false );
	}
	else
	{
		(&m_tData[row * m_uWidth])[column] = value;
		return ( true );
	}
}

template <typename T>
void Matrix<T>::Release( )
{
	m_uWidth = 0;
	m_uHeight = 0;

	if ( m_tData )
	{
		delete[] m_tData;
		m_tData = 0;
	}
}

template <typename T>
T* Matrix<T>::operator[] ( uint32 uIndex )
{
	return &m_tData[uIndex * m_uWidth];
}

template <typename T>
bool Matrix<T>::IsInRange( _In_ uint32 uRow, _In_ uint32 uColumn )
{
	return ( (uRow < m_uWidth) && (uColumn < m_uHeight) );
}
 
