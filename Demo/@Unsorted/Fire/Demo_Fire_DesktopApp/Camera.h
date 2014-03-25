#pragma once

#include "Precompiled.h"

namespace Sandbox
{
	class Camera
	{
	private: // Members

		Reign::Vector3 m_focus;
		Reign::Vector3 m_eye;
		Reign::Vector3 m_up;
		float32 m_aspect;
		float32 m_near;
		float32 m_far;
		float32 m_fov;
		Reign::Matrix m_view;
		Reign::Matrix m_proj;

	public: // Properties

#pragma warning(disable: 4003)

		// View matrix properties
		_Decl_value_prop_finl2(m_view,		Reign::Matrix,		View);
		_Decl_value_prop_finl2(m_focus,		Reign::Vector3,		Focus);
		_Decl_value_prop_finl2(m_eye,		Reign::Vector3,		Eye);
		_Decl_value_prop_finl2(m_up,		Reign::Vector3,		Up);

		// Projection matrix properties
		_Decl_value_prop_finl2(m_proj,		Reign::Matrix,		Proj);
		_Decl_value_prop_finl1(m_aspect,	float32,			Aspect);
		_Decl_value_prop_finl1(m_near,		float32,			Near);
		_Decl_value_prop_finl1(m_far,		float32,			Far);
		_Decl_value_prop_finl1(m_fov,		float32,			Fov);

#pragma warning(default: 4003)

	public:

		Camera() : m_eye(0.0f, 0.0f, -1.0f), m_focus(0.0f, 0.0f, 1.0f), m_aspect(1.0f),
			m_up(0.0f, 1.0f, 0.0), m_fov(Reign::XM_PIDIV4),
			m_near(0.001f), m_far(100.f)
		{
		}

		REIGN_FORCEINLINING void UpdateView()
		{
			m_view = Reign::Matrix::CreateLookAt(
				m_eye, m_focus, m_up
				);
		}

		REIGN_HINTINLINING void UpdateProj()
		{
			m_proj = Reign::Matrix::CreatePerspectiveFieldOfView(
				m_fov, m_aspect, m_near, m_far
				);
		}

	};
}
