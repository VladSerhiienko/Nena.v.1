
//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

struct PixelShaderInput
{
	float4 PositionClip : SV_Position;
	float2 ColorTex : TexCoord0;
};

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

Texture2D<float4> g_ColorTexture : register ( t0 );
SamplerState g_LinearSampler : register ( s0 );

//-----------------------------------------------------------------------------
// Shader
//-----------------------------------------------------------------------------

float4 main( in const PixelShaderInput input ) : SV_TARGET
{
	//printf( "CPU::FluidBox::PixelShaderCall" );
	//printf( "----------------------------------------------------" );
	float4 color = g_ColorTexture.Sample( g_LinearSampler, input.ColorTex );
	//color.z = abs(lerp( color.x, color.y, 0.5f ));
	//color.z = dot(color.xy, color.xy);

	//color.xyz *= 3.0f;

	//return abs(color) * 2.0f;
	//return abs(color);
	return color;

	// Or just ...
	//return g_ColorTexture.Sample( g_LinearSampler, input.ColorTex );
}