#pragma once

typedef long double __float128;

struct map_type
{
    unsigned short i_size, j_size, real_size;
    __float128 **memory;
};

struct map_type *get_inverse( struct map_type * );
__float128 get_determinant( struct map_type * );
void memset_map( struct map_type *,
                 __float128 );
struct map_type *allocate( unsigned ,
                           unsigned );
void deallocate( struct map_type * );
__float128 get_euclidean_distance( struct map_type * );
struct map_type *clone( struct map_type * );
void memcpy_map( struct map_type *,
                 struct map_type * );
struct map_type *transposition( struct map_type * );
struct map_type *get_identity_matrix( unsigned short );
struct map_type *subtraction( struct map_type *,
                              struct map_type * );
struct map_type *addition( struct map_type *,
                           struct map_type * );
struct map_type *multiplicate( struct map_type *,
                               struct map_type * );
struct map_type *multiplicate_on_value( __float128 value,
                                        struct map_type * );