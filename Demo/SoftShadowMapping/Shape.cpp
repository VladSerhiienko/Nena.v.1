#include "Shape.h"

Demo::Shape::Shape()
{
}

Demo::Shape::~Shape()
{
	if (N2PMap) delete[] N2PMap;

	PolygonList.clear();
	PointListOS.clear();
	NormalListOS.clear();
	TexcoordsList.clear();
}