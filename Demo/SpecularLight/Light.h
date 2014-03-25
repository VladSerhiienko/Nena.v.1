#pragma once

#include <Nena\Math.h>
#include <Nena\DirectXTypes.h>
#include <Nena\PlatformAssist.h>

namespace Demo
{
	struct Light
	{
		::Nena::Color AmbientColor;
		::Nena::Color DiffuseColor;
		::Nena::Vector4 DiffuseDirection;
		::Nena::Color SpecularColor;
		::FLOAT SpecularPower;
	};
}