
struct PSInput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORDS0;
};

float4 main(PSInput input) : SV_TARGET
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}