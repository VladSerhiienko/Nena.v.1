
#define __in //in const 

namespace Globals
{
	//SamplerState LinearSampler : register (s0);
	Texture3D<float4> Speed : register (t0);
	RWTexture3D<float> Divergence : register (u0);

#define __speed Globals::Speed
#define __div Globals::Divergence
#define __lsampler Globals::LinearSampler

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

//#define __vdims Globals::VolumeDims.xyz
//#define __sample(tex, pos) tex.SampleLevel(__lsampler, pos, 0)

#define __left		uint3(-1,  0,  0)
#define __right		uint3( 1,  0,  0)
#define __bottom	uint3( 0, -1,  0)
#define __up		uint3( 0,  1,  0)
#define __back		uint3( 0,  0, -1)
#define __ahead		uint3( 0,  0,  1)

#define __1o2		0.50000000000f
#define __zero		0.00000000000f

}

[numthreads(4, 4, 4)] void main(__in uint3 position : SV_DISPATCHTHREADID)
{
	float speedx1 = __speed[position + __left].x;
	float speedx2 = __speed[position + __right].x;
	float speedy1 = __speed[position + __bottom].y;
	float speedy2 = __speed[position + __up].y;
	float speedz1 = __speed[position + __back].z;
	float speedz2 = __speed[position + __ahead].z;

	float div = (
		speedx2 - speedx1 +
		speedy2 - speedy1 +
		speedz2 - speedz1
		) * __1o2;

	//if (div != __zero)
	//	printf("DIV (%3d %3d %3d) = %f", position.x, position.y, position.z, div);

	//div *= 1000.0f;

	__div[position] = div;
}
