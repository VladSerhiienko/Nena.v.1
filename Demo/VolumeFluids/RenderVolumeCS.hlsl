
#define __in in const 

namespace Globals
{
	SamplerState LinearSampler : register (s0);
	Texture3D<float4> Speed : register (t0);
	Texture3D<float> Pressure : register (t1);
	Texture3D<float> Divergence : register (t2);
	RWTexture2D<float4> RenderTarget : register (u0);

#define __speed Globals::Speed
#define __pressure Globals::Pressure
#define __divergence Globals::Divergence

#define __target Globals::RenderTarget
#define __lsampler Globals::LinearSampler
#define __sample(tex, pos) tex.SampleLevel(__lsampler, pos, 0)

	cbuffer Factors : register (cb0)
	{
		float4 VolumeDims; // xyz
		float4 ViewportDims; // xy
		float4 Mouse; // xy
		float4 Drag; // xy
		float4 ForceImpact; // x
		float4x4 EyeTransform;
		float4 FocusZoom; // x
	};

#define __drag Globals::Drag.xyz
#define __vdims Globals::VolumeDims.xyz
#define __width Globals::ViewportDims.x
#define __height Globals::ViewportDims.y
#define __vpxy Globals::ViewportDims.xy
#define __mousexy Globals::Mouse.xy
#define __rot Globals::EyeTransform
#define __dragxy Globals::Drag.xy
#define __impactrx Globals::ForceImpact.x

}

[numthreads(8, 8, 1)] void main(__in uint3 position : SV_DISPATCHTHREADID)
{

	float3 eye = float3(0.0f, 0.0f, 4.0f);
	float2 rayAim = (2 * position.xy - __vpxy) / __vpxy;

	float3 rayDelta = -eye + float3(rayAim, 0);

	eye = mul(eye, (float3x3)__rot);
	rayDelta = mul(rayDelta, (float3x3)__rot);

	float3 positivet = max((+1.0f - eye) / rayDelta, 0);
	float3 negativet = max((-1.0f - eye) / rayDelta, 0);

	float3 frontt = min(positivet, negativet);
	float3 backt = max(positivet, negativet);

	float tfront = max(frontt.x, max(frontt.y, frontt.z));
	float tback = min(backt.x, min(backt.y, backt.z));

	float3 fronttex = (eye + tfront * rayDelta + 1.0f) * 0.5f;
	float3 backtex = (eye + tback * rayDelta + 1.0f) * 0.5f;

	float tracesteps = tfront < tback ? floor(length(backtex - fronttex) * __vdims.x + 0.5f) : 0;
	float3 texraydelta = (backtex - fronttex) / tracesteps;

#define _Draw_speed 1
#define _Draw_pressure 2
#define _Draw_divergence 3

#define _Draw 1

	float i = 0.5f;
	bool print = false;
	float3 samplePosition = 0.0f;

	float value = 0.0f;
	float visibility = 0;

#if _Draw == _Draw_speed
	float3 speedxyz = 0.0f;
#endif

	for (; i < tracesteps; i++)
	{
		samplePosition = fronttex + i * texraydelta;

#if _Draw == _Draw_speed
		speedxyz = __sample(__speed, samplePosition).xyz;
		value = length(speedxyz);
#elif _Draw == _Draw_pressure
		value = abs(__sample(__pressure, samplePosition));
#elif _Draw == _Draw_divergence
		value = abs(__sample(__divergence, samplePosition));
#endif

		visibility = max(visibility, value);
	}

	float4 metalColor = saturate(lerp(
		float4(1.41, 1.41, 1, 1), 
		float4(0, -1.41, -3, 1), 
		visibility * 0.3333f
		));

//#if _Draw != _Draw_speed
//	metalColor = visibility * 10000.0f;
//#endif

	print = visibility != 0.000f;

	if (print)
	{
		printf("VOLUME (%3d %3d %3d)", position.x, position.y, position.z);
		printf("\tcolor %3.3f %3.3f %3.3f", metalColor.x, metalColor.y, metalColor.z);
		printf("\tvisibility %f", visibility);
	}

	__target[position.xy] = metalColor;

}