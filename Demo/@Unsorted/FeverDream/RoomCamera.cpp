#include "pch.h"
#include "RoomCamera.h"


RoomCamera::RoomCamera() : m_focus(0.0f, 0.0f, 1.0f), m_aspect(1.0f), 
	m_up(0.0f, 1.0f, 0.0), m_fov(XM_PIDIV4),
	m_near(0.001f), m_far(500.f)
{
}