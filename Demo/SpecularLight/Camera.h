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
		::Nena::Vector3 EyePosition;

		::FLOAT FieldOfView;
		::FLOAT AspectRatio;
		::FLOAT NearPlane;
		::FLOAT FarPlane;

		Demo::Camera::Camera()
		{
			EyeDisplacement.z = -10.f;
			//FocusPosition.y = 2.0f;

			FieldOfView = ::Nena::XM_PIDIV4;
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
			static ::Nena::Vector3 up = { 0.f, 1.f, 0.f }, displacement;
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

			EyePosition = FocusPosition + displacement;
			View = ::Nena::XMMatrixLookAtLH(
				EyePosition, FocusPosition, up
				);

			/*Framework::Output(
				"Demo::Camera::UpdateView() >> pitch %f yaw %f\n",
				pitchA, yawA
				);

			Framework::Output(
				"Demo::Camera::UpdateView() >> eye %f %f %f\n",
				eye.x, eye.y, eye.z
				);*/
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