
struct PixelShaderInput
{
	float4 PositionClip : SV_POSITION;
	float4 DepthPositionClip : TEXCOORD0;
};

float4 main( in PixelShaderInput input ) : SV_TARGET
{
	//printf( "DepthPixelShader" );

	float depthValue = input.DepthPositionClip.z / input.DepthPositionClip.w;
	
	/*printf( "depth position clip z = %f w = %f", 
		input.DepthPositionClip.z, 
		input.DepthPositionClip.w
		);
	
	printf( "depth value % f",
		depthValue
		);*/

	return float4(depthValue, depthValue, depthValue, 1.0f);
}