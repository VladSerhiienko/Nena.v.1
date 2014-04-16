#include "mtx.h"
using namespace Demo;



void main()
{
	matrix_nxn m1, m2;
	m1.alloc(10, 5);
	m2.alloc(5, 9);

	math::zero(m1);
	math::zero(m2);
	math::identity(m1);
	math::identity(m2);
	math::trace(m1);
	math::trace(m2);

	matrix_nxn m1m2;
	math::mul(m1, m2, m1m2);
	math::trace(m1m2);

}