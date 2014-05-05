#include "app.precompiled.h"
#include "itv.oasis.h"

struct SomeClass
{
	~SomeClass() { OutputDebugStringA("SomeClass::~SomeClass()\n"); }
};

int main(int argc, char *argv [])
{
	InteractiveTV::Oasis::GetForCurrentThread()->Run();
	return EXIT_SUCCESS;
}