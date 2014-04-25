#pragma once
#include "Camera.h"

#include <Nena\Math.h>
#include <Nena\DirectXTypes.h>
#include <Nena\PlatformAssist.h>

namespace Demo
{
	struct Light : public Camera
	{
		Nena::Color AmbientColor;
		Nena::Color DiffuseColor;

		Light() : Camera(), AmbientColor(), DiffuseColor()
		{
			AmbientColor = (Nena::XMVECTOR) Nena::Colors::Black;
			DiffuseColor = (Nena::XMVECTOR) Nena::Colors::White;

			//AmbientColor = (Nena::XMVECTOR) Nena::Colors::White;
			//DiffuseColor = (Nena::XMVECTOR) Nena::Colors::WhiteSmoke;
		}
	};
}