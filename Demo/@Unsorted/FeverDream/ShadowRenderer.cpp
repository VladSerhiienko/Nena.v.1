#include "pch.h"
#include "ShadowRenderer.h"
#include "BasicLoader.h"

ShadowRenderer::ShadowRenderer( _In_ ID3D11Device1 *d3dDevice, _In_ ID3D11DeviceContext1 *d3dContext ) : 
	m_d3dDevice(d3dDevice), m_d3dContext(d3dContext)
{
}

void ShadowRenderer::OnFrameMove( _In_ ShadowRenderer::TimerView *timer )
{
	(void)timer;

	m_d3dContext->IASetInputLayout(m_shadowInputLayout.Get()); // vs in
	m_d3dContext->VSSetShader(m_shadowVertexShader.Get(), nullptr, 0u); // vs

	m_d3dContext->PSSetShader(m_shadowPixelShader.Get(), nullptr, 0u); // ps
	m_d3dContext->PSSetConstantBuffers(0, 1, m_lightColors.GetAddressOf()); // cb
	m_d3dContext->PSSetShaderResources(0, ARRAYSIZE(m_shadowPixelShaderTextureViewArray), m_shadowPixelShaderTextureViewArray); // t
	m_d3dContext->PSSetSamplers(0, 2, m_shadowPixelShaderSamplerArray); // s
}

void ShadowRenderer::UnbindRenderTargetFromInput()
{
	static ID3D11ShaderResourceView *s_null = nullptr;
	m_d3dContext->PSSetShaderResources(1, 1, &s_null); // t
}

