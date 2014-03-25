template <typename T>
HVASolver<T>::HVASolver( TpsStorage<T> storage ) : 
	m_pvtFineRow( 0 ), m_pvtFineColumn( 0 )
{
	m_ptStorage = &storage;

	m_pvtFineRow = new Matrix<T>( 1u, storage.GetWidth() );
	m_pvtFineColumn = new Matrix<T>( storage.GetHeight(), 1u );
	m_pmtSolution = new Matrix<T>( storage.GetHeight(), storage.GetWidth() );
}

template <typename T>
void  HVASolver<T>::CalculateFines( )
{
	uint32 width = GetWidth();
	uint32 height = GetHeight();


}

template <typename T>
bool HVASolver<T>::GetNextValues( uint32 &row, uint32 &column )
{

}

template <typename T>
uint32 HVASolver<T>::GetWidth( )
{
	return ( m_ptStorage->GetWidth() );
}

template <typename T>
uint32 HVASolver<T>::GetHeight( )
{
	return ( m_ptStorage->GetHeight() );
}

template <typename T>
void HVASolver<T>::Release( )
{
	// Delete penalties from rows
	if ( m_pvtFineRow )
	{
		delete[] m_pvtFineRow;
		m_pvtFineRow = 0;
	}

	// Delete penalties from columns
	if ( m_pvtFineColumn )
	{
		delete[] m_pvtFineColumn;
		m_pvtFineColumn = 0;
	}
}

template <typename T>
HVASolver<T>::~HVASolver( )
{
	Release();
}