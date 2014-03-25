#include "bsctpsstorage.h"
using namespace Juniherz;

int main( )
{
	// Create storage object.
	Storage32* storage = new Storage32( 2u, 3u );



	// Delete storage object.
	storage->Release();

}