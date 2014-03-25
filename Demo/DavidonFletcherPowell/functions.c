#include <math.h>
#include "functions.h"

__float128 func_1( struct map_type *map_x )
{
    return pow( * ( *( map_x->memory + 0 ) + 0 ) - ( * ( *( map_x->memory + 0 ) + 1 ) ), 2 )
           + powf( * ( *( map_x->memory + 0 ) + 0 ) + ( * ( *( map_x->memory + 0 ) + 1 ) ) - 10, 2 ) / 9;
}

__float128 func_2( struct map_type *map_x )
{
    return powf( 1 - * ( *( map_x->memory + 0 ) + 0 ), 2 )
           + 100 * powf( ( * ( *( map_x->memory + 0 ) + 1 ) - powf( * ( *( map_x->memory + 0 ) + 0 ), 2 ) ), 2 );
}

__float128 func_3( struct map_type *map_x )
{
    return powf( ( * ( *( map_x->memory + 0 ) + 0 ) - 3 ) / 100.00f, 2 )
           - ( * ( *( map_x->memory + 0 ) + 1 ) - * ( *( map_x->memory + 0 ) + 0 ) )
           + exp( 20 * ( * ( *( map_x->memory + 0 ) + 1 ) - * ( *( map_x->memory + 0 ) + 0 ) ) );
}

__float128 func_4( struct map_type *map_x )
{
    return powf( * ( *( map_x->memory + 0 ) + 0 ) + 10.00f * powf( * ( *( map_x->memory + 0 ) + 1 ), 2 ), 2 )
           + 5 * powf( * ( *( map_x->memory + 0 ) + 2 ) - * ( *( map_x->memory + 0 ) + 3 ), 2 )
           + powf( * ( *( map_x->memory + 0 ) + 1 ) - 2 * * ( *( map_x->memory + 0 ) + 2 ), 4 )
           + 10 * powf( * ( *( map_x->memory + 0 ) + 0 ) - * ( *( map_x->memory + 0 ) + 3 ), 4 );
}

__float128 func_5( struct map_type *map_x )
{
    int i;
    __float128 result, numerator, denumerator;
    static const __float128 a[7] =
    {
        0.000000f,
        0.000428f,
        0.001000f,
        0.001610f,
        0.002090f,
        0.003480f,
        0.005250f
    };
    static const __float128 b[7] =
    {
        7.3910000f,
        11.1800000f,
        16.4400000f,
        16.2000000f,
        22.2000000f,
        24.0200000f,
        31.3200000f
    };

    result = 0;
    for( i = 0; i < 7; i++ )
    {
        numerator = powf( * ( *( map_x->memory + 0 ) + 0 ), 2 ) + powf( * ( *( map_x->memory + 0 ) + 1 ), 2 ) * a[i] + powf( * ( *( map_x->memory + 0 ) + 2 ), 2 ) * powf( a[i], 2 );
        denumerator = 1 + powf( * ( *( map_x->memory + 0 ) + 3 ), 2 ) * a[i];
        result += powf( ( numerator / denumerator - b[i] ) / b[i]  , 2 );
    }
    return result * powf( 10.00f, 4 );
}

struct map_type **get_start_points()
{
    struct map_type **result;
    result = ( struct map_type ** )malloc( sizeof( struct map_type * ) * 5 );

    *( result + 0 ) = allocate( 1, 2 );
    *( *( ( *( result + 0 ) )->memory + 0 ) + 0 ) = 0.00000f;
    *( *( ( *( result + 0 ) )->memory + 0 ) + 1 ) = 1.00000f;

    *( result + 1 ) = allocate( 1, 2 );
    *( *( ( *( result + 1 ) )->memory + 0 ) + 0 ) = -1.20000f;
    *( *( ( *( result + 1 ) )->memory + 0 ) + 1 ) = 1.00000f;

    *( result + 2 ) = allocate( 1, 2 );
    *( *( ( *( result + 2 ) )->memory + 0 ) + 0 ) = 0.00000f;
    *( *( ( *( result + 2 ) )->memory + 0 ) + 1 ) = -1.0000f;

    *( result + 3 ) = allocate( 1, 4 );
    *( *( ( *( result + 3 ) )->memory + 0 ) + 0 ) = 3.00000f;
    *( *( ( *( result + 3 ) )->memory + 0 ) + 1 ) = -1.00000f;
    *( *( ( *( result + 3 ) )->memory + 0 ) + 2 ) = 0.00000f;
    *( *( ( *( result + 3 ) )->memory + 0 ) + 3 ) = 1.00000f;

    *( result + 4 ) = allocate( 1, 4 );
    *( *( ( *( result + 4 ) )->memory + 0 ) + 0 ) = 2.700000f;
    *( *( ( *( result + 4 ) )->memory + 0 ) + 1 ) = 90.000000f;
    *( *( ( *( result + 4 ) )->memory + 0 ) + 2 ) = 1500.00000f;
    *( *( ( *( result + 4 ) )->memory + 0 ) + 3 ) = 10.000000f;

    return result;
}

function *get_functions()
{
    function *result;
    result = ( function * )malloc( sizeof( function ) * 5 );
    *( result + 0 ) = &func_1;
    *( result + 1 ) = &func_2;
    *( result + 2 ) = &func_3;
    *( result + 3 ) = &func_4;
    *( result + 4 ) = &func_5;
    return result;
}