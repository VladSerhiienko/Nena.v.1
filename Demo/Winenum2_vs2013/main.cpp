#include "RemoteControl.h"

int main(int argc, CHAR* argv[])
{
	Demo::RemoteControl remote;

	std::string arg = argc >= 2 ? argv[1] : "-emit-to-focus";
	//std::string arg = argc >= 2 ? argv[1] : "-print-all";

	if (arg == "-print-all")
		remote.WriteToplevelWindows();
	else
		remote.ReadFocusData(),
		remote.UpdateFocusHandles();

	std::cout << "Press any key to exit" << std::endl;
	std::cin.get();
	return EXIT_SUCCESS;
}