
struct PSInput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORDS0;
};

float4 main(PSInput input) : SV_TARGET
{
	//printf("Pixel shader call");
	return float4(0.5f, 0.7f, 0.0f, 1.0f);
}