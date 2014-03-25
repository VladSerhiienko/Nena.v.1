struct PixelShaderInput
{
	float4 PositionWorld : POSITION;
	float4 NormalWorld : NORMAL;
	float2 TexCoord : TEXCOORD0;
};

Texture2D g_WallTex : register (t0); // 0
SamplerState g_LinearSampler: register (s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

float4 main( in PixelShaderInput input ) : SV_TARGET
{
	// Black color for undefined texture index
	float4 texColor = g_WallTex.Sample(
		g_LinearSampler, 
		input.TexCoord
		);

	printf("PIXEL");
	printf("vertex color = <%f %f %f>",
		texColor.x,
		texColor.y,
		texColor.z
		);

	return texColor;
}