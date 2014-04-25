
struct VsInput
{
	uint3 V0 : VSVERTEX0INDICES;
	uint3 V1 : VSVERTEX1INDICES;
	uint3 V2 : VSVERTEX2INDICES;
};

struct GsInput
{
	uint3x3 V : GSVERTEXINDICES;
};

GsInput main(in const VsInput input)
{
	printf("ShadowVS");

	GsInput output;
	output.V[0] = input.V0;
	output.V[1] = input.V1;
	output.V[2] = input.V2;
	return output;
}