
#define __in in const 

namespace Globals
{
	//SamplerState LinearSampler : register (s0);
	Texture3D<float4> Speed : register (t0);
	RWTexture3D<float4> SpeedN : register (u0);

#define __speed Globals::Speed
#define __speedN Globals::SpeedN
//#define __lsampler Globals::LinearSampler
//#define __sample(tex, pos) tex.SampleLevel(__lsampler, pos, 0)

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


[numthreads(4, 4, 4)] void main(__in uint3 position : SV_DISPATCHTHREADID)
{

	float3 eye = float3(0.0f, 0.0f, 4.0f);
	float3 view = -eye;

	float3 agent = 0.0f;
	agent.xy = (2 * __mousexy - __vpxy) / __vpxy;
	agent = mul(agent, (float3x3)__rot);

	float3 drag = float3(__dragxy, 0.0f);
	float3 force = mul(drag, (float3x3)__rot);

	agent = (agent + 1.0f) * 0.5f;
	agent = agent * __vdims;

	float3 agentRelative = position - agent;
	float agentRadius2 = dot(agentRelative, agentRelative);
	float impactRadius2 = __impactrx * __impactrx;
	float coeff = exp(-agentRadius2 / impactRadius2);

	bool print = coeff >= 0.001f;
	print = false;

	if (print)
	{
		printf("FORCES (%3d %3d %3d)", position.x, position.y, position.z);
		printf("\tforce %3.3f %3.3f %3.3f", force.x, force.y, force.z);
		printf("\tcoeff %f", coeff);
	}

	float4 speedxyzw = __speed[position];

	if (print)
	{
		printf("\tspeed old %3.3f %3.3f %3.3f %3.3f", speedxyzw.x, speedxyzw.y, speedxyzw.z, speedxyzw.w);
	}

	speedxyzw.xyz = speedxyzw.xyz + force * coeff;
	speedxyzw.w = speedxyzw.w + length(force) * coeff;

	if (print)
	{
		printf("\tspeed new %3.3f %3.3f %3.3f %3.3f", speedxyzw.x, speedxyzw.y, speedxyzw.z, speedxyzw.w);
	}

	__speedN[position] = speedxyzw;

}