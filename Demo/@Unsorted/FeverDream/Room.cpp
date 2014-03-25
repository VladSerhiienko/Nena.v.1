#include "pch.h"
#include "Room.h"

#define RoomSpec_TrueValue			("True")
#define RoomSpec_FalseValue			("False")
#define RoomSpec_IdAttribute		("Id")
#define RoomSpec_TextureIdAttribute ("TextureId")
#define RoomSpec_SideNode			("Side")
#define RoomSpec_TransposeAttribute ("Transpose")
#define RoomSpec_RootNode			("Room")
#define RoomSpec_TerrainNode		("Terrain")
#define RoomSpec_StartNode			("Start")
#define RoomSpec_FinishNode			("Finish")
#define RoomSpec_PlayerNode			("Player")
#define RoomSpec_NPCNode			("NPC")
#define RoomSpec_XAttribute			("X")
#define RoomSpec_YAttribute			("Y")
#define RoomSpec_PlayerId			("PlayerId")
#define RoomSpec_OffsetNode			("Offset")
#define RoomSpec_DimensionsNode		("Dimension")
#define RoomSpec_TexturesNode		("Textures")
#define RoomSpec_TilesNode			("Tiles")
#define RoomSpec_TileNode			("Tile")
#define RoomSpec_TileRadiiNode		("TileRadii")
#define RoomSpec_SlicesNode			("Slices")
#define RoomSpec_SliceNode			("Slice")
#define RoomSpec_PathAttribute		("Path")
#define RoomSpec_WallTile			('#')
#define RoomSpec_ForableTile		(' ')
#define RoomSpec_WallTile1			('1')
#define RoomSpec_ForableTile2		('0')

#define RoomSpec_Separator			"."
#define RoomSpec_VersionAttribute	"Version"
#define RoomSpec_ClientAttribute	"Client"
#define RoomSpec_Version			"1.0.0.0"
#define RoomSpec_OS					"Windows"

#define RoomSpec_Client RoomSpec_OS RoomSpec_Separator \
	RoomSpec_VersionAttribute RoomSpec_Separator \
	RoomSpec_Version

Room::Room()
{
}

