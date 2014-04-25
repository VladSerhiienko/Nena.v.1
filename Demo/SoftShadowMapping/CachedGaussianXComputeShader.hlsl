#include "ImageProcessingGlobals.hlsli"

namespace PerGroup
{
	groupshared float4 Cache[_Szx + 6][3]; // 3 + sz + 3
#define _cache(i,j) PerGroup::Cache[i][j]
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

	_cache(groupThreadId.x + 3, 0) = pel * _filter(0);
	_cache(groupThreadId.x + 3, 1) = pel * _filter(1);
	_cache(groupThreadId.x + 3, 2) = pel * _filter(2);

	float4 local = 0.0f;

	if (groupIndex == 0)
	{
		_inload(local, dispatchThreadId - uint3(3, 0, 0));
		_cache(0, 0) = local * _filter(0);
		_cache(0, 1) = local * _filter(1);
		_cache(0, 2) = local * _filter(2);

		_inload(local, dispatchThreadId - uint3(2, 0, 0));
		_cache(1, 0) = local * _filter(0);
		_cache(1, 1) = local * _filter(1);
		_cache(1, 2) = local * _filter(2);

		_inload(local, dispatchThreadId - uint3(1, 0, 0));
		_cache(2, 0) = local * _filter(0);
		_cache(2, 1) = local * _filter(1);
		_cache(2, 2) = local * _filter(2);
	}
	else if (groupIndex == _Szx - 1)
	{
		_inload(local, dispatchThreadId - uint3(1, 0, 0));
		_cache(_Szx + 3, 0) = local * _filter(0);
		_cache(_Szx + 3, 1) = local * _filter(1);
		_cache(_Szx + 3, 2) = local * _filter(2);

		_inload(local, dispatchThreadId - uint3(2, 0, 0));
		_cache(_Szx + 4, 0) = local * _filter(0);
		_cache(_Szx + 4, 1) = local * _filter(1);
		_cache(_Szx + 4, 2) = local * _filter(2);

		_inload(local, dispatchThreadId - uint3(3, 0, 0));
		_cache(_Szx + 5, 0) = local * _filter(0);
		_cache(_Szx + 5, 1) = local * _filter(1);
		_cache(_Szx + 5, 2) = local * _filter(2);
	}

	GroupMemoryBarrierWithGroupSync();

	int x = 0;
	float4 color = 0.0f;
	int3 tex = groupThreadId;

	color += _cache(tex.x + 0, 0);
	color += _cache(tex.x + 1, 1);
	color += _cache(tex.x + 2, 2);
	color += pel * _filter(3);
	color += _cache(tex.x + 4, 2);
	color += _cache(tex.x + 5, 1);
	color += _cache(tex.x + 6, 0);

	_outstore(dispatchThreadId.xy, color);
}