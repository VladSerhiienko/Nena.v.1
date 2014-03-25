#pragma once


#include "ShapeRenderer.h"

namespace Demo
{
	struct PlanetSatellites;

	struct SatelliteTransform
	{
		friend PlanetSatellites;

		typedef std::vector<SatelliteTransform> Vector;
		typedef Vector::iterator Iterator;

		::Nena::Vector3 PlanetPositionWS;

		::Nena::Vector3 SatellitePositionTS;
		::Nena::Vector3 SatellitePositionWS;

		::Nena::Matrix TransformXOS;
		::Nena::Matrix TransformXWS;
		::Nena::Matrix OrbitTransformXOS;
		::Nena::Matrix OrbitTransformXWS;

		::FLOAT YVelocity = 0.0f;
		::FLOAT YPostAngle = 0.0f;
		::FLOAT YAngle = 0.0f;
		::FLOAT ZAngle = 0.0f;

		void Initialize()
		{
			using namespace ::Nena;

			::BOOL ok;

			ok = FALSE;
			while (!ok)
				if (YAngle >= XM_2PI) YAngle -= XM_2PI;
				else if (YAngle <= -XM_2PI) YAngle += XM_2PI;
				else ok = TRUE;

			ok = FALSE;
			while (!ok)
				if (YVelocity >= XM_2PI) YVelocity -= XM_2PI;
				else if (YVelocity <= -XM_2PI) YVelocity += XM_2PI;
				else ok = TRUE;

		}

	};

	struct PlanetSatellites
	{
		Shape Content;
		ShapeRenderer Renderer;

		Shape OrbitContent;
		ShapeRenderer OrbitRenderer;

		SatelliteTransform::Vector Transforms;

		void Update(
			_In_::FLOAT elapsedTime,
			_In_::FLOAT totalTime
			)
		{
			using namespace ::Nena;

			for (auto &x : Transforms)
			{
				x.YAngle += x.YVelocity * elapsedTime;
				if (x.YAngle >= XM_2PI) x.YAngle -= XM_2PI;
				else if (x.YAngle <= -XM_2PI) x.YAngle += XM_2PI;

				x.TransformXWS = x.TransformXOS;
				x.OrbitTransformXWS = x.OrbitTransformXOS;

				x.TransformXWS = XMMatrixMultiply(
					XMMatrixRotationY(x.YPostAngle),
					x.TransformXWS
					);

				x.OrbitTransformXWS = XMMatrixMultiply(
					XMMatrixRotationY(x.YPostAngle),
					x.OrbitTransformXWS
					);

				x.TransformXWS = XMMatrixMultiply(
					XMMatrixMultiply(
						XMMatrixRotationY(x.YAngle),
						XMMatrixRotationZ(x.ZAngle)
						),
					x.TransformXWS
					);

				x.OrbitTransformXWS = XMMatrixMultiply(
					XMMatrixRotationZ(x.ZAngle),
					x.OrbitTransformXWS
					);

				x.SatellitePositionWS = x.SatellitePositionTS + x.PlanetPositionWS;

				x.TransformXWS = XMMatrixMultiply(
					XMMatrixTranslationFromVector(
						x.SatellitePositionWS
						),
					x.TransformXWS
					);

				/*x.OrbitTransformXWS = XMMatrixMultiply(
					XMMatrixTranslationFromVector(
						x.SatellitePositionWS
						),
					x.OrbitTransformXWS
					);*/
	
			}
		}

		void Render()
		{
			for (auto x : Transforms)
			{
				Renderer.ShapeW = &x.TransformXWS;
				OrbitRenderer.ShapeW = &x.OrbitTransformXWS;

				Renderer.Update();
				Renderer.Render();
				OrbitRenderer.Update();
				OrbitRenderer.Render();
			}

		}

	};

}