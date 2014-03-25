#pragma once

#include "matrix.h"

typedef __float128( *function )( struct map_type * );
struct map_type *dfp( function,
                      struct map_type *,
                      __float128,
                      __float128,
                      const unsigned int );