#include "ImageProcessingGlobals.hlsli"

namespace PerGroup
{
	groupshared float4 Cache[_Szy + 6]; // 3 + sz + 3
#define _cache(i) PerGroup::Cache[i]
};

[numthreads(1, _Szy, 1)] void main(
	//[numthreads(1, 1, 1)] void main(
	//in uint3 groupID : SV_GroupID, 
	in uint3 dispatchThreadId : SV_DISPATCHTHREADID,
	in uint3 groupThreadId : SV_GROUPTHREADID,
	in uint groupIndex : SV_GroupIndex
	)
{
	float4 pel = 0.0f;

	_inload(pel, dispatchThreadId);
	_cache(groupThreadId.y + 3) = pel;

	if (groupIndex == 0)
	{
		_inload(_cache(0), dispatchThreadId - uint3(0, 3, 0));
		_inload(_cache(1), dispatchThreadId - uint3(0, 2, 0));
		_inload(_cache(2), dispatchThreadId - uint3(0, 1, 0));
	}
	else if (groupIndex == _Szy - 1)
	{
		_inload(_cache(_Szy + 3), dispatchThreadId + uint3(0, 3, 0));
		_inload(_cache(_Szy + 4), dispatchThreadId + uint3(0, 2, 0));
		_inload(_cache(_Szy + 5), dispatchThreadId + uint3(0, 1, 0));
	}

	GroupMemoryBarrierWithGroupSync();

	float4 c = 0.0f;
	float4 s = 0.0f;
	float4 d = 0.0f;
	float4 r = 0.0f;
	float4 w = 0.0f;
	const float rsigma = 0.051f;

	int3 loc = groupThreadId;

	[unroll] for (int y = 0; y < 7; y++)
	{
		s = _cache(loc.y + y);
		d = pel - s;

		r = exp((-1.0f * d * d) / (2.0f * rsigma * rsigma));
		c += s * r * _filter(y);
		w += r * _filter(y);

	}

	_outstore(dispatchThreadId.xy, c / w);
}