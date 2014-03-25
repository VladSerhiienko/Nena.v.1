#pragma once

#include "ShapeRenderer.h"

namespace Demo
{
	struct Planet
	{
		Shape Content;
		ShapeRenderer Renderer;

		::FLOAT ZAngle;
		::FLOAT YVelocity;

		void Update(
			_In_::FLOAT elapsedTime,
			_In_::FLOAT totalTime
			)
		{
			using namespace ::Nena;

			Content.TransformMWS = XMMatrixMultiply(
				XMMatrixRotationX(XM_PI),
				XMMatrixScaling(0.5f, 0.5f, 0.5f)
				);

			Content.TransformMWS = XMMatrixMultiply(
				XMMatrixRotationZ(ZAngle),
				Content.TransformMWS
				);

			Content.TransformMWS = XMMatrixMultiply(
				XMMatrixRotationY(YVelocity * totalTime),
				Content.TransformMWS
				);

		}

		void Render()
		{


			Renderer.Update();
			Renderer.Render();
		}
	};

}