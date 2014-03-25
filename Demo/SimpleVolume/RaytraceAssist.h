#pragma once

#include <Nena\AppNeon.h>
#include <Nena\Math.h>
#include "Transform.h"


#define __RaytraceAssist_AllowSSE2 1 // ??

#if __RaytraceAssist_AllowSSE2 // yeeep
 #define __RaytraceAssist_CallConv __vectorcall
#else
 #define __RaytraceAssist_CallConv
#endif

namespace Demo
{
	struct Ray
	{
		::Nena::Vector3 O; // Origin
		::Nena::Vector3 D; // Direction

		inline ::Nena::Vector3 __RaytraceAssist_CallConv operator()(float_t t)
		{
			return D * t + O;
		}
	};

	struct RaytraceAssist
	{
		static Ray __RaytraceAssist_CallConv CalculateRay(
			_In_::Nena::Vector2 screen, // screen dims
			_In_::Nena::Vector2 mouse, // pure mouse coords
			_In_::Nena::Matrix viewInv, // view inverse matrix 
			_In_::Nena::Matrix projection // projection matrix
			)
		{
			Ray ray;
			::Nena::Vector2 v;
			//::Nena::Vector3 v;

			v.x = (2.0f * mouse.x / screen.x - 1.0f) / projection._11;
			v.y = (1.0f - 2.0f * mouse.y / screen.y) / projection._22;
			//v.z = 1.0f;

			ray.D.x = v.x * viewInv._11 + v.y * viewInv._21 + /*v.z **/ viewInv._31;
			ray.D.y = v.x * viewInv._12 + v.y * viewInv._22 + /*v.z **/ viewInv._32;
			ray.D.z = v.x * viewInv._13 + v.y * viewInv._23 + /*v.z **/ viewInv._33;
			ray.D = DirectX::XMVector3Normalize(ray.D);

			ray.O.x = viewInv._41; // eye.x
			ray.O.y = viewInv._42; // eye.y
			ray.O.z = viewInv._43; // eye.z

			return ray;
		}

		static bool TestObb(
			_In_::Nena::Vector3 ray_origin,        // Ray origin, in world space
			_In_::Nena::Vector3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
			_In_::Nena::Vector3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
			_In_::Nena::Vector3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
			_In_::Nena::Matrix ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
			_Out_::Nena::Scalar& intersection_distance,		// Output : distance between ray_origin and the intersection with the OBB
			_Out_::Nena::Scalar& t1,
			_Out_::Nena::Scalar& t2
			)
		{
			using namespace Nena;
			// Intersection method from Real-Time Rendering and Essential Mathematics for Games

			float tMin = 0.0f;
			float tMax = 100000.0f;

			Vector3 OBBposition_worldspace(ModelMatrix._41, ModelMatrix._42, ModelMatrix._43);

			Vector3 delta = OBBposition_worldspace - ray_origin;

			// Test intersection with the 2 planes perpendicular to the OBB's X axis
			{
				Vector3 xaxis(ModelMatrix._11, ModelMatrix._12, ModelMatrix._13);

				XMFLOAT3 e, f;
				XMStoreFloat3(&e, XMVector3Dot(xaxis, delta));
				XMStoreFloat3(&f, XMVector3Dot(ray_direction, xaxis));

				if (fabs(f.x) > 0.001f)
				{ // Standard case

					t1 = (e.x + aabb_min.x) / f.x; // Intersection with the "left" plane
					t2 = (e.x + aabb_max.x) / f.x; // Intersection with the "right" plane
					// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

					// We want t1 to represent the nearest intersection, 
					// so if it's not the case, invert t1 and t2
					if (t1 > t2) std::swap(t1, t2);

					// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
					if (t2 < tMax) tMax = t2;
					// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
					if (t1 > tMin) tMin = t1;

					// And here's the trick :
					// If "far" is closer than "near", then there is NO intersection.
					// See the images in the tutorials for the visual explanation.
					if (tMax < tMin)
						return false;

				}
				else
				{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
					if (-e.x + aabb_min.x > 0.0f || 
						-e.x + aabb_max.x < 0.0f)
						return false;
				}
			}


			// Test intersection with the 2 planes perpendicular to the OBB's Y axis
			// Exactly the same thing than above.
			{
				Vector3 yaxis(ModelMatrix._21, ModelMatrix._22, ModelMatrix._23);

				XMFLOAT3 e, f;
				XMStoreFloat3(&e, XMVector3Dot(yaxis, delta));
				XMStoreFloat3(&f, XMVector3Dot(ray_direction, yaxis));

				if (fabs(f.x) > 0.001f)
				{

					t1 = (e.x + aabb_min.y) / f.x;
					t2 = (e.x + aabb_max.y) / f.x;

					if (t1 > t2) std::swap(t1, t2);

					if (t2 < tMax) tMax = t2;
					if (t1 > tMin) tMin = t1;
					if (tMin > tMax) return false;

				}
				else
				{
					if (-e.x + aabb_min.y > 0.0f || 
						-e.x + aabb_max.y < 0.0f)
						return false;
				}
			}


			// Test intersection with the 2 planes perpendicular to the OBB's Z axis
			// Exactly the same thing than above.
			{
				Vector3 zaxis(ModelMatrix._31, ModelMatrix._32, ModelMatrix._33);

				XMFLOAT3 e, f;
				XMStoreFloat3(&e, XMVector3Dot(zaxis, delta));
				XMStoreFloat3(&f, XMVector3Dot(ray_direction, zaxis));

				if (fabs(f.x) > 0.001f)
				{

					t1 = (e.x + aabb_min.z) / f.x;
					t2 = (e.x + aabb_max.z) / f.x;

					if (t1 > t2) std::swap(t1, t2);
					if (t2 < tMax) tMax = t2;
					if (t1 > tMin) tMin = t1;
					if (tMin > tMax) return false;
				}
				else
				{
					if (-e.x + aabb_min.z > 0.0f || 
						-e.x + aabb_max.z < 0.0f)
						return false;
				}
			}

			intersection_distance = tMin;
			return true;

		}

	};

}


