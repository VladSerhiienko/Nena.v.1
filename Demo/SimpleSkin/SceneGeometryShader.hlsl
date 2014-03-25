struct Face
{
	uint3x3 V : VERTEXINDICES;
};

struct GSOutput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Texcoords : TEXCOORD0;
};

namespace Resources
{
	cbuffer Transform : register (cb0)
	{
		row_major float4x4 WVP[3];
	};

#define _GET_WORLD Resources::WVP[0]
#define _GET_VIEW Resources::WVP[1]
#define _GET_PROJ Resources::WVP[2]

	cbuffer JointsBuffer : register (cb1)
	{
		row_major float4x4 Joints[5];
	};

#define _GET_JOINT(i) Resources::Joints[i]

	cbuffer ClustersBuffer : register (cb2)
	{
		float4 Weights[8][5];
	};

#define _GET_WEIGHT(p, j) Resources::Weights[p][j].x

	StructuredBuffer<float3> Points : register (t0);
	StructuredBuffer<float3> Normals : register (t1);
	StructuredBuffer<float2> Texcoords : register (t2);
}

#define _APPLY_JOINT_DEFORM_FOR_POSITION(j) element.Position += mul(position, _GET_JOINT(j)) * _GET_WEIGHT(pointIndex, j);
#define _APPLY_JOINT_DEFORM_FOR_NORMAL(j) element.Normal += mul(normal, (float3x3) _GET_JOINT(j)) * _GET_WEIGHT(pointIndex, j);

[maxvertexcount(3)] void main(in point Face input[1], inout TriangleStream<GSOutput> output, uint primitiveId : SV_PRIMITIVEID)
{
	printf("---------------------------");
	printf("\tGS CALL\t");

	GSOutput element;

	float3 normal = 0;
	float3 v0, v1, v2;
	float4 position = 0;
	uint jointIndex = 0;
	uint pointIndex = 0;
	float weight = 0.0f;

	uint i = 0, j = 0;
	[loop] for (i = 0; i < 5; i++)
	{
		v0 = (float3) Resources::Joints[i][0];
		v1 = (float3) Resources::Joints[i][1];
		v2 = (float3) Resources::Joints[i][2];

		printf(" joint [%d]:", i);
		printf(" { %2.2f %2.2f %2.2f } ", v0.x, v0.y, v0.z);
		printf(" { %2.2f %2.2f %2.2f } ", v1.x, v1.y, v1.z);
		printf(" { %2.2f %2.2f %2.2f } ", v2.x, v2.y, v2.z);
	}

	printf(" weights:");
	[loop] for (i = 0; i < 8; i++)
	{
		printf(" { %2.4f %2.4f %2.4f %2.4f } ", 
			_GET_WEIGHT(i, 0), _GET_WEIGHT(i, 1), 
			_GET_WEIGHT(i, 2), _GET_WEIGHT(i, 3)
			);
	}

	for (i = 0; i < 3; i++) //for (j = 0; j < 3; j++)
	{
		pointIndex = input[0].V[i][0];

		element.Texcoords = Resources::Texcoords[input[0].V[i][2]];
		normal = Resources::Normals[input[0].V[i][1]];

		position = float4(Resources::Points[pointIndex], 1.0f);
		position = mul(position, Resources::WVP[0]);

		element.Position = 0;
		element.Normal = 0;

		_APPLY_JOINT_DEFORM_FOR_POSITION(0);
		_APPLY_JOINT_DEFORM_FOR_POSITION(1);
		_APPLY_JOINT_DEFORM_FOR_POSITION(2); 
		_APPLY_JOINT_DEFORM_FOR_POSITION(3); 
		_APPLY_JOINT_DEFORM_FOR_POSITION(4);

		//element.Position = mul(position, Resources::WVP[0]);
		//element.Position = mul(element.Position, Resources::WVP[0]);
		element.Position = mul(element.Position, Resources::WVP[1]);
		element.Position = mul(element.Position, Resources::WVP[2]);

		/*_APPLY_JOINT_DEFORM_FOR_NORMAL(0);
		_APPLY_JOINT_DEFORM_FOR_NORMAL(1);
		_APPLY_JOINT_DEFORM_FOR_NORMAL(2);
		_APPLY_JOINT_DEFORM_FOR_NORMAL(3);
		_APPLY_JOINT_DEFORM_FOR_NORMAL(4);*/

		element.Normal = mul(element.Normal, (float3x3) Resources::WVP[0]);
		element.Normal = mul(element.Normal, (float3x3) Resources::WVP[1]);
		//element.Normal = mul(element.Normal, (float3x3) Resources::WVP[2]);

		printf("s> position <%f %f %f>", position.x, position.y, position.z);
		printf("p> position <%f %f %f>", element.Position.x, element.Position.y, element.Position.z);


		/*printf(
			"c> position <%f %f>", 
			element.Position.x / element.Position.w, 
			element.Position.y / element.Position.w
			);*/

		output.Append(element);
	}
}