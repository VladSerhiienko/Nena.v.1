
#define __in in const 

namespace Globals
{
	SamplerState LinearSampler : register (s0); 
	Texture3D<float4> Advected : register (t0);
	Texture3D<float4> AdvectedR : register (t1);
	RWTexture3D<float4> AdvectedN : register (u0);

#define __speed Globals::Advected
#define __speedR Globals::AdvectedR
#define __speedN Globals::AdvectedN
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


[numthreads(4, 4, 4)] void main(__in uint3 position : SV_DISPATCHTHREADID)
{
	//printf("AdvectMC3CS::main(%d, %d, %d", position.x, position.y, position.z);

	float3 tracedPosition = position - __speed[position].xyz;
	float3 samplePosition = (tracedPosition + 0.5f) / __vdims;

	float4 advected = __sample(__speed, samplePosition);
	float4 advectedR = __sample(__speedR, samplePosition);

	float4 s = 1.5f * advected - 0.5f * advectedR;

	__speedN[position] = s;
}


/*

nodeValues[0] = phi_n.Sample(samPointClamp, npos + float3(-ht.x, -ht.y, -ht.z));
nodeValues[1] = phi_n.Sample(samPointClamp, npos + float3(-ht.x, -ht.y, ht.z));
nodeValues[2] = phi_n.Sample(samPointClamp, npos + float3(-ht.x, ht.y, -ht.z));
nodeValues[3] = phi_n.Sample(samPointClamp, npos + float3(-ht.x, ht.y, ht.z));
nodeValues[4] = phi_n.Sample(samPointClamp, npos + float3(ht.x, -ht.y, -ht.z));
nodeValues[5] = phi_n.Sample(samPointClamp, npos + float3(ht.x, -ht.y, ht.z));
nodeValues[6] = phi_n.Sample(samPointClamp, npos + float3(ht.x, ht.y, -ht.z));
nodeValues[7] = phi_n.Sample(samPointClamp, npos + float3(ht.x, ht.y, ht.z));

float4 r = 
	phi_n_1_hat.Sample(samLinear, nposTC) + 0.5 * 
	(	
		phi_n.Sample(samPointClamp, in.CENTERCELL) - 
		phi_n_hat.Sample(samPointClamp, in.CENTERCELL)
	);




*/