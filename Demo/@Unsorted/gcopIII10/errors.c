
#include <stdio.h>
#include "genocop.h"

/*
**
** void  error_exit(char error_text[])
**
** Prints out the supplied error string to file/console,
**
** and aborts the program.
**
**
*/
void
error_exit(char error_text [])
{
	printf("\nERROR!\n%s\n", error_text);
	printf("Program execution terminated.\n");
	printf("Please report bugs to zbyszek@uncc.edu or gnazhiya@uncc.edu\n");
	printf("The WWW homepage is at :\n");
	printf("http://www.coe.uncc.edu/~gnazhiya/gchome.html\n");
	getchar();
	exit(1);
}
/**************************************************************************/
