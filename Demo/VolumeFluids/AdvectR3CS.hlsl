
#define __in in const 

namespace Globals
{
	SamplerState LinearSampler : register (s0);
	Texture3D<float4> Speed : register (t0);
	Texture3D<float4> Advected : register (t1);
	RWTexture3D<float4> AdvectedR : register (u0);

#define __speed Globals::Speed
#define __speedA Globals::Advected
#define __speedN Globals::AdvectedR
#define __lsampler Globals::LinearSampler

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

#define __vdims Globals::VolumeDims.xyz
#define __sample(tex, pos) tex.SampleLevel(__lsampler, pos, 0)

}


[numthreads(4, 4, 4)] void main(__in int3 position : SV_DISPATCHTHREADID)
{
	float3 samplePosition = ((float3)position + __speed[position].xyz + 0.5f) / __vdims;

	float4 advectedR = __sample(__speedA, samplePosition);

	__speedN[position] = advectedR;
}