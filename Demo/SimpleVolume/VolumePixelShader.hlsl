#include "VolumeAssist.hlsli"

struct PsInput
{
	float4 PositionProj : SV_POSITION;
};

namespace PerCall
{
	cbuffer Camera : register (cb0)
	{
		row_major float4x4 CameraViewInverse;
		row_major float4x4 CameraProjection;
		float4 ScreenDims;
	};

#define _cvi PerCall::CameraViewInverse
#define _cp PerCall::CameraProjection
#define _sd PerCall::ScreenDims

	cbuffer Obb : register (cb1)
	{
		row_major float4x4 ObbWorld;
		row_major float4x4 ObbWorldInverse;
		float4 AabbMin;
		float4 AabbMax;
	};

#define _aabbmin PerCall::AabbMin
#define _aabbmax PerCall::AabbMax
#define _obbw PerCall::ObbWorld
#define _obbwi PerCall::ObbWorldInverse

	Texture3D<float> Volume : register (t0);
	SamplerState Wrap : register (s0);

#define _v PerCall::Volume
#define _wrap PerCall::Wrap

};

float4 main(in PsInput input) : SV_TARGET
{
	//printf("PS");

	float3 o = 0.0f, d = 0.0f;
	float2 m = input.PositionProj.xy;
	VolumeAssist::GenerateRay(
		_sd.xy, m, _cvi, _cp,
		//_sd.xy, m * _sd.xy, _cvi, _cp,
		o, d
		);

	printf("PS: in xy %2.2f %2.2f", m.x, m.y);
	//printf("PS: ray origin    %2.2f %2.2f %2.2f", o.x, o.y, o.z);
	//printf("PS: ray direction %2.2f %2.2f %2.2f", d.x, d.y, d.z);
	//printf("PS: aabbmin %2.2f %2.2f %2.2f", _aabbmin.x, _aabbmin.y, _aabbmin.z);
	//printf("PS: aabbmax %2.2f %2.2f %2.2f", _aabbmax.x, _aabbmax.y, _aabbmax.z);

	bool test = false;
	float dist = 0.0f, t1 = 0.0f, t2 = 0.0f;

	test = VolumeAssist::TestRayObb(
		o, d, _aabbmin.xyz, _aabbmax.xyz, _obbw,
		dist, t1, t2
		);

	if (test)
	{
		printf("PS: t1 %2.2f t2 %2.2f", t1, t2);


		float3 w_start = (o + d * t1).xyz;
		float3 w_end = (o + d * t2).xyz;
		float3 w_d = w_end - w_start;

		printf("PS: w s %2.2f %2.2f %2.2f", w_start.x, w_start.y, w_start.z);
		printf("PS: w e %2.2f %2.2f %2.2f", w_end.x, w_end.y, w_end.z);

		float3 local_start = mul(o + d * t1, _obbwi).xyz;
		float3 local_end = mul(o + d * t2, _obbwi).xyz;
		float3 local_d = local_end - local_start;

		float3 tex_start = (local_start + 5.0f) / 10.0f;
		float3 tex_end = (local_end + 5.0f) / 10.0f;
		float3 tex_d = local_d / 10.0f;
		float tex_step = 0.1f;

		//printf("PS: lcl s %2.2f %2.2f %2.2f", local_start.x, local_start.y, local_start.z);
		//printf("PS: lcl e %2.2f %2.2f %2.2f", local_end.x, local_end.y, local_end.z);
		//printf("PS: lcl d %2.2f %2.2f %2.2f", local_d.x, local_d.y, local_d.z);

		//printf("PS: tex s %2.2f %2.2f %2.2f", tex_start.x, tex_start.y, tex_start.z);
		//printf("PS: tex e %2.2f %2.2f %2.2f", tex_end.x, tex_end.y, tex_end.z);
		//printf("PS: tex d %2.2f %2.2f %2.2f", tex_d.x, tex_d.y, tex_d.z);

		return dot(local_d, local_d) / 10.0f;
	}
	else
	{
		printf("PS: test failed");
	}

	//return 0.0f;
	return float4(1.0f, 0.f, 0.f, 1.0f);
}