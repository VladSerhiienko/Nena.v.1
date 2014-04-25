#pragma once

#include <Nena\Math.h>

namespace Demo
{
	struct Transform
	{
		::Nena::Quaternion OrientationQ;
		::Nena::Quaternion RotationQ;
		::Nena::Vector3 Translation;
		::Nena::Vector3 RotationEA;
		::Nena::Matrix World;

		void EvaluateWorld()
		{
			World = ::Nena::Matrix::CreateTranslation(Translation);
			World *= ::Nena::Matrix::CreateFromQuaternion(RotationQ);

			/*World = ::Nena::Matrix::CreateFromQuaternion(RotationQ);
			World *= ::Nena::Matrix::CreateTranslation(Translation);*/

			/*World = ::Nena::Matrix::CreateTranslation(Translation);
			World *= ::Nena::Matrix::CreateRotationX(RotationEA.x);
			World *= ::Nena::Matrix::CreateRotationY(RotationEA.y);
			World *= ::Nena::Matrix::CreateRotationZ(RotationEA.z);*/
		}
	};
}