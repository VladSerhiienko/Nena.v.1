
#define __in in const 

namespace Globals
{
	//SamplerState LinearSampler : register (s0);

	Texture3D<float4> Speed : register (t0);
	Texture3D<float> Pressure : register (t1);
	RWTexture3D<float4> SpeedN : register (u0);

#define __speed Globals::Speed
#define __speedN Globals::SpeedN
#define __pressure Globals::Pressure
//#define __div Globals::Divergence
//#define __lsampler Globals::LinearSampler

	//cbuffer Factors : register (cb0)
	//{
	//	float4 VolumeDims; // xyz
	//	float4 ViewportDims; // xy
	//	float4 Mouse; // xy
	//	float4 Drag; // xy
	//	float4 ForceImpact; // x
	//	float4x4 EyeTransform;
	//	float4 FocusZoom; // x
	//};

#define __vdims Globals::VolumeDims.xyz
//#define __sample(tex, pos) tex.SampleLevel(__lsampler, pos, 0)

#define __left		int3(-1, 0, 0)
#define __right		int3(1, 0, 0)
#define __bottom	int3(0, -1, 0)
#define __up		int3(0, 1, 0)
#define __back		int3(0, 0, -1)
#define __ahead		int3(0, 0, 1)
#define __1o2		0.50000000000f
#define __zero		0.00000000000f

}

[numthreads(4, 4, 4)] void main(__in int3 position : SV_DISPATCHTHREADID)
{
	float pressurex1 = __pressure[position + __left];
	float pressurex2 = __pressure[position + __right];
	float pressurey1 = __pressure[position + __bottom];
	float pressurey2 = __pressure[position + __up];
	float pressurez1 = __pressure[position + __back];
	float pressurez2 = __pressure[position + __ahead];

	float4 grad = float4(
		pressurex2 - pressurex1,
		pressurey2 - pressurey1,
		pressurez2 - pressurez1, 
		__zero
		) *__1o2;

	__speedN[position] = __speed[position] - grad;
}
