
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

GsInput main(VsInput input)//, in uint vertexId : SV_VERTEXID)
{
	//printf("---------------------------------------");
	//printf("Vertex shader call");
	/*printf("Vertex=%d", vertexId);
	printf("0: %d %d %d", input.V0.x, input.V0.y, input.V0.z);
	printf("1: %d %d %d", input.V1.x, input.V1.y, input.V1.z);
	printf("2: %d %d %d", input.V2.x, input.V2.y, input.V2.z);*/

	GsInput output;
	output.V[0] = input.V0;
	output.V[1] = input.V1;
	output.V[2] = input.V2;
	return output;
}