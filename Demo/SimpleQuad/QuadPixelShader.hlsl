
struct PS_INPUT // = VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	//printf("PS CALL");
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
	return input.color;
}