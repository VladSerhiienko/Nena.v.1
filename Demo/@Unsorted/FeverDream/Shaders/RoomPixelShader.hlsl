struct PixelShaderInput
{
	float4 PositionWorld : POSITION;
	float4 NormalWorld : NORMAL;
	float2 TexCoord : TEXCOORD0;
	int TexId : TEXCOORD1;
};

Texture2D g_CeilingTex : register (t2); // 2
Texture2D g_FloorTex : register (t0); // 0
Texture2D g_WallTex : register (t1); // 1

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
	float4 texColor = float4(0.0f, 0.0f, 0.0f, 1.0f); 

	switch(input.TexId)
	{
	case 1: texColor = g_WallTex.Sample(
		g_LinearSampler, 
		input.TexCoord
		); break;
	case 0: texColor = g_FloorTex.Sample(
		g_LinearSampler, 
		input.TexCoord
		); break;
	case 2: texColor = g_CeilingTex.Sample(
		g_LinearSampler, 
		input.TexCoord
		); break;
	}

	printf("vertex color [%d] = <%f %f %f>",
		input.TexId,
		texColor.x,
		texColor.y,
		texColor.z
		);

	return texColor;
}