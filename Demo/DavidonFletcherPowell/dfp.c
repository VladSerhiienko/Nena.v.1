#include <stdio.h>
#include "dfp.h"

struct map_type *first_derivatives( function func,
        struct map_type *map_x )
{
    int i, j;
    __float128 xt, hm, fp, fm, h;
    struct map_type *result;

    result = allocate( map_x->i_size, map_x->j_size );
    for( i = 0; i < 1; i++ )
    {
        for( j = 0; j < result->j_size; j++ )
        {
            xt = *( *( map_x->memory + i ) + j );
            h = 0.0100000f * ( 1.0000000f + fabs( xt ) );
            *( *( map_x->memory + i ) + j ) = xt + h;
            fp = func( map_x );
            *( *( map_x->memory + i ) + j ) = xt - h;
            fm = func( map_x );
            *( *( map_x->memory + i ) + j ) = xt;
            *( *( result->memory + i ) + j ) = ( fp - fm ) / 2.000000f / h;
        }
    }
    return result;
}

__float128 my_fx_ex( function func,
                            struct map_type *map_x,
                            __float128 lambda,
                            struct map_type *map_delta_x )
{
    int i;
    struct map_type *x_tmp;
    __float128 result;

    x_tmp = allocate( map_x->i_size, map_x->j_size );
    for( i = 0; i < map_x->j_size; i++ )
    {
        *( *( x_tmp->memory + 0 ) + i ) = *( *( map_x->memory + 0 ) + i ) + lambda * ( *( * ( map_delta_x->memory + 0 ) + i ) );
    }
    result = func( x_tmp );
    deallocate( x_tmp );
    return result;
}

__float128 line_search( function func,
                               struct map_type *map_x,
                               __float128 lambda,
                               struct map_type *map_d )
{
    static const unsigned int max_iter = 4000;
    static const __float128 toler = 0.00001000f;
    __float128 h, f0, fp, fm, diff, deriv1, deriv2;
    unsigned int iter;

    for( iter = 0;; iter++ )
    {
        if( iter > max_iter )
        {
            lambda = 0;
            break;
        }

        h = 0.0010000f * ( 1.0000000f + fabs( lambda ) );
        f0 = my_fx_ex( func, map_x, lambda, map_d );
        fp = my_fx_ex( func, map_x, lambda + h, map_d );
        fm = my_fx_ex( func, map_x, lambda - h, map_d );

        deriv1 = ( fp - fm ) / 2.0000000f / h;
        deriv2 = ( fp - 2.0000000f * f0 + fm ) / powf( h, 2 );
        diff = deriv1 / deriv2;
        lambda -= diff;
        if( fabs( diff ) < toler )
        {
            break;
        }
    }
    return lambda;
}

struct map_type *dfp( function func,
                      struct map_type *map_x,
                      __float128 grad_toler,
                      __float128 fx_toler,
                      const unsigned int max_iter )
{
    unsigned int iter;
    __float128 lambda, result_func_value, temp_func_value;
    struct map_type *result, *b, *grad1, *tmp, *s, *grad2,
            *g, *tmp2, *tmp3, *d, *x1, *x2, *x3, *x4, *x5;

    result = NULL;
    result_func_value = func( map_x );

    b = get_identity_matrix( map_x->j_size );
    grad1 = first_derivatives( func, map_x );
    tmp = transposition( grad1 );
    deallocate( grad1 );
    grad1 = tmp;
    for( iter = 0 ; iter < max_iter; iter++ )
	{
        tmp = multiplicate_on_value( -1, b );
        s = multiplicate( tmp, grad1 );
        deallocate( tmp );

        tmp = transposition( s );
        tmp2 = multiplicate_on_value( powf( get_euclidean_distance( s ), -1 ), tmp );
        deallocate( s );
        deallocate( tmp );
        s = tmp2;

        lambda = 1;
        lambda = line_search( func, map_x, lambda, s );
        d = multiplicate_on_value( lambda, s );

        tmp = addition( map_x, d );
        deallocate( d );
        deallocate( map_x );
        map_x = tmp;
        temp_func_value = func( map_x );
        if( result_func_value > temp_func_value )
        {
            iter = 0;
            result_func_value = temp_func_value;
            if( result != NULL )
            {
                deallocate( result );
            }
            result = clone( map_x );
        }

        grad2 = first_derivatives( func, map_x );
        tmp = transposition( grad2 );
        deallocate( grad2 );
        grad2 = tmp;
        g = subtraction( grad2, grad1 );
        deallocate( grad1 );
        grad1 = grad2;
        if( get_euclidean_distance( grad1 ) < grad_toler )
        {
            /// TODO: Нужно очистить память
            break;
        }

        tmp = transposition( s );
        x1 = multiplicate( s, tmp );
        x2 = multiplicate( s, g );
        deallocate( s );
        deallocate( tmp );

        tmp = multiplicate_on_value( lambda, x1 );
        tmp2 = get_inverse( x2 );
        tmp3 = multiplicate( tmp, tmp2 );
        deallocate( tmp );
        tmp = addition( b, tmp3 );
        deallocate( x1 );
        deallocate( x2 );
        deallocate( tmp2 );
        deallocate( tmp3 );
        deallocate( b );
        b = tmp;

        x3 = multiplicate( b, g );
        tmp = transposition( b );
        x4 = multiplicate( tmp, g );
        deallocate( tmp );

        tmp = transposition( g );
        tmp2 = multiplicate( tmp, b );
        x5 = multiplicate( tmp2, g );
        deallocate( g );
        deallocate( tmp );
        deallocate( tmp2 );

        tmp = transposition( x4 );
        tmp2 = multiplicate( x3, tmp );
        deallocate( tmp );
        tmp = get_inverse( x5 );
        tmp3 = multiplicate( tmp, tmp2 );
        deallocate( tmp );

        tmp = subtraction( b, tmp3 );
        deallocate( b );
        b = tmp;

        deallocate( tmp2 );
        deallocate( tmp3 );
        deallocate( x3 );
        deallocate( x4 );
        deallocate( x5 );
    }
    return result;
}