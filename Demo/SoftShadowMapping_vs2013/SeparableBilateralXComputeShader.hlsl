#include "ImageProcessingGlobals.hlsli"

namespace PerGroup
{
	groupshared float4 Cache[_Szx + 6]; // 3 + sz + 3
#define _cache(i) PerGroup::Cache[i]
};

[numthreads(_Szx, 1, 1)] void main(
	//[numthreads(1, 1, 1)] void main(
	//in uint3 groupID : SV_GroupID, 
	in uint3 dispatchThreadId : SV_DISPATCHTHREADID,
	in uint3 groupThreadId : SV_GROUPTHREADID,
	in uint groupIndex : SV_GroupIndex
	)
{
	float4 pel = 0.0f;
	_inload(pel, dispatchThreadId);
	_cache(groupThreadId.x + 3) = pel;

	if (groupIndex == 0)
	{
		_inload(_cache(0), dispatchThreadId - uint3(3, 0, 0));
		_inload(_cache(1), dispatchThreadId - uint3(2, 0, 0));
		_inload(_cache(2), dispatchThreadId - uint3(1, 0, 0));
	}
	else if (groupIndex == _Szx - 1)
	{
		_inload(_cache(_Szx + 3), dispatchThreadId + uint3(3, 0, 0));
		_inload(_cache(_Szx + 4), dispatchThreadId + uint3(2, 0, 0));
		_inload(_cache(_Szx + 5), dispatchThreadId + uint3(1, 0, 0));
	}

	GroupMemoryBarrierWithGroupSync();

	float4 c = 0.0f;
	float4 s = 0.0f;
	float4 d = 0.0f;
	float4 r = 0.0f;
	float4 w = 0.0f;
	const float rsigma = 0.051f;

	int3 loc = groupThreadId;

	[unroll] for (int x = 0; x < 7; x++)
	{
		s = _cache(loc.x + x);
		d = pel - s;
		r = exp((-1.0f * d * d) / (2.0f * rsigma * rsigma));
		c += s * r * _filter(x);
		w += r * _filter(x);

	}

	_outstore(dispatchThreadId.xy, c / w);
}