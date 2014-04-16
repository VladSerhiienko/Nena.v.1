#include <stdio.h>
#include <math.h>
#include "dfp.h"

void show( const char *tag,
           struct map_type *map )
{
    int i, j;

    printf( "<%s>", tag );
    for( i = 0; i < map->i_size; i++ )
    {
        for( j = 0; j < map->j_size; j++ )
        {
            printf( " %.10Lf ", (long double)*( *( map->memory + i ) + j ) );
        }
    }
    printf( "</%s>\n", tag );
}

void main()
{
    int i;
    struct map_type **points, *result;
    function *functions;

    functions = get_functions();
    points = get_start_points();
    for( i = 0; i < 5; i++ )
    {
    	printf("<func %d>\n", i + 1 );
    	show( "Start X", *( points + i ) );
        result = dfp( *( functions + i ),
                      *( points + i ),
                      0.0000100f,
                      0.0000100f,
                      500 );
        printf( "Value %.10Lf\n", (long double)( *( functions + i ) )( result ) );
        show( "X", result );
        printf("</func %d>\n\n", i + 1 );

    }
    free( points );
    free( functions );
	getchar();
}