#pragma once

#include <Nena\Math.h>
#include <Nena\DirectXTypes.h>

namespace Demo
{
	struct Camera
	{
		::Nena::Matrix View;
		::Nena::Matrix Projection;

		::Nena::Vector3 FocusPosition;
		::Nena::Vector3 EyeDisplacement;

		::FLOAT FieldOfView;
		::FLOAT AspectRatio;
		::FLOAT NearPlane;
		::FLOAT FarPlane;

		Demo::Camera::Camera()
		{
			EyeDisplacement.z = 4;

			FieldOfView = ::Nena::XM_PIDIV2;
			AspectRatio = 1.3333f;
			NearPlane = 0.010f;
			FarPlane = 100.00f;

			UpdateView({ 0, 0 }, 0);
			UpdateProjection();
		}

		void Demo::Camera::UpdateView(
			::Nena::Vector2 delta,
			::FLOAT zoom
			)
		{
			static ::Nena::Vector3 up = { 0.f, 1.f, 0.f }, eye, displacement;
			static ::Nena::Quaternion pitchQ, yawQ;
			static ::FLOAT pitchA, yawA;

			delta *= 0.005f;
			zoom *= 0.005f;
			pitchA += delta.y;
			yawA += delta.x;

			pitchQ.w = ::cosf(pitchA * .50f);
			pitchQ.x = ::sin(pitchA * .50f);
			yawQ.w = ::cosf(yawA *.50f);
			yawQ.y = ::sin(yawA *.50f);

			EyeDisplacement.z += zoom;
			displacement = ::Nena::XMVector3Rotate(
				::Nena::XMVector3Rotate(
					EyeDisplacement, pitchQ
					), 
				yawQ
				);

			eye = FocusPosition + displacement;
			View = ::Nena::XMMatrixLookAtLH(
				eye, FocusPosition, up
				);
		}

		void Demo::Camera::UpdateProjection(
			)
		{
			Projection = ::Nena::XMMatrixPerspectiveFovLH(
				FieldOfView, AspectRatio, NearPlane, FarPlane
				);
		}

	};
}