Texture2D g_ColorTexture : register (t0);
Texture2D g_DepthTexture0 : register (t1);
Texture2D g_DepthTexture1 : register (t2);
Texture2D g_DepthTexture2 : register (t3);
Texture2D g_DepthTexture3 : register (t4);

SamplerState g_ClampSampler : register (s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

SamplerState g_WrapSampler : register (s1)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

cbuffer Light
{
	float4 g_AmbientColor;
	float4 g_DiffuseColor[4];
};

struct PixelShaderInput
{
	float4 PositionClip				: SV_POSITION;
	float3 NormalWorld				: NORMAL;
	float2 Tex						: TEXCOORD0;
	float4 LightPositionClip[4]		: LIGHTCLP0;
	float3 LightPositionInertial[4]	: LIGHTDIR0;
	uint NumLights					: NLIGHTS;
};

float4 main( in PixelShaderInput input ) : SV_TARGET
{
	float4 finalColor = g_AmbientColor; // default for all pixels

	printf("pixel shader: num lights = %d", input.NumLights);

	for (int i = 0; i < input.NumLights; i++)
	{
		// Projected texture coordinates
		float2 projectedTexCoord = float2(
			input.LightPositionClip[i].x / input.LightPositionClip[i].w * +0.5f + 0.5f,
			input.LightPositionClip[i].y / input.LightPositionClip[i].w * -0.5f + 0.5f
			);

		// If projected coordinates are in the range from 0 to 1, then the pixel 
		// is in the view of the light.
		if (saturate(projectedTexCoord.x) == projectedTexCoord.x && 
			saturate(projectedTexCoord.y) == projectedTexCoord.y)
		{
			float depth = 0.0f;

			switch(i)
			{
			case 0: depth = g_DepthTexture0.Sample(g_ClampSampler, projectedTexCoord).r + 0.002f; break;
			case 1: depth = g_DepthTexture1.Sample(g_ClampSampler, projectedTexCoord).r + 0.002f; break;
			case 2: depth = g_DepthTexture2.Sample(g_ClampSampler, projectedTexCoord).r + 0.002f; break;
			case 3: depth = g_DepthTexture3.Sample(g_ClampSampler, projectedTexCoord).r + 0.002f; break;
			};

			const float lightDepth = input.LightPositionClip[i].z / input.LightPositionClip[i].w - 0.001f;

			if (lightDepth < depth)
			{
				// Calculate the amount of light on pixel.
				const float lightIntensity = saturate(dot(input.NormalWorld, input.LightPositionInertial[i]));

				if (lightIntensity > 0.0f)
				{
					// Determite the final color based on diffuse color and amount of light on it.
					finalColor += (g_DiffuseColor[i] * lightIntensity);
					// Saturate the final color.
					finalColor = saturate(finalColor);
				}
			}
		}
	}

	const float4 textureColor = g_ColorTexture.Sample(g_WrapSampler, input.Tex);
	finalColor *= textureColor;
	return finalColor;
}