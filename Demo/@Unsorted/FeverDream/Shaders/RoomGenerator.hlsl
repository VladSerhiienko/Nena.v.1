#define MaxTilesPerX 16
#define MaxTilesPerY 16
#define ElementsPerTile 36 // 2 * 3 * 6

struct Tile
{
	float X;
	float Y;
	int Fordable;
};

struct Vertex
{
	float4 PositionWorld; // float4x4 World;
	float4 NormalWorld; // float4 PositionLocal;
	float2 TexCoord;
	int TexId;
};

cbuffer Room : register (cb0)
{
	float g_TilesPerX;
	float g_TilesPerY;
	float g_TileRadii;
};

cbuffer Tile : register (cb1)
{
	static const float2 g_TextureCoords[4] = 
	{
		float2(0.0f, 1.0f), // v0
		float2(1.0f, 1.0f), // v1
		float2(1.0f, 0.0f), // v2
		float2(0.0f, 0.0f), // v3
	};
};


StructuredBuffer<Tile> g_Tiles : register (t0); // 16 * 16
RWStructuredBuffer<Vertex> g_Vertices : register (u0); // 2 tri * 3 vert * 6 side * 16 * 16 tiles

/// Execute onces per level to generate static room mesh

[numthreads(MaxTilesPerX, MaxTilesPerY, 1)]
void main( in uint3 dispatchThreadId : SV_DispatchThreadID )
{
	printf( "Room X=%d Y=%d Radii=%f", g_TilesPerX, g_TilesPerY, g_TileRadii );

	const uint tileX = dispatchThreadId.x;
	const uint tileY = dispatchThreadId.y;
	const uint tileIndex = tileY * MaxTilesPerY + tileX;
	const uint elementIndex = tileIndex * ElementsPerTile;

	printf( "thread x = %d y = %d", tileX, tileY );
	printf( "tile index = %d", tileIndex );
	printf( "element index = %d", elementIndex );
	 
	const Tile tile = g_Tiles[tileIndex];
	
	printf( "tile x = %f y = %f", tile.X, tile.Y );

	const float tileUpHeight = tile.Fordable == 1 ? 0 : g_TileRadii;
	const float tileDownHeight = -g_TileRadii * 0.5f;
	const float4 tileCenter = float4(tile.X * 2 * g_TileRadii, 0.0f, tile.Y * 2 * g_TileRadii, 0.0f); 

	printf( "tile center x = %f z = %f", tileCenter.x, tileCenter.z );

	/// Foreach tile side evaluate world position implicitly
	/// Assign normal and texture coord for each tile element (vertex)
	/// Append generated vertices to the vertex buffers for the vertex shader.


	uint elementIndexOffset = 0;

	const float4 fru = float4( +g_TileRadii,	tileUpHeight,		+g_TileRadii, 0.0f );	// + + + // forward right up
	const float4 flu = float4( -g_TileRadii,	tileUpHeight,		+g_TileRadii, 0.0f );	// - + + // forward left up
	const float4 fld = float4( -g_TileRadii,	tileDownHeight,		+g_TileRadii, 0.0f );	// - - + // forward left down
	const float4 frd = float4( +g_TileRadii,	tileDownHeight,		+g_TileRadii, 0.0f );	// + - + // forward right down
	const float4 bru = float4( +g_TileRadii,	tileUpHeight,		-g_TileRadii, 0.0f );	// + + - // backward right up
	const float4 blu = float4( -g_TileRadii,	tileUpHeight,		-g_TileRadii, 0.0f );	// - + - // backward left up
	const float4 bld = float4( -g_TileRadii,	tileDownHeight,		-g_TileRadii, 0.0f );	// - - - // backward right up
	const float4 brd = float4( +g_TileRadii,	tileDownHeight,		-g_TileRadii, 0.0f );	// + - - // backward right down

	// Front

	{
		Vertex triPoint[4];

		triPoint[0].PositionWorld = tileCenter + fru;
		triPoint[1].PositionWorld = tileCenter + flu;
		triPoint[2].PositionWorld = tileCenter + fld;
		triPoint[3].PositionWorld = tileCenter + frd;

		[unroll] for (int j = 0; j < 4; j++)
		{
			triPoint[j].PositionWorld.w = 1.0f;
			triPoint[j].NormalWorld = float4(0.0f, 0.0f, 1.0f, 0.0f);
			triPoint[j].TexCoord = g_TextureCoords[j];
			triPoint[j].TexId = 1; // wall
		}

		g_Vertices[elementIndex + 0 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 0 + elementIndexOffset );
		g_Vertices[elementIndex + 1 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 1 + elementIndexOffset );
		g_Vertices[elementIndex + 2 + elementIndexOffset] = triPoint[0]; printf( "writing vertex to %d", elementIndex + 2 + elementIndexOffset );
		g_Vertices[elementIndex + 3 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 3 + elementIndexOffset );
		g_Vertices[elementIndex + 4 + elementIndexOffset] = triPoint[2]; printf( "writing vertex to %d", elementIndex + 4 + elementIndexOffset );
		g_Vertices[elementIndex + 5 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 5 + elementIndexOffset );

		elementIndexOffset += 6;
	}

	// Right

	{
		Vertex triPoint[4];

		triPoint[0].PositionWorld = tileCenter + bru;
		triPoint[1].PositionWorld = tileCenter + fru;
		triPoint[2].PositionWorld = tileCenter + frd;
		triPoint[3].PositionWorld = tileCenter + brd;

		[unroll] for (int j = 0; j < 4; j++)
		{
			triPoint[j].PositionWorld.w = 1.0f;
			triPoint[j].NormalWorld = float4(1.0f, 0.0f, 0.0f, 0.0f);
			triPoint[j].TexCoord = g_TextureCoords[j];
			triPoint[j].TexId = 1; // wall
		}

		g_Vertices[elementIndex + 0 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 0 + elementIndexOffset );
		g_Vertices[elementIndex + 1 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 1 + elementIndexOffset );
		g_Vertices[elementIndex + 2 + elementIndexOffset] = triPoint[0]; printf( "writing vertex to %d", elementIndex + 2 + elementIndexOffset );
		g_Vertices[elementIndex + 3 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 3 + elementIndexOffset );
		g_Vertices[elementIndex + 4 + elementIndexOffset] = triPoint[2]; printf( "writing vertex to %d", elementIndex + 4 + elementIndexOffset );
		g_Vertices[elementIndex + 5 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 5 + elementIndexOffset );

		elementIndexOffset += 6;
	}

	// Back

	{
		Vertex triPoint[4];

		triPoint[0].PositionWorld = tileCenter + blu;
		triPoint[1].PositionWorld = tileCenter + bru;
		triPoint[2].PositionWorld = tileCenter + brd;
		triPoint[3].PositionWorld = tileCenter + bld;

		[unroll] for (int j = 0; j < 4; j++)
		{
			triPoint[j].PositionWorld.w = 1.0f;
			triPoint[j].NormalWorld = float4(0.0f, 0.0f, -1.0f, 0.0f);
			triPoint[j].TexCoord = g_TextureCoords[j];
			triPoint[j].TexId = 1; // wall
		}

		g_Vertices[elementIndex + 0 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 0 + elementIndexOffset );
		g_Vertices[elementIndex + 1 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 1 + elementIndexOffset );
		g_Vertices[elementIndex + 2 + elementIndexOffset] = triPoint[0]; printf( "writing vertex to %d", elementIndex + 2 + elementIndexOffset );
		g_Vertices[elementIndex + 3 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 3 + elementIndexOffset );
		g_Vertices[elementIndex + 4 + elementIndexOffset] = triPoint[2]; printf( "writing vertex to %d", elementIndex + 4 + elementIndexOffset );
		g_Vertices[elementIndex + 5 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 5 + elementIndexOffset );

		elementIndexOffset += 6;
	}

	// Left

	{
		Vertex triPoint[4];

		triPoint[0].PositionWorld = tileCenter + flu;
		triPoint[1].PositionWorld = tileCenter + blu;
		triPoint[2].PositionWorld = tileCenter + bld;
		triPoint[3].PositionWorld = tileCenter + fld;

		[unroll] for (int j = 0; j < 4; j++)
		{
			triPoint[j].PositionWorld.w = 1.0f;
			triPoint[j].NormalWorld = float4(-1.0f, 0.0f, 0.0f, 0.0f);
			triPoint[j].TexCoord = g_TextureCoords[j];
			triPoint[j].TexId = 1; // wall
		}

		g_Vertices[elementIndex + 0 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 0 + elementIndexOffset );
		g_Vertices[elementIndex + 1 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 1 + elementIndexOffset );
		g_Vertices[elementIndex + 2 + elementIndexOffset] = triPoint[0]; printf( "writing vertex to %d", elementIndex + 2 + elementIndexOffset );
		g_Vertices[elementIndex + 3 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 3 + elementIndexOffset );
		g_Vertices[elementIndex + 4 + elementIndexOffset] = triPoint[2]; printf( "writing vertex to %d", elementIndex + 4 + elementIndexOffset );
		g_Vertices[elementIndex + 5 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 5 + elementIndexOffset );

		elementIndexOffset += 6;
	}

	// Upper

	{
		Vertex triPoint[4];

		triPoint[0].PositionWorld = tileCenter + flu;
		triPoint[1].PositionWorld = tileCenter + fru;
		triPoint[2].PositionWorld = tileCenter + bru;
		triPoint[3].PositionWorld = tileCenter + blu;

		[unroll] for (int j = 0; j < 4; j++)
		{
			triPoint[j].PositionWorld.w = 1.0f;
			triPoint[j].NormalWorld = float4(0.0f, 1.0f, 0.0f, 0.0f);
			triPoint[j].TexCoord = g_TextureCoords[j];
			triPoint[j].TexId = tile.Fordable == 1 ? 0 : 3; // floor or black
		}

		g_Vertices[elementIndex + 0 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 0 + elementIndexOffset );
		g_Vertices[elementIndex + 1 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 1 + elementIndexOffset );
		g_Vertices[elementIndex + 2 + elementIndexOffset] = triPoint[0]; printf( "writing vertex to %d", elementIndex + 2 + elementIndexOffset );
		g_Vertices[elementIndex + 3 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 3 + elementIndexOffset );
		g_Vertices[elementIndex + 4 + elementIndexOffset] = triPoint[2]; printf( "writing vertex to %d", elementIndex + 4 + elementIndexOffset );
		g_Vertices[elementIndex + 5 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 5 + elementIndexOffset );

		/*g_Vertices[elementIndex + 0 + elementIndexOffset] = triPoint[0]; printf( "writing vertex to %d", elementIndex + 0 + elementIndexOffset );
		g_Vertices[elementIndex + 1 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 1 + elementIndexOffset );
		g_Vertices[elementIndex + 2 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 2 + elementIndexOffset );
		g_Vertices[elementIndex + 3 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 3 + elementIndexOffset );
		g_Vertices[elementIndex + 4 + elementIndexOffset] = triPoint[2]; printf( "writing vertex to %d", elementIndex + 4 + elementIndexOffset );
		g_Vertices[elementIndex + 5 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 5 + elementIndexOffset );*/

		elementIndexOffset += 6;
	}
	
	// Bottom

	{
		Vertex triPoint[4];

		triPoint[0].PositionWorld = tileCenter + bld;
		triPoint[1].PositionWorld = tileCenter + brd;
		triPoint[2].PositionWorld = tileCenter + frd;
		triPoint[3].PositionWorld = tileCenter + fld;

		[unroll] for (int j = 0; j < 4; j++)
		{
			triPoint[j].PositionWorld.w = 1.0f;
			triPoint[j].NormalWorld = float4(0.0f, -1.0f, 0.0f, 0.0f);
			triPoint[j].TexCoord = g_TextureCoords[j];
			triPoint[j].TexId = 3; // always black
		}

		g_Vertices[elementIndex + 0 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 0 + elementIndexOffset );
		g_Vertices[elementIndex + 1 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 1 + elementIndexOffset );
		g_Vertices[elementIndex + 2 + elementIndexOffset] = triPoint[0]; printf( "writing vertex to %d", elementIndex + 2 + elementIndexOffset );
		g_Vertices[elementIndex + 3 + elementIndexOffset] = triPoint[1]; printf( "writing vertex to %d", elementIndex + 3 + elementIndexOffset );
		g_Vertices[elementIndex + 4 + elementIndexOffset] = triPoint[2]; printf( "writing vertex to %d", elementIndex + 4 + elementIndexOffset );
		g_Vertices[elementIndex + 5 + elementIndexOffset] = triPoint[3]; printf( "writing vertex to %d", elementIndex + 5 + elementIndexOffset );

		elementIndexOffset += 6;
	}
}