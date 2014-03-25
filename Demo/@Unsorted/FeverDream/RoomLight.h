#pragma once

#include "pch.h"

class RoomLight
{
private:

	Reign::Vector4 m_ambientColor;
	Reign::Vector4 m_diffuseColor;
	Reign::Vector3 m_focus;
	Reign::Vector3 m_eye;
	Reign::Vector3 m_up;
	Reign::Matrix m_view;
	Reign::Matrix m_proj;
	float32 m_aspect;
	float32 m_near;
	float32 m_far;
	float32 m_fov;

public:
	
	REIGN_FORCEINLINING Reign::Matrix const *GetView() const { return &m_view; }
	REIGN_FORCEINLINING Reign::Matrix GetViewCpy() const { return m_view; }
	REIGN_FORCEINLINING Reign::Matrix *EditView() { return &m_view; }
	REIGN_FORCEINLINING Reign::Matrix const *GetProj() const { return &m_proj; }
	REIGN_FORCEINLINING Reign::Matrix GetProjCpy() const { return m_proj; }
	REIGN_FORCEINLINING Reign::Matrix *EditProj() { return &m_proj; }
	REIGN_FORCEINLINING Reign::Vector4 const *GetAmbient() const {  return &m_ambientColor; }
	REIGN_FORCEINLINING Reign::Vector4 GetAmbientCpy() const { return m_ambientColor; }
	REIGN_FORCEINLINING Reign::Vector4 *EditAmbient() { return &m_ambientColor; }
	REIGN_FORCEINLINING Reign::Vector4 const *GetDiffuse() const {  return &m_diffuseColor; }
	REIGN_FORCEINLINING Reign::Vector4 GetDiffuseCpy() const { return m_diffuseColor; }
	REIGN_FORCEINLINING Reign::Vector4 *EditDiffuse() { return &m_diffuseColor; }
	REIGN_FORCEINLINING Reign::Vector3 const *GetEye() const { return &m_eye; }
	REIGN_FORCEINLINING Reign::Vector3 GetEyeCpy() const { return m_eye; }
	REIGN_FORCEINLINING Reign::Vector3 *EditEye() { return &m_eye; }
	REIGN_FORCEINLINING Reign::Vector3 const *GetFocus() const { return &m_focus; }
	REIGN_FORCEINLINING Reign::Vector3 GetFocusCpy() const { return m_focus; }
	REIGN_FORCEINLINING Reign::Vector3 *EditFocus() { return &m_focus; }
	REIGN_FORCEINLINING Reign::Vector3 const *GetUp() const { return &m_up; }
	REIGN_FORCEINLINING Reign::Vector3 GetUpCpy() const { return m_up; }
	REIGN_FORCEINLINING Reign::Vector3 *EditUp() { return &m_up; }
	REIGN_FORCEINLINING float32 Aspect() const { return m_aspect; }
	REIGN_FORCEINLINING void Aspect( _In_ float32 value ) { m_aspect = value; }
	REIGN_FORCEINLINING float32 Near() const { return m_near; }
	REIGN_FORCEINLINING void Near( _In_ float32 value ) { m_near = value; }
	REIGN_FORCEINLINING float32 Fov() const { return m_fov; }
	REIGN_FORCEINLINING void Fov( _In_ float32 value ) { m_fov = value; }
	REIGN_FORCEINLINING float32 Far() const { return m_far; }
	REIGN_FORCEINLINING void Far( _In_ float32 value ) { m_far = value; }

	RoomLight();

	inline void UpdateView( )
	{
		m_view = Reign::Matrix::CreateLookAt(
			m_eye, m_focus, m_up
			);
	}

	inline void UpdateProj( )
	{
		m_proj = Reign::Matrix::CreatePerspectiveFieldOfView(
			m_fov, m_aspect, m_near, m_far
			);
	}

};