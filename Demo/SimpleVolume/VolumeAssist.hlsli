
namespace VolumeAssist
{
	void swap(inout float a, inout float b)
	{
		float c = a;
		a = b;
		b = c;
	}

	void GenerateRay(
		in float2 screen,
		in float2 mouse,
		in float4x4 viewInv,
		in float4x4 projection,
		out float3 ray_origin,
		out float3 ray_direction
		)
	{
		float2 v;

		v.x = (2.0f * mouse.x / screen.x - 1.0f) / projection._11;
		v.y = (1.0f - 2.0f * mouse.y / screen.y) / projection._22;

		//printf("PS: gen ray: s %2.1f %2.1f", screen.x, screen.y);
		//printf("PS: gen ray: m %2.1f %2.1f", mouse.x, mouse.y);
		//printf("PS: gen ray: v %2.1f %2.1f", v.x, v.y);

		ray_direction.x = v.x * viewInv._11 + v.y * viewInv._21 + viewInv._31;
		ray_direction.y = v.x * viewInv._12 + v.y * viewInv._22 + viewInv._32;
		ray_direction.z = v.x * viewInv._13 + v.y * viewInv._23 + viewInv._33;
		ray_direction = normalize(ray_direction);

		ray_origin.x = viewInv._41;
		ray_origin.y = viewInv._42;
		ray_origin.z = viewInv._43;
	}

	bool TestRayObb(
		in float3 ray_origin,
		in float3 ray_direction,
		in float3 aabb_min,
		in float3 aabb_max,
		in float4x4 aabb_world,
		out float dist,
		out float t1,
		out float t2
		)
	{

		float tMin = 0.0f;
		float tMax = 100000.0f;

		float3 pos = float3(
			aabb_world._41, 
			aabb_world._42, 
			aabb_world._43
			);

		//printf("PS: test oob: aabbw %2.2f %2.2f %2.2f", pos.x, pos.y, pos.z);

		float3 delta = pos - ray_origin;

		//printf("PS: test oob: delta %2.2f %2.2f %2.2f", delta.x, delta.y, delta.z);

		{
			float3 xaxis = float3(
				aabb_world._11, 
				aabb_world._12, 
				aabb_world._13
				);

			//printf("PS: test oob: x ax %2.2f %2.2f %2.2f", xaxis.x, xaxis.y, xaxis.z);


			float e = dot(xaxis, delta);
			float f = dot(ray_direction, xaxis);

			//printf("PS: test oob: x: e %2.2f f %2.2f", e, f);

			if (abs(f) > 0.001f)
			{

				t1 = (e + aabb_min.x) / f; 
				t2 = (e + aabb_max.x) / f;

				//printf("PS: test oob: x: t1 %2.2f t2 %2.2f", t1, t2);

				if (t1 > t2) swap(t1, t2);
				if (t2 < tMax) tMax = t2;
				if (t1 > tMin) tMin = t1;
				if (tMax < tMin)
				{
					//printf("PS: test oob: x fail");
					return false;
				}
			}
			else
			{
				if (-e + aabb_min.x > 0.0f ||
					-e + aabb_max.x < 0.0f)
				{
					//printf("PS: test oob: x fail");
					return false;
				}
			}
		}

		{
			float3 yaxis = float3(
				aabb_world._21, 
				aabb_world._22, 
				aabb_world._23
				);

			//printf("PS: test oob: y ax %2.2f %2.2f %2.2f", yaxis.x, yaxis.y, yaxis.z);

			float e = dot(yaxis, delta);
			float f = dot(ray_direction, yaxis);

			//printf("PS: test oob: y: e %2.2f f %2.2f", e, f);

			if (abs(f) > 0.001f)
			{
				t1 = (e + aabb_min.y) / f;
				t2 = (e + aabb_max.y) / f;

				//printf("PS: test oob: y: t1 %2.2f t2 %2.2f", t1, t2);

				if (t1 > t2) swap(t1, t2);
				if (t2 < tMax) tMax = t2;
				if (t1 > tMin) tMin = t1;
				if (tMin > tMax)
				{
					//printf("PS: test oob: y fail");
					return false;
				}

			}
			else
			{
				if (-e + aabb_min.y > 0.0f ||
					-e + aabb_max.y < 0.0f)
				{
					//printf("PS: test oob: y fail");
					return false;
				}
			}
		}

		{
			float3 zaxis = float3(
				aabb_world._31, 
				aabb_world._32, 
				aabb_world._33
				);

			//printf("PS: test oob: z ax %2.2f %2.2f %2.2f", zaxis.x, zaxis.y, zaxis.z);

			float e = dot(zaxis, delta);
			float f = dot(ray_direction, zaxis);

			//printf("PS: test oob: z: e %2.2f f %2.2f", e, f);

			if (abs(f) > 0.001f)
			{
				t1 = (e + aabb_min.z) / f;
				t2 = (e + aabb_max.z) / f;

				//printf("PS: test oob: z: t1 %2.2f t2 %2.2f", t1, t2);

				if (t1 > t2) swap(t1, t2);
				if (t2 < tMax) tMax = t2;
				if (t1 > tMin) tMin = t1;
				if (tMin > tMax)
				{
					//printf("PS: test oob: z fail");
					return false;
				}
			}
			else
			{
				if (-e + aabb_min.z > 0.0f ||
					-e + aabb_max.z < 0.0f)
				{
					//printf("PS: test oob: z fail");
					return false;
				}
			}
		}

		dist = tMin;
		return true;
	}
};