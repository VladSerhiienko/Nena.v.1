struct PixelShaderInput // GeometryShaderOutput
{
	//float4 Position	: SV_POSITION;
	float4 Color		: COLOR;
	//float2 TexCoord	: TEXCOORD;
};

float4 main( in PixelShaderInput Input ) : SV_TARGET
{
	//printf( "Ps call" );

	// See: The vertex shader where this color was approximated. 
	return Input.Color;
}