#pragma once

#include "pch.h"

class Room
{
public:

	struct Tile;
	typedef std::vector<Tile> TileCollection;
	typedef std::map<String8, String8> TexPathCollection;
	typedef concurrency::concurrent_vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> TexCollection;
	typedef concurrency::concurrent_vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> SRVCollection;
	typedef concurrency::concurrent_vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>> UAVCollection;

	struct NPC
	{
		uint16 Id;
		float32 u16XStart, u16YStart;
		float32 u16XFinish, u16YFinish;
	};

	struct Terrain
	{
		typedef std::vector<char> Map;
		typedef std::vector<NPC> Players;

		float32 fTileRadii;
		uint16 u16XDim, u16YDim;
		Players vnPlayers;
		Map vnRoom;
	};


	struct Tile
	{
		float32 X;
		float32 Y;
		Reign::Matrix World;
		int32 bFordable;
	};

	inline static void ZeroTile(Tile *tile)
	{
		ZeroValue(tile);
	}

public:

	Room();
	void ReadFrom(
		_In_ String8 spec // xml
		);

	//Microsoft::WRL::ComPtr<ID3D11Buffer> m_tiles;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_tilesRO;
	//Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_tilesRW;

	Terrain m_terrain;
	TileCollection m_tiles;
	//TexCollection m_textures;
	TexPathCollection m_paths;
	//SRVCollection m_textureSRVs;
	//UAVCollection m_textureUAVs;

};