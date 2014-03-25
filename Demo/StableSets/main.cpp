#include <iostream>
#include <Nena\StepTimer.h>

#include "Graph.h"

int main(int argc, char *argv [])
{
	Demo::Graph::HResult hr = S_OK;

	Demo::Graph g;
	hr = g.ReadFromFile("graph2.txt", FALSE);

	g.BuildTrees();
	g.GenerateStableSets();
	g.PrintStableSets();

	getchar(); 
	return EXIT_SUCCESS;
}