void ShadowRenderer::CreateResources()
{
	BasicLoader^ loader = ref new BasicLoader(m_d3dDevice.Get());
	{ // Load vertex shader
		loader->LoadShader(
			"ShadowVertexShader.cso", nullptr, 0, 
			m_shadowVertexShader.GetAddressOf(),
			m_shadowInputLayout.GetAddressOf()
			);
	}
	{ // Load pixel shader
		loader->LoadShader(
			"ShadowPixelShader.cso",
			m_shadowPixelShader.GetAddressOf()
			);
	}
	{ // Load spatial const buffer
		ZeroValue(&m_spatialData); m_updateSpatial = false;
		BasicLoader::CreateConstantBuffer<Spatial>(
			m_d3dDevice.Get(), m_spatial.GetAddressOf(), 
			&m_spatialData
			);
	}
	{ // Load light spatial const buffer
		ZeroValue(&m_lightSpatialData); m_updateLightSpatial = false;
		BasicLoader::CreateConstantBuffer<LightSpatial>(
			m_d3dDevice.Get(), m_lightSpatial.GetAddressOf(), 
			&m_lightSpatialData
			);
	}
	{ // Load light colors const buffer
		ZeroValue(&m_lightColorsData); m_updateLightColors = false;
		BasicLoader::CreateConstantBuffer<LightColors>(
			m_d3dDevice.Get(), m_lightColors.GetAddressOf(), 
			&m_lightColorsData
			);
	}
	{ // Load light colors const buffer
		ZeroValue(&m_numLightsData); m_updateLightColors = false;
		BasicLoader::CreateConstantBuffer<NumLights>(
			m_d3dDevice.Get(), m_numLights.GetAddressOf(), 
			&m_numLightsData
			);
	}
	{ // Load samplers
		D3D11_SAMPLER_DESC samplerDesc; ZeroValue(&samplerDesc);
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;????????? ? ??? ????????? ? ??????? .xml.
??????? «??????? ???????»:
<?xml version="1.0"?>

<!--***************
USE UNICODE ONLY
****************-->

<Room>

  <!--Terrain (client/implementation-independent)-->
  <Terrain Version="1.0.0.0">
    
    <!--***************************************************************
    MaxAllowedX="256" MaxAllowedY="256" are version dependent values.
    So far they cannot be changed for current version unless the whole 
    developer team commit this change.
    ****************************************************************-->

    <Dimension X="10" Y="7" MaxAllowedX="256" MaxAllowedY="256" Origin="TopLeft"/>

    <Verification Of="Dimension">
      <AssertOn ValueAt="Dimension.X" IsLessThenValueAt="Dimension.MaxAllowedX" Type="UInt32">
        <Message Content="Room width exceeds max allowed value."/>
      </AssertOn>
      <AssertOn ValueAt="Dimension.X" IsGreaterThenValue="3" Type="UInt32">
        <Message Content="Room width is less then min allowed value."/>
      </AssertOn>
      <AssertOn ValueAt="Dimension.Y" IsLessThenValueAt="Dimension.MaxAllowedY" Type="UInt32">
        <Message Content="Room height exceeds max allowed value."/>
      </AssertOn>
      <AssertOn ValueAt="Dimension.Y" IsGreaterThenValue="3" Type="UInt32">
        <Message Content="Room height is less then min allowed value."/>
      </AssertOn>
    </Verification>
    
    <NPC>
      <Player Id="0">
        <Start X="1.5" Y="1.5"/>
        <Finish X="1.5" Y="8.5"/>
      </Player>
      <Player Id="1">
        <Start X="2.5" Y="1.5"/>
        <Finish X="2.5" Y="8.5"/>
      </Player>
    </NPC>
    
    <Slices Transpose="False" TileRadii="10.0">
      <!--            0123456789              -->
      <!--0--> <Slice>##########</Slice> <!--0-->
      <!--1--> <Slice>#  # ## ##</Slice> <!--1-->
      <!--2--> <Slice>#   #   ##</Slice> <!--2-->
      <!--3--> <Slice>##   # # #</Slice> <!--3-->
      <!--4--> <Slice>#        #</Slice> <!--4-->
      <!--5--> <Slice># # ##   #</Slice> <!--5-->
      <!--6--> <Slice>##########</Slice> <!--6-->
      <!--            0123456789              -->
    </Slices>
  </Terrain>

  <!--***************************************************************
  Check: 
  (1) whether the players will not appear inside the wall tile
  (2) whether the players can actually reach the finish tile
  (3) room is a closed area (walls.. walls everywhere...)
  ****************************************************************-->
  <!--Not implemented-->
  <Verification Of="Player.Start"></Verification>
  <Verification Of="Player.Finish"></Verification>

  <!--Images (client/implementation-dependent)-->
  <Textures Client="Windows.Version.1.0.0.0">
    <Path Id="Wall" ShaderResourceId="1">Assets/Game/ground2.bmp</Path>    <!--1 tex-->
    <Path Id="Floor" ShaderResourceId="0">Assets/Game/cellwall.jpg</Path>    <!--0 tex-->
    <Path Id="Ceiling" ShaderResourceId="2">Assets/Game/cellfloor.bmp</Path>    <!--2 tex-->
  </Textures>
  
  <!--AI related to the room (common for all clients)-->
  <AI>
    <!--Bot startup location-->
    <!--Treasure location-->
    <!--Light locations-->
    <!--HP locations-->
    <!--Bonuses-->
  </AI>
</Room>
???????. . ?????? ????????????? ????? ??? ????????? ? ???.

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
???? ??? ???????????? ????? ??? «?????? ???????» (?????-????).

??? ???????????? .xml ?????? ?? ?++ ? ?????? ?????????? RapidXML. ?? ??????? ????????, ?? ???????? ???????????? ?? ????????? ??????????????.

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
???? ??? ???????????? ????? ??? «?????? ???????» (??????????).

#pragma once

#include "pch.h"

class RoomLight
{
private:

	Reign::Vector4 m_ambientColor;
	Reign::Vector4 m_diffuseColor;
	Reign::Vector3 m_focus;
	Reign::Vector3 m_eye;
	Reign::Vector3 m_up;
	Reign::Matrix m_view;
	Reign::Matrix m_proj;
	float32 m_aspect;
	float32 m_near;
	float32 m_far;
	float32 m_fov;

public:
	
	REIGN_FORCEINLINING Reign::Matrix const *GetView() const { return &m_view; }
	REIGN_FORCEINLINING Reign::Matrix GetViewCpy() const { return m_view; }
	REIGN_FORCEINLINING Reign::Matrix *EditView() { return &m_view; }
	REIGN_FORCEINLINING Reign::Matrix const *GetProj() const { return &m_proj; }
	REIGN_FORCEINLINING Reign::Matrix GetProjCpy() const { return m_proj; }
	REIGN_FORCEINLINING Reign::Matrix *EditProj() { return &m_proj; }
	REIGN_FORCEINLINING Reign::Vector4 const *GetAmbient() const {  return &m_ambientColor; }
	REIGN_FORCEINLINING Reign::Vector4 GetAmbientCpy() const { return m_ambientColor; }
	REIGN_FORCEINLINING Reign::Vector4 *EditAmbient() { return &m_ambientColor; }
	REIGN_FORCEINLINING Reign::Vector4 const *GetDiffuse() const {  return &m_diffuseColor; }
	REIGN_FORCEINLINING Reign::Vector4 GetDiffuseCpy() const { return m_diffuseColor; }
	REIGN_FORCEINLINING Reign::Vector4 *EditDiffuse() { return &m_diffuseColor; }
	REIGN_FORCEINLINING Reign::Vector3 const *GetEye() const { return &m_eye; }
	REIGN_FORCEINLINING Reign::Vector3 GetEyeCpy() const { return m_eye; }
	REIGN_FORCEINLINING Reign::Vector3 *EditEye() { return &m_eye; }
	REIGN_FORCEINLINING Reign::Vector3 const *GetFocus() const { return &m_focus; }
	REIGN_FORCEINLINING Reign::Vector3 GetFocusCpy() const { return m_focus; }
	REIGN_FORCEINLINING Reign::Vector3 *EditFocus() { return &m_focus; }
	REIGN_FORCEINLINING Reign::Vector3 const *GetUp() const { return &m_up; }
	REIGN_FORCEINLINING Reign::Vector3 GetUpCpy() const { return m_up; }
	REIGN_FORCEINLINING Reign::Vector3 *EditUp() { return &m_up; }
	REIGN_FORCEINLINING float32 Aspect() const { return m_aspect; }
	REIGN_FORCEINLINING void Aspect( _In_ float32 value ) { m_aspect = value; }
	REIGN_FORCEINLINING float32 Near() const { return m_near; }
	REIGN_FORCEINLINING void Near( _In_ float32 value ) { m_near = value; }
	REIGN_FORCEINLINING float32 Fov() const { return m_fov; }
	REIGN_FORCEINLINING void Fov( _In_ float32 value ) { m_fov = value; }
	REIGN_FORCEINLINING float32 Far() const { return m_far; }
	REIGN_FORCEINLINING void Far( _In_ float32 value ) { m_far = value; }

	RoomLight();

	inline void UpdateView( )
	{
		m_view = Reign::Matrix::CreateLookAt(
			m_eye, m_focus, m_up
			);
	}

	inline void UpdateProj( )
	{
		m_proj = Reign::Matrix::CreatePerspectiveFieldOfView(
			m_fov, m_aspect, m_near, m_far
			);
	}

};
???? ??? ????????????? ?????? ? ?????????.

#pragma once

#include "pch.h"

class RoomCamera
{
private:

	Reign::Vector3 m_rotation;
	Reign::Vector3 m_focus;
	Reign::Vector3 m_eye;
	Reign::Vector3 m_up;
	Reign::Matrix m_view;
	Reign::Matrix m_proj;
	float32 m_aspect;
	float32 m_near;
	float32 m_far;
	float32 m_fov;

public:
	
	REIGN_FORCEINLINING Reign::Matrix const *GetView() const { return &m_view; }
	REIGN_FORCEINLINING Reign::Matrix GetViewCpy() const { return m_view; }
	REIGN_FORCEINLINING Reign::Matrix *EditView() { return &m_view; }
	REIGN_FORCEINLINING Reign::Matrix const *GetProj() const { return &m_proj; }
	REIGN_FORCEINLINING Reign::Matrix GetProjCpy() const { return m_proj; }
	REIGN_FORCEINLINING Reign::Matrix *EditProj() { return &m_proj; }

	REIGN_FORCEINLINING Reign::Vector3 const *GetRotation() const { return &m_rotation; }
	REIGN_FORCEINLINING Reign::Vector3 GetRotationCpy() const { return m_rotation; }
	REIGN_FORCEINLINING Reign::Vector3 *EditRotation() { return &m_rotation; }

	REIGN_FORCEINLINING Reign::Vector3 const *GetEye() const { return &m_eye; }
	REIGN_FORCEINLINING Reign::Vector3 GetEyeCpy() const { return m_eye; }
	REIGN_FORCEINLINING Reign::Vector3 *EditEye() { return &m_eye; }
	REIGN_FORCEINLINING Reign::Vector3 const *GetFocus() const { return &m_focus; }
	REIGN_FORCEINLINING Reign::Vector3 GetFocusCpy() const { return m_focus; }
	REIGN_FORCEINLINING Reign::Vector3 *EditFocus() { return &m_focus; }
	REIGN_FORCEINLINING Reign::Vector3 const *GetUp() const { return &m_up; }
	REIGN_FORCEINLINING Reign::Vector3 GetUpCpy() const { return m_up; }
	REIGN_FORCEINLINING Reign::Vector3 *EditUp() { return &m_up; }
	
	REIGN_FORCEINLINING float32 Aspect() const { return m_aspect; }
	REIGN_FORCEINLINING void Aspect( _In_ float32 value ) { m_aspect = value; }
	REIGN_FORCEINLINING float32 Near() const { return m_near; }
	REIGN_FORCEINLINING void Near( _In_ float32 value ) { m_near = value; }
	REIGN_FORCEINLINING float32 Fov() const { return m_fov; }
	REIGN_FORCEINLINING void Fov( _In_ float32 value ) { m_fov = value; }
	REIGN_FORCEINLINING float32 Far() const { return m_far; }
	REIGN_FORCEINLINING void Far( _In_ float32 value ) { m_far = value; }

	REIGN_FORCEINLINING void SetRotationAngles( Reign::Vector3 const &rotation) { m_rotation = rotation * 0.0174532925f; }
	REIGN_FORCEINLINING void SetRotationAngles( Reign::Vector3 const *rotation) { SetRotationAngles(*rotation); }

public:

	RoomCamera();

	REIGN_FORCEINLINING void UpdateView( )
	{
		//auto const rotationMatrix = Matrix::CreateFromYawPitchRoll(
		//	m_rotation.y,	// yaw
		//	m_rotation.x,	// pitch
		//	m_rotation.z	// roll
		//	);

		//m_focus = DirectX::XMVector3TransformCoord(
		//	m_focus, rotationMatrix
		//	);

		//m_focus += m_eye;
		//m_up = DirectX::XMVector3TransformCoord(
		//	m_up, rotationMatrix
		//	);

		m_view = Reign::Matrix::CreateLookAt(
			m_eye, m_focus, m_up
			);
	}

	inline void UpdateProj( )
	{
		m_proj = Reign::Matrix::CreatePerspectiveFieldOfView(
			m_fov, m_aspect, m_near, m_far
			);
	}

};
???? ??? ????????????? ???????.

#pragma once

#include "pch.h"

ref class ShadowRenderer
{
#pragma region Timer
private: typedef Application::Utility::Timing::TimerView TimerView;
#pragma endregion

private:

	REIGN_ALIGN(16)
	struct Spatial
	{
		Reign::Matrix g_World;
		Reign::Matrix g_CameraView;
		Reign::Matrix g_CameraProj;
		Reign::Matrix g_LightView[4];
		Reign::Matrix g_LightProj[4];
	} m_spatialData; // vs

	REIGN_ALIGN(16)
	struct LightSpatial
	{
		//uint32 g_NumSparkles[4];
		Reign::Vector4 g_LightPositionWorld[4];
	} m_lightSpatialData; // vs

	REIGN_ALIGN(16)
	struct NumLights
	{
		uint32 g_NumSparkles;
	} m_numLightsData; // vs

	REIGN_ALIGN(16)
	struct LightColors
	{
		Reign::Vector4 g_AmbientColor;
		Reign::Vector4 g_DiffuseColor[4];
	} m_lightColorsData; // vs

	boolean m_updateLightSpatial; // vs
	boolean m_updateLightColors; // vs
	boolean m_updateSpatial; // vs
	uint32 m_numIndices;

	// From the external source

	Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice; // d3d
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext; // d3d

	// Internally created

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_clampSampler; // ps
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_wrapSampler; // ps
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shadowVertexShader; // vs
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_shadowInputLayout; // vs il
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shadowPixelShader; // ps
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightSpatial; // vs
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightColors; // ps
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spatial; // vs
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_numLights; // vs

	// Convinient way to config vertex shader stage const buffers.
	ID3D11Buffer *m_shadowVertexShaderBufferArray[3]; // vs
	// Convinient way to config pixel shader stage samplers.
	ID3D11SamplerState *m_shadowPixelShaderSamplerArray[2]; // ps
	// Convinient way to config pixel shader stage texture views.
	ID3D11ShaderResourceView *m_shadowPixelShaderTextureViewArray[5]; // ps

internal:

	REIGN_FORCEINLINING LightSpatial const *GetLightSpatial() const { return &m_lightSpatialData; }
	REIGN_FORCEINLINING LightSpatial GetLightSpatialCpy() const { return m_lightSpatialData; }
	REIGN_FORCEINLINING LightSpatial *EditLightSpatial() { return &m_lightSpatialData; }

	REIGN_FORCEINLINING NumLights const *GetNumLights() const { return &m_numLightsData; }
	REIGN_FORCEINLINING NumLights GetNumLightsCpy() const { return m_numLightsData; }
	REIGN_FORCEINLINING NumLights *EditNumLights() { return &m_numLightsData; }

	REIGN_FORCEINLINING LightColors const *GetLightColors() const { return &m_lightColorsData; }
	REIGN_FORCEINLINING LightColors GetLightColorsCpy() const { return m_lightColorsData; }
	REIGN_FORCEINLINING LightColors *EditLightColors() { m_updateLightColors = true; return &m_lightColorsData; }

	REIGN_FORCEINLINING Spatial const *GetSpatial() const { return &m_spatialData; }
	REIGN_FORCEINLINING Spatial GetSpatialCpy() const { return m_spatialData; }
	REIGN_FORCEINLINING Spatial *EditSpatial() { return &m_spatialData; }

	REIGN_HINTINLINING uint32 GetNumIndices( ) const { return m_numIndices; }
	REIGN_HINTINLINING void SetNumIndices( _In_ uint32 numIndices ) { m_numIndices = numIndices; }

	REIGN_FORCEINLINING void OnFrame() 
	{ 
		m_d3dContext->VSSetConstantBuffers(0, 3, m_shadowVertexShaderBufferArray); // cb
		m_d3dContext->PSSetShaderResources(0, 5, m_shadowPixelShaderTextureViewArray); // t

		/*if (m_updateSpatial)*/ 
		m_d3dContext->UpdateSubresource(
			m_spatial.Get(), 0, nullptr, 
			(&m_spatialData), 0, 0
			);
		/*if (m_updateLightSpatial)*/ 
		m_d3dContext->UpdateSubresource(
			m_lightSpatial.Get(), 0, nullptr, 
			(&m_lightSpatialData), 0, 0
			);
		/*if (m_updateLightColors)*/ 
		m_d3dContext->UpdateSubresource(
			m_lightColors.Get(), 0, nullptr, 
			(&m_lightColorsData), 0, 0
			);
		m_d3dContext->UpdateSubresource(
			m_numLights.Get(), 0, nullptr, 
			(&m_numLightsData), 0, 0
			);
		m_d3dContext->DrawIndexed(
			m_numIndices, 0, 0
			); 
	}

	ShadowRenderer( 
		_In_ ID3D11Device1 *d3dDevice, 
		_In_ ID3D11DeviceContext1 *d3dContext
		); 

	void OnFrameMove( 
		_In_ ShadowRenderer::TimerView *timer = nullptr
		);

	REIGN_FORCEINLINING void SetColorTextureView(
		_In_ ID3D11ShaderResourceView *texView
		)
	{
		m_shadowPixelShaderTextureViewArray[0] = texView; // t0
	}

	REIGN_FORCEINLINING void SetDepthTextureView(
		_In_ ID3D11ShaderResourceView *texView, _In_ uint32 index
		)
	{
		m_shadowPixelShaderTextureViewArray[index + 1] = texView; // t1
	}

	void CreateResources(
		);

	void UnbindRenderTargetFromInput();

};
???? ??? ?????????? ???????????? ????? (?????-????).

#include "pch.h"
#include "ShadowRenderer.h"
#include "BasicLoader.h"

ShadowRenderer::ShadowRenderer( _In_ ID3D11Device1 *d3dDevice, _In_ ID3D11DeviceContext1 *d3dContext ) : 
	m_d3dDevice(d3dDevice), m_d3dContext(d3dContext)
{
}

void ShadowRenderer::OnFrameMove( _In_ ShadowRenderer::TimerView *timer )
{
	(void)timer;

	m_d3dContext->IASetInputLayout(m_shadowInputLayout.Get()); // vs in
	m_d3dContext->VSSetShader(m_shadowVertexShader.Get(), nullptr, 0u); // vs

	m_d3dContext->PSSetShader(m_shadowPixelShader.Get(), nullptr, 0u); // ps
	m_d3dContext->PSSetConstantBuffers(0, 1, m_lightColors.GetAddressOf()); // cb
	m_d3dContext->PSSetShaderResources(0, ARRAYSIZE(m_shadowPixelShaderTextureViewArray), m_shadowPixelShaderTextureViewArray); // t
	m_d3dContext->PSSetSamplers(0, 2, m_shadowPixelShaderSamplerArray); // s
}

void ShadowRenderer::UnbindRenderTargetFromInput()
{
	static ID3D11ShaderResourceView *s_null = nullptr;
	m_d3dContext->PSSetShaderResources(1, 1, &s_null); // t
}

void ShadowRenderer::CreateResources()
{
	BasicLoader^ loader = ref new BasicLoader(m_d3dDevice.Get());
	{ // Load vertex shader
		loader->LoadShader(
			"ShadowVertexShader.cso", nullptr, 0, 
			m_shadowVertexShader.GetAddressOf(),
			m_shadowInputLayout.GetAddressOf()
			);
	}
	{ // Load pixel shader
		loader->LoadShader(
			"ShadowPixelShader.cso",
			m_shadowPixelShader.GetAddressOf()
			);
	}
	{ // Load spatial const buffer
		ZeroValue(&m_spatialData); m_updateSpatial = false;
		BasicLoader::CreateConstantBuffer<Spatial>(
			m_d3dDevice.Get(), m_spatial.GetAddressOf(), 
			&m_spatialData
			);
	}
	{ // Load light spatial const buffer
		ZeroValue(&m_lightSpatialData); m_updateLightSpatial = false;
		BasicLoader::CreateConstantBuffer<LightSpatial>(
			m_d3dDevice.Get(), m_lightSpatial.GetAddressOf(), 
			&m_lightSpatialData
			);
	}
	{ // Load light colors const buffer
		ZeroValue(&m_lightColorsData); m_updateLightColors = false;
		BasicLoader::CreateConstantBuffer<LightColors>(
			m_d3dDevice.Get(), m_lightColors.GetAddressOf(), 
			&m_lightColorsData
			);
	}
	{ // Load light colors const buffer
		ZeroValue(&m_numLightsData); m_updateLightColors = false;
		BasicLoader::CreateConstantBuffer<NumLights>(
			m_d3dDevice.Get(), m_numLights.GetAddressOf(), 
			&m_numLightsData
			);
	}
	{ // Load samplers
		D3D11_SAMPLER_DESC samplerDesc; ZeroValue(&samplerDesc);
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		DX::ThrowIfFailed(m_d3dDevice->CreateSamplerState(&samplerDesc, m_wrapSampler.GetAddressOf()));

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		DX::ThrowIfFailed(m_d3dDevice->CreateSamplerState(&samplerDesc, m_clampSampler.GetAddressOf()));

		m_shadowPixelShaderSamplerArray[0] = m_clampSampler.Get();
		m_shadowPixelShaderSamplerArray[1] = m_wrapSampler.Get();
	}

	m_shadowVertexShaderBufferArray[0] = m_spatial.Get();
	m_shadowVertexShaderBufferArray[1] = m_lightSpatial.Get();
	m_shadowVertexShaderBufferArray[2] = m_numLights.Get();
}


???? ??? ???????????? ????? (??????????).
















		DX::ThrowIfFailed(m_d3dDevice->CreateSamplerState(&samplerDesc, m_wrapSampler.GetAddressOf()));

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		DX::ThrowIfFailed(m_d3dDevice->CreateSamplerState(&samplerDesc, m_clampSampler.GetAddressOf()));

		m_shadowPixelShaderSamplerArray[0] = m_clampSampler.Get();
		m_shadowPixelShaderSamplerArray[1] = m_wrapSampler.Get();
	}

	m_shadowVertexShaderBufferArray[0] = m_spatial.Get();
	m_shadowVertexShaderBufferArray[1] = m_lightSpatial.Get();
	m_shadowVertexShaderBufferArray[2] = m_numLights.Get();
}

