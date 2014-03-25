#include "Transform.h"
#include "TransformSolver.h"

Demo::Transform::Transform() : ChannelArray()
{
}

::FLOAT Demo::Transform::Curve::ResolveAt(::FLOAT t, _In_::BOOL rec)
{
	using namespace ::Nena;

	/*if (!rec) 
		Platform::DebugTrace(
			"%s::%s [%u] > ResolveAt(%f)\n", 
			HName.c_str(), Name.c_str(), 
			(UINT32) Keys.size(),
			t
			);*/

	if (Right)
	{
		if (Right->TV.x >= t)
		{
			if (Left)
			{
				if (Left->TV.x <= t)
				{
					::FLOAT value; 
					TransformSolver::Result result;

					result = TransformSolver::SolveSegmentEst(
						Left->TV, Left->OT,
						Right->TV, Right->IT, 
						t, value
						);

					//Platform::DebugTrace("\t-> solve ([%f %f] => %f) -> %f\n", Left->TV.x, Right->TV.x, t, value);

					return value;
				}
				else
				{
					Right = Left;
					Left = Left->PK;
					//Platform::DebugTrace("\t-> shift << (back) \n");

					return ResolveAt(t, TRUE);
				}
			}
			else
			{
				//Platform::DebugTrace("\t-> under (%f => %f) -> %f\n", Right->TV.x, t, Right->TV.y);
				return Right->TV.y;
			}
		}
		else
		{
			Left = Right;
			Right = Right->NK;

			//Platform::DebugTrace("\t-> shift >> (forth) \n");

			return ResolveAt(t, TRUE);
		}
	}
	else
	{
		//Platform::DebugTrace("\t-> over (%f => %f) -> %f\n", Left->TV.x, t, Left->TV.y);
		return Left->TV.y;
	}
}

void Demo::Transform::ResolveSS_Animated(_In_::FLOAT time)
{
	if (ChannelArray.Sx) Scale.x = ChannelArray.Sx->ResolveAt(time);
	if (ChannelArray.Sy) Scale.y = ChannelArray.Sy->ResolveAt(time);
	if (ChannelArray.Sz) Scale.z = ChannelArray.Sz->ResolveAt(time);
}

void Demo::Transform::ResolveSR_Animated(_In_::FLOAT time)
{
	if (ChannelArray.Rx || ChannelArray.Ry || ChannelArray.Rz)
	{
		RotationEA.x = ChannelArray.Rx ? ChannelArray.Rx->ResolveAt(time) : RotationEA.x;
		RotationEA.y = ChannelArray.Ry ? ChannelArray.Ry->ResolveAt(time) : RotationEA.y;
		RotationEA.z = ChannelArray.Rz ? ChannelArray.Rz->ResolveAt(time) : RotationEA.z;

		Rotation = ::Nena::XMQuaternionRotationRollPitchYaw(
			RotationEA.x, RotationEA.y, RotationEA.z
			);
	}
}

void Demo::Transform::ResolveST_Animated(_In_::FLOAT time)
{
	if (ChannelArray.Tx) Translation.x = ChannelArray.Tx->ResolveAt(time);
	if (ChannelArray.Ty) Translation.y = ChannelArray.Ty->ResolveAt(time);
	if (ChannelArray.Tz) Translation.z = ChannelArray.Tz->ResolveAt(time);
}

