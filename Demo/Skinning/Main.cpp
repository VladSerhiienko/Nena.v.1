#include "SceneFileContent.h"

int main(int arc, char *arv [])
{
	Demo::SceneFileContent content;

	content.ReadFile(
		"D:/Development/Maya/Scenes/shape-6-skinned.ma.xml",
		"D:/Development/Maya/Scenes/shape-6-posed.ma.xml"
		);

	getchar();
	return 0;
} 