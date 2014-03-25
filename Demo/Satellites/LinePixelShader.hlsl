struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
	return float4(input.Color.rgb, 1.0f);
}