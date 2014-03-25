#include "pch.h"
#include "RoomLight.h"

RoomLight::RoomLight() : m_up(0.0f, 1.0f, 0.0), 
	m_fov(XM_PIDIV4), m_aspect(1.0f),
	m_near(0.001f), m_far(500.f)
{
}