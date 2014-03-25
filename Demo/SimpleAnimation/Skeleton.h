#pragma once

#include <vector>
#include "Transform.h"

namespace Demo
{
	struct Skeleton
	{
		struct Joint
		{
			typedef ::std::vector<Joint *> Vector;
			typedef Vector::iterator Iterator;
			typedef int Bool;

			friend Skeleton;

			Transform TransformOS;
			Transform TransformWS;

			Joint *Original = nullptr;
			Joint *Parent = nullptr;
			Vector Kids;

			::std::string Name;

			inline Joint::Bool IsAffector() { return !Kids.size(); }
			inline Joint::Bool IsHandle() { return !Parent; }

#pragma region Accessors

			inline ::Nena::Matrix GetTranslateWSM()
			{
				return ::Nena::XMMatrixTranslationFromVector(TransformWS.Translation);
			}

			inline ::Nena::Matrix GetTranslateOSM()
			{
				return ::Nena::XMMatrixTranslationFromVector(TransformOS.Translation);
			}

			inline ::Nena::Vector3 GetTranslateWSV()
			{
				return TransformWS.Translation;
			}

			inline ::Nena::Vector3 GetTranslateOSV()
			{
				return TransformOS.Translation;
			}

			inline ::Nena::Vector3 GetTranslateWSW()
			{
				return TransformWS.Translation;
			}

			inline ::Nena::Vector3 GetTranslateOSW()
			{
				using namespace ::Nena;
				using namespace ::DirectX;
				return TransformOS.Translation;
			}

			inline ::Nena::Matrix GetRotateTransposeWSM()
			{
				::Nena::Matrix rotate = ::Nena::XMMatrixRotationQuaternion(TransformWS.Rotation);
				rotate.Transpose();
				return rotate;
			}

			inline ::Nena::Matrix GetRotateTransposeOSM()
			{
				::Nena::Matrix rotate = ::Nena::XMMatrixRotationQuaternion(TransformOS.Rotation);
				rotate.Transpose();
				return rotate;
			}

			inline ::Nena::Matrix GetRotateWSM()
			{
				return ::Nena::XMMatrixRotationQuaternion(TransformWS.Rotation);
			}

			inline ::Nena::Matrix GetRotateOSM()
			{
				return ::Nena::XMMatrixRotationQuaternion(TransformOS.Rotation);
			}

			inline ::Nena::Quaternion GetRotateWSQ()
			{
				return TransformWS.Rotation;
			}

			inline ::Nena::Quaternion GetRotateOSQ()
			{
				return TransformOS.Rotation;
			}

#pragma endregion

			::Nena::Matrix GetTransformMatrixWS();
			::Nena::Matrix GetTransformMatrixInverseWS();

			~Joint()
			{
				Original = nullptr;
				Parent = nullptr;
				Kids.clear();
			}

		private:

			void UpdateTransformWS();
			void UpdateKinderTransformWS();

			void UpdateTransformOS(::FLOAT time);
			void UpdateKinderTransformOS(::FLOAT time);

		};

		struct Handle : public Joint
		{
			void UpdateTransformWS();
			void UpdateTransformOS(::FLOAT time);

			inline void Update(::FLOAT time)
			{
				Handle::UpdateTransformOS(time);
				Handle::UpdateTransformWS();
			}
		};

		Joint::Vector Joints;
		Handle *Root = nullptr;

		~Skeleton()
		{
			if (Joints.size())
			{
				for (auto p : Joints) { delete p; p = nullptr; }
				Joints.clear();
			}
		}

		Skeleton() { }

	private:

		Skeleton(Skeleton const &);

	};

	typedef Skeleton::Joint SkeletonJoint;
	typedef Skeleton::Handle SkeletonHandle;
}