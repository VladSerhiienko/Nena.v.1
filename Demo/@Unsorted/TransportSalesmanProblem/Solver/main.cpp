#include "bsctpsstorage.h"
using namespace Juniherz;

// Sample prototypes.
void Sample1( );

// Application startup.
int main( )
{
	// Launch samples hier.
	Sample1();

	// Prompt for the user input to exit.
	std::cin.get();
	// No errors. 
	return ( 0 );
}

// 1> Test storage obj.
void Sample1( )
{
	// Storage object.
	Storage32* storage = new Storage32( 4u, 4u );

	float32 prices[16u] = 
	{
		15.0f, 16.0f, 14.0f, 12.0f, 
		13.0f, 12.0f, 14.0f, 11.0f,
		12.0f, 13.0f, 16.0f, 15.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 
	};

	float32 supply[4u] = 
	{
		40.0f, 30.0f, 20.0f, 10.0f
	};

	float32 demand[4u] = 
	{
		40.0f, 20.0f, 10.0f, 30.0f
	};

	bool bResult = false;
	bResult = storage->SetPriceValues( 0u, 16u, prices );
	bResult = storage->SetSupplyValues( 0u, 4u, supply );
	bResult = storage->SetDemandValues( 0u, 4u, demand );

	// Delete storage object.
	storage->Release();
}