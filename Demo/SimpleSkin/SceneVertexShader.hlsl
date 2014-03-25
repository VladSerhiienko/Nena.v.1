
struct VSInput
{
	uint3 Vertex0 : VERTEX0INFO; // 0
	uint3 Vertex1 : VERTEX1INFO; // 12
	uint3 Vertex2 : VERTEX2INFO; // 24
};

struct GSInput
{
	uint3x3 Indices : VERTEXINDICES;
};

GSInput main(VSInput input, uint vertexId : SV_VERTEXID)
{
	printf("---------------------------");
	printf("\tVS CALL %d\t", vertexId);

	GSInput output;

	//printf("%d %d %d", input.Vertex0.x, input.Vertex0.y, input.Vertex0.z);
	//printf("%d %d %d", input.Vertex1.x, input.Vertex1.y, input.Vertex1.z);
	//printf("%d %d %d", input.Vertex2.x, input.Vertex2.y, input.Vertex2.z);

	output.Indices[0][0] = input.Vertex0.x;
	output.Indices[0][1] = input.Vertex0.y;
	output.Indices[0][2] = input.Vertex0.z;
	output.Indices[1][0] = input.Vertex1.x;
	output.Indices[1][1] = input.Vertex1.y;
	output.Indices[1][2] = input.Vertex1.z;
	output.Indices[2][0] = input.Vertex2.x;
	output.Indices[2][1] = input.Vertex2.y;
	output.Indices[2][2] = input.Vertex2.z;

	return output;
}