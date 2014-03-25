#include "bsctypespch.h"
#include "bscmatrix.h"
using namespace Juniherz;

int main( )
{
	matrix32 *m = new matrix32( 2u, 3u );
	(*m)[0u][0u] = 2.0f;
	(*m)[1u][2u] = 2.0f;

	float32 fValue = 0.0f;
	bool bResult = false;

	bResult = m->GetAt( 1u, 2u, fValue );
	bResult = m->GetAt( 1u, 3u, fValue );
	bResult = m->SetAt( 1u, 0u, -3.2f );
	bResult = m->GetAt( 0u, 2u, fValue );

	for (uint32 i = 0; i < 2u; i++)
	{
		for (uint32 j = 0; j < 3u; j++)
		{
			std::cout << "i: " << i << " " << "j: " << j << " >> " << (*m)[i][j] << std::endl;
		}
	}

	m->Release();

	std::cin.get();
	return ( 0 );
}