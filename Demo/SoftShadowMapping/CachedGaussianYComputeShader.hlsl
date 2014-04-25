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

	int y = 0;
	float4 color = 0.0f;
		int loc = groupThreadId.y;

	[unroll] for (; y < 7; y++)
		color += _cache(loc + y) * _filter(y);

	_outstore(dispatchThreadId.xy, color);
}