
//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

struct PixelShaderInput
{
	float4 PositionClip					: SV_POSITION;
	float4 RefractionPositionClip		: TEXCOORD0;
	float2 ColorTex						: TEXCOORD1;
};

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------

Texture2D g_GlassColor					: register(t0); // Basic surface color.
Texture2D g_GlassNormal					: register(t1); // Surface normal vectors.
Texture2D g_GlassRefraction				: register(t2); // Scene rendered behind the glass.
SamplerState g_LinearSampler			: register(s0); // Wraps coords.

cbuffer Refraction						: register(cb0)
{
	float g_GlassPerturb; // Perturbation for the refraction texture
};

//-----------------------------------------------------------------------------
// Shader
//-----------------------------------------------------------------------------

float4 main(in const PixelShaderInput input) : SV_TARGET
{
	float4 finalColor;
	//printf("CPU::Glass::PixelShaderCall");
	//printf("----------------------------------------------------");
	//printf("perturb      = <%f>", g_GlassPerturb);

	// Glass normal
	const float4 normal = g_GlassNormal.Sample(g_LinearSampler, input.ColorTex);
	//printf("normal     = <%f %f %f>", normal.x, normal.y, normal.z);
	// Glass color
	const float4 color = g_GlassColor.Sample(g_LinearSampler, input.ColorTex);
	//printf("color      = <%f %f %f %f>", color.x, color.y, color.z, color.w);

	// 1) Projection space
	// 2) Normalized device space
	// 3) Domain (from [-1,+1] to [0,1])
	const float2 refractionTextureCoords = float2(
		+input.RefractionPositionClip.x / input.RefractionPositionClip.w / 2.0f + 0.5f,
		-input.RefractionPositionClip.y / input.RefractionPositionClip.w / 2.0f + 0.5f
		);

	// 1) Domain (from [0,1] to [-1,+1])
	const float3 normalExpended = ((normal.xyz * 2.0f) - 1.0f)*50.0f;
	//printf("normal exp  = <%f %f %f>", normalExpended.x, normalExpended.y, normalExpended.z);

	// Simulate light distortion
	const float2 lightDistortion = refractionTextureCoords + (normalExpended.xy * g_GlassPerturb);
	const float4 refractionColor = g_GlassRefraction.Sample(g_LinearSampler, lightDistortion);
	//const float4 refractionColor = g_GlassRefraction.Sample(g_LinearSampler, input.ColorTex);
	/*printf(
		"CPU::Glass::PixelShaderCall > refract = %f %f %f %f", 
		refractionColor.x, refractionColor.y, 
		refractionColor.z, refractionColor.w
		);*/

	finalColor = lerp(color, refractionColor, 0.5f);
	return finalColor;
	//return refractionColor;
};