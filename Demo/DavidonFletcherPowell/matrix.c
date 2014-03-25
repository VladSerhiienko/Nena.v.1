#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include "other.h"
#include "matrix.h"

struct map_type *get_inverse( struct map_type *map )
{
    struct map_type *result;
    if( map->i_size == 1
        && map->j_size == 1 )
    {
        result = allocate( 2, 2 );
        result->i_size = 1;
        result->j_size = 1;
        __float128 value = *( *( map->memory + 0 ) + 0 );
        *( *( result->memory + 0 ) + 0 ) = value == 0 ? 0 : powf( value, -1 );
    }
    else
    {
        __float128 determinant = get_determinant( map );
        result = multiplicate_on_value( powf( determinant, -1 ), map );
    }
    return result;
}

__float128 get_determinant( struct map_type *map )
{
    __float128 result;

    result = 0;
    switch( map->i_size )
    {
        case 2:
            result = *( *( map->memory + 0 ) + 0 ) * ( *( * ( map->memory + 1 ) + 1 ) )
                     - ( *( * ( map->memory + 1 ) + 0 ) ) * ( *( * ( map->memory + 0 ) + 1 ) );
            break;

            /*
            case 3:
            case 4:
            */
    }
    return result;
}

void memset_map( struct map_type *map,
                 __float128 value )
{
    int i, j;
    for( i = 0; i < map->real_size; i++ )
    {
        for( j = 0; j < map->real_size; j++ )
        {
            *( *( map->memory + i ) + j ) = value;
        }
    }
}

struct map_type *allocate( unsigned i_size,
                           unsigned j_size )
{
    int i;
    struct map_type *result;
    if( !i_size
        || !j_size )
    {
        return NULL;
    }

    result = ( struct map_type * )malloc( sizeof( struct map_type ) );
    result->i_size = i_size;
    result->j_size = j_size;
    result->real_size = i_size > j_size ? i_size : j_size;
    result->memory = ( __float128 ** )malloc( sizeof( __float128 * ) * result->real_size );
    for( i = 0; i < result->real_size; i++ )
    {
        *( result->memory + i ) = ( __float128 * )malloc( sizeof( __float128 ) * result->real_size );
    }
    memset_map( result, 0 );
    return result;
}

void deallocate( struct map_type *map )
{
    int i;
    for( i = 0; i < map->real_size; i++ )
    {
        free( *( map->memory + i ) );
    }
    free( map->memory );
    free( map );
}

__float128 get_euclidean_distance( struct map_type *map )
{
    int i;
    __float128 result;
    result = 0;

    if( map->i_size == 1 )
    {
        for( i = 0; i < map->j_size; i++ )
        {
            result += fabs(*( *( map->memory + 0 ) + i ));
        }
    }
    else if( map->j_size == 1 )
    {
        for( i = 0; i < map->i_size; i++ )
        {
            result += fabs(*( *( map->memory + i ) + 0 ));
        }
    }
    return result;
}

struct map_type *clone( struct map_type *map )
{
    int i, j;
    struct map_type *result;
    result = allocate( map->i_size, map->j_size );
    for( i = 0; i < map->real_size; i++ )
    {
        for( j = 0; j < map->real_size; j++ )
        {
            *( *( result->memory + i ) + j ) = *( *( map->memory + i ) + j );
        }
    }
    return result;
}

void memcpy_map( struct map_type *from,
                 struct map_type *to )
{
    int i, j;
    for( i = 0; i < from->i_size; i++ )
    {
        for( j = 0; j < from->j_size; j++ )
        {
            *( *( to->memory + i ) + j ) = *( *( from->memory + i ) + j );
        }
    }
}

struct map_type *transposition( struct map_type *map )
{
    int i, j;
    struct map_type *result;
    result = clone( map );
    for( i = 0; i < result->real_size; i++ )
    {
        for( j = 0; j < result->real_size; j++ )
        {
            *( *( result->memory + i ) + j ) = *( *( map->memory + j ) + i );
        }
    }
    result->i_size = map->j_size;
    result->j_size = map->i_size;
    return result;
}

struct map_type *get_identity_matrix( unsigned short size )
{
    int i;
    struct map_type *result;
    result = allocate( size, size );
    for( i = 0; i < size; i++ )
    {
        *( *( result->memory + i ) + i ) = 1;
    }
    return result;
}

struct map_type *subtraction( struct map_type *map,
                              struct map_type *map_2 )
{
    struct map_type *tmp, *result;
    tmp = multiplicate_on_value( -1, map_2 );
    result = addition( map, tmp );
    deallocate( tmp );
    return result;
}

struct map_type *addition( struct map_type *map,
                           struct map_type *map_2 )
{
    int i, j;
    struct map_type *result;
    result = allocate( map->i_size, map->j_size );
    if( map_2->i_size == 1
        && map_2->j_size == 1 )
    {
        for( i = 0; i < result->i_size; i++ )
        {
            for( j = 0; j < result->j_size; j++ )
            {
                *( *( result->memory + i ) + j ) = *( *( map->memory + i ) + j ) + ( *( *( map_2->memory + 0 ) + 0 ) );
            }
        }
    }
    else if( map->i_size == map_2->i_size
             && map->j_size == map_2->j_size )
    {
        for( i = 0; i < result->i_size; i++ )
        {
            for( j = 0; j < result->j_size; j++ )
            {
                *( *( result->memory + i ) + j ) = *( *( map->memory + i ) + j ) + ( *( *( map_2->memory + i ) + j ) );
            }
        }
    }
    else
    {
        for( i = 0; i < result->i_size; i++ )
        {
            for( j = 0; j < result->j_size; j++ )
            {
                *( *( result->memory + i ) + j ) = *( *( map->memory + i ) + j ) + ( *( *( map_2->memory + j ) + i ) );
            }
        }
    }
    return result;
}

struct map_type *multiplicate( struct map_type *map,
                               struct map_type *map_2 )
{
    int i, j, k;
    __float128 sum_tmp;
    struct map_type *result;

    if( ( map->i_size == 1 && map->j_size == 1 )
        && ( map_2->i_size > 1 || map_2->j_size > 1 ) )
    {
        result = multiplicate_on_value( *( *( map->memory + 0 ) + 0 ), map_2 );
    }
    else
    {
        result = allocate( map->i_size, map_2->j_size );
        for( i = 0; i < map->i_size; i++ )
        {
            for( j = 0; j < map_2->j_size; j++ )
            {
                for( sum_tmp = 0, k = 0; k < map->j_size; k++ )
                {
                    sum_tmp += *( *( map->memory + i ) + k ) * ( *( *( map_2->memory + k ) + j ) );
                }
                *( *( result->memory + i ) + j ) = sum_tmp;
            }
        }
    }
    return result;
}

struct map_type *multiplicate_on_value( __float128 value,
                                        struct map_type *map )
{
    int i, j;
    struct map_type *result;
    result = clone( map );
    for( i = 0; i < result->i_size; i++ )
    {
        for( j = 0; j < result->j_size; j++ )
        {
            *( *( result->memory + i ) + j ) *= value;
        }
    }
    return result;
}