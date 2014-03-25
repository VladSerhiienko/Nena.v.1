#pragma once

#include <Nena\Math.h>
#include <Nena\DeviceResources.h>

namespace Framework
{
	namespace Application
	{
		struct QuadResources // all the resources quad renderer requires
		{
			// device dependent resources

			Nena::Graphics::Resources::Buffer TransformationBuffer; // cb wvp
			Nena::Graphics::Resources::Buffer VertexBuffer; // vb
			Nena::Graphics::Resources::Buffer IndexBuffer; // ib
			Nena::Graphics::Resources::VSLayout Vi; // input layout
			Nena::Graphics::Resources::VShader Vs; // vertex shader
			Nena::Graphics::Resources::PShader Ps; // pixel shader

			QuadResources() { }
			~QuadResources() { }

		};
	}
}