void Room::ReadFrom(
	_In_ String8 file // xml
	)
{
	using namespace std;
	using namespace rapidxml;

	xml_document<> xmlDocument;
	xml_node<> *xmlRootNode;

	// Read the xml file into a vector
	ifstream reader(file);
	vector<char> buffer(
		(istreambuf_iterator<char>(reader)), 
		(istreambuf_iterator<char>())
		);

	buffer.push_back('\0');

	xmlDocument.parse<0>(&buffer[0]);
	xmlRootNode = xmlDocument.first_node(RoomSpec_RootNode);

	auto terrain = xmlRootNode->first_node(RoomSpec_TerrainNode);
	{
		auto terrainDimension = terrain->first_node(RoomSpec_DimensionsNode);
		m_terrain.u16XDim = atoi(terrainDimension->first_attribute(RoomSpec_XAttribute)->value());
		m_terrain.u16YDim = atoi(terrainDimension->first_attribute(RoomSpec_YAttribute)->value());
	}

	{
		auto npcId = 0;
		auto npc = terrain->first_node(RoomSpec_NPCNode);
		auto npcNode = npc->first_node(RoomSpec_PlayerNode);
		for ( /*npcNode*/; npcNode; npcNode = npcNode->next_sibling(RoomSpec_PlayerNode), npcId++)
		{
			NPC npcInfo;
			npcInfo.Id = atoi(npcNode->first_attribute(RoomSpec_IdAttribute)->value());
			npcInfo.u16XStart = _To_single atof(npcNode->first_node(RoomSpec_StartNode)->first_attribute(RoomSpec_XAttribute)->value());
			npcInfo.u16YStart = _To_single atof(npcNode->first_node(RoomSpec_StartNode)->first_attribute(RoomSpec_YAttribute)->value());
			npcInfo.u16XFinish = _To_single atof(npcNode->first_node(RoomSpec_FinishNode)->first_attribute(RoomSpec_XAttribute)->value());
			npcInfo.u16YFinish = _To_single atof(npcNode->first_node(RoomSpec_FinishNode)->first_attribute(RoomSpec_YAttribute)->value());
			m_terrain.vnPlayers.push_back(npcInfo);
		}
	}

	{
		auto slicesNode = terrain->first_node(RoomSpec_SlicesNode);
		m_terrain.fTileRadii = _To_single atof(slicesNode->first_attribute(RoomSpec_TileRadiiNode)->value());
		auto sliceNode = slicesNode->first_node(RoomSpec_SliceNode);
		for ( /*sliceNode*/; sliceNode; sliceNode = sliceNode->next_sibling(RoomSpec_SliceNode))
		{
			String8 sliceSeq = sliceNode->value();

			PrintToDebugWindowOnly8(sliceSeq.data());

			for ( auto seqElement : sliceSeq ) m_terrain.vnRoom.push_back(seqElement);
		}
	}

	PrintToDebugWindowOnly8("");

	{
		auto textures = xmlRootNode->first_node(RoomSpec_TexturesNode);
		while (textures != nullptr)
		{
			auto clientMatches = strcmp(
				textures->first_attribute(RoomSpec_ClientAttribute)->value(), 
				RoomSpec_Client
				) == 0;

			if (clientMatches)
			{
				auto path = textures->first_node(RoomSpec_PathAttribute);
				for ( /*sliceNode*/; path; path = path->next_sibling(RoomSpec_PathAttribute))
				{
					std::string id = path->first_attribute(RoomSpec_IdAttribute)->value();
					std::string file = path->value();
					m_paths[id] = file;
				}

				break;
			}
			else
			{
				textures = xmlRootNode->next_sibling(RoomSpec_TexturesNode);
			}
		}
	}

	// tiles

	for ( auto y = 0; y < m_terrain.u16YDim; y++) // loop though 
	for ( auto x = 0; x < m_terrain.u16XDim; x++) // all tiles
	{
		Tile tile;
		ZeroTile(&tile);
		tile.X = ( _To_single x - _To_single m_terrain.u16YDim * 0.5f - 0.5f); // * TileRadii;
		tile.Y = -( _To_single m_terrain.u16XDim * 0.5f - _To_single y + 0.5f); // * TileRadii;

		//tile.World = Reign::Matrix::CreateScale(TileRadii);
		tile.World = Reign::Matrix::CreateTranslation(
			tile.X, 1, 
			tile.Y
			);

		m_tiles.push_back(tile);
	}

	for ( auto y = 0; y < m_terrain.u16YDim; y++) // loop though 
	for ( auto x = 0; x < m_terrain.u16XDim; x++) // active tiles
	{
		auto index = y * m_terrain.u16XDim + x;
		//auto index = y * m_terrain.u16YDim + x;
		m_tiles[index].bFordable = m_terrain.vnRoom[index] == RoomSpec_ForableTile 
			? 1 // the sparkle can cross the tile safely
			: 0; // there is smth the sparkle cannot move through

		if (m_tiles[index].bFordable) m_tiles[index].World = 
			Reign::Matrix::CreateTranslation(0.0f, -1, 0.0f) * 
			m_tiles[index].World;
	}

#if 1
	for ( auto y = 0; y < m_terrain.u16YDim; y++)
	{
		for ( auto x = 0; x < m_terrain.u16XDim; x++)
		{
			auto index = y * m_terrain.u16XDim + x;
			//auto index = y * m_terrain.u16YDim + x;

			if (m_tiles[index].bFordable) OutputDebugStringA(" ");
			else OutputDebugStringA("#");
		}

		OutputDebugStringA("\n");
	}
#endif

	xmlDocument.clear();
}