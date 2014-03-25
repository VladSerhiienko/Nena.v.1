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

	int x = 0;
	float4 color = 0.0f;
	int loc = groupThreadId.x;

	[unroll] for (; x < 7; x++)
		color += _cache(loc + x) * _filter(x);

	_outstore(dispatchThreadId.xy, color);
}