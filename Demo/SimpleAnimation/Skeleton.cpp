#include "Skeleton.h"
#include <Nena\PlatformAssist.h>

::Nena::Matrix Demo::Skeleton::Joint::GetTransformMatrixWS()
{
	::Nena::Matrix rotate = ::Nena::XMMatrixRotationQuaternion(TransformWS.Rotation);
	::Nena::Matrix translate = ::Nena::XMMatrixTranslationFromVector(TransformWS.Translation);
	::Nena::Matrix transform = ::Nena::XMMatrixMultiply(rotate, translate);

	return transform;
}

::Nena::Matrix Demo::Skeleton::Joint::GetTransformMatrixInverseWS()
{
	::Nena::Matrix rotate = ::Nena::XMMatrixRotationQuaternion(TransformWS.Rotation);
	rotate = ::Nena::XMMatrixTranspose(rotate);
	::Nena::Matrix translate = ::Nena::XMMatrixTranslationFromVector(
		::Nena::XMVectorNegate(::Nena::XMVector3Transform(
			TransformWS.Translation, rotate
			))
		);

	::Nena::Matrix transform = ::Nena::XMMatrixMultiply(rotate, translate);
	return transform;
}

void Demo::Skeleton::Joint::UpdateKinderTransformWS()
{
	Joint::Iterator jvi;
	for (jvi = Joint::Kids.begin(); jvi != Joint::Kids.end(); jvi++)
		(*jvi)->UpdateTransformWS();
}

void Demo::Skeleton::Joint::UpdateKinderTransformOS(::FLOAT time)
{
	Joint::Iterator jvi;
	for (jvi = Joint::Kids.begin(); jvi != Joint::Kids.end(); jvi++)
		(*jvi)->UpdateTransformOS(time);
}

void Demo::Skeleton::Joint::UpdateTransformWS()
{
	TransformWS.Translation = Parent->TransformWS.Translation;

	TransformWS.Translation += ::Nena::XMVector3Rotate(
		TransformOS.Translation, Parent->TransformWS.Rotation
		);

	TransformWS.Rotation = ::Nena::XMQuaternionMultiply(
		TransformOS.Rotation, Nena::XMQuaternionMultiply(
			TransformOS.Orientation, Parent->TransformWS.Rotation
			)
			);

	/*Nena::Platform::DebugTrace(
		"%s => (%2.2f %2.2f %2.2f)\n", Name.c_str(),
		Joint::TransformWS.Translation.x,
		Joint::TransformWS.Translation.y,
		Joint::TransformWS.Translation.z
		);*/

	UpdateKinderTransformWS();
}

void Demo::Skeleton::Joint::UpdateTransformOS(::FLOAT time)
{
	TransformOS.ResolveSX(time);
	UpdateKinderTransformOS(time);
}

void Demo::Skeleton::Handle::UpdateTransformWS()
{
	Joint::TransformWS.Translation = Joint::TransformOS.Translation;
	Joint::TransformWS.Rotation = ::Nena::XMQuaternionMultiply(
		Joint::TransformOS.Rotation, Joint::TransformOS.Orientation
		);

	/*Nena::Platform::DebugTrace(
		"%s => (%2.2f %2.2f %2.2f)\n", Name.c_str(),
		Joint::TransformWS.Translation.x,
		Joint::TransformWS.Translation.y,
		Joint::TransformWS.Translation.z
		);*/

	Joint::UpdateKinderTransformWS();
}

void Demo::Skeleton::Handle::UpdateTransformOS(::FLOAT time)
{
	Joint::UpdateTransformOS(time);
}