#pragma once

#include <Nena\Math.h>

namespace Demo
{
	struct Transform
	{
		::Nena::Quaternion Orientation;
		::Nena::Quaternion Rotation;
		::Nena::Vector3 Translation;
	};
}