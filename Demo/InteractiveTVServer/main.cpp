#include "InteractiveTV.h"

void EnableCrt()
{
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
}

int main(int argc, char *argv [])
{
	using namespace Nena::Video;
	using namespace Nena::Video::Perc;
	using namespace Presentation;

	EnableCrt();

	printf("-------------------------------------------\n");
	printf("> InteractiveTV::TrackingService\n");
	printf("-------------------------------------------\n");
	printf("Launching, please wait...\n");

	InteractiveTV::Get()->Init();
	InteractiveTV::Get()->Launch();
	if (!InteractiveTV::Get()->IsValid())
		goto __label_exit_error;

	printf("-------------------------------------------\n");
	printf("> Press and hold escape for 3 seconds to quit\n");

	while (TRUE) 
	if (GetAsyncKeyState(VK_ESCAPE))
	if (GetConsoleWindow() == GetForegroundWindow()) break;
	else Sleep(3000);

	printf("-------------------------------------------\n");
	printf("> Cancelling...\n");

	InteractiveTV::Get()->Close();

	printf("-------------------------------------------\n");
	printf("> Cancelled...\n");

	goto __label_exit_main;

	__label_exit_error:

	printf("-------------------------------------------\n");
	printf("> Initialization failed\n");

	__label_exit_main:

	printf("-------------------------------------------\n");
	printf("> Press any key to exit\n");
	printf("-------------------------------------------\n");

	getchar();
	return EXIT_SUCCESS;
}