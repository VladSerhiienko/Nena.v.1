cbuffer TRANSFORM
{
	column_major float4x4 g_world;
	column_major float4x4 g_view;
	column_major float4x4 g_proj;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT main(in VS_INPUT input, in uint vertexId : SV_VERTEXID)
{
	VS_OUTPUT output;

	input.position.w = input.color.w = 1.0f;
	output.position = mul(input.position, g_world);

	printf(
		"VS CALL %d world  { %2.2f %2.2f %2.2f %2.2f }", vertexId,
		output.position.x,
		output.position.y,
		output.position.z,
		output.position.w
		);

	output.position = mul(output.position, g_view);

	/*printf(
		"VS CALL %d view   { %2.2f %2.2f %2.2f %2.2f }", vertexId,
		output.position.x,
		output.position.y,
		output.position.z,
		output.position.w
		);*/

	output.position = mul(output.position, g_proj);

	printf(
		"VS CALL %d clip   { %2.2f %2.2f %2.2f %2.2f }", vertexId,
		output.position.x,
		output.position.y,
		output.position.z,
		output.position.w
		);

	printf(
		"VS CALL %d device { %2.2f %2.2f %2.2f %2.2f }", vertexId,
		output.position.x / output.position.w,
		output.position.y / output.position.w,
		output.position.z / output.position.w,
		output.position.w / output.position.w
		);

	float4x4 viewProj = mul(g_view, g_proj);
	float4 pos = mul(input.position, viewProj);

	printf(
		"VS CALL %d vp     { %2.2f %2.2f %2.2f %2.2f }", vertexId,
		pos.x,
		pos.y,
		pos.z,
		pos.w
		);

	output.color = input.color;
	return output;
}