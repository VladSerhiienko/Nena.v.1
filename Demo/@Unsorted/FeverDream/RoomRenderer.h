#pragma once

#include "pch.h"

#include "Room.h"
#include "RoomLight.h"
#include "RoomCamera.h"
#include "RenderTarget.h"
#include "DepthRenderer.h"
#include "ShadowRenderer.h"
#include "ClientContext.h"

ref class GameRenderer;
ref class RoomRenderer
{
internal:

#pragma region Flat room rendering objects


	REIGN_ALIGN(16)
	struct Spatial
	{
		Reign::Matrix World;
		Reign::Matrix View;
	};
	
	REIGN_ALIGN(16)
	struct Projection
	{
		Reign::Matrix Proj;
	};

	struct Vertex
	{
		Reign::Vector4 PositionWorld;
		Reign::Vector4 NormalWorld;
		Reign::Vector2 TexCoord;
		Int32 TexId;
	};

#pragma endregion

	struct Sparkle // PC/NPC
	{
		bool m_active;
		RoomLight m_sparkleLight;
	};

	RoomRenderer( _In_ ID3D11Device1 *device, _In_ ID3D11DeviceContext1 *context, _In_ IWICImagingFactory2 *factory, _In_ GameRenderer^ gameRenderer );
	void GenerateResources( _In_ Room *room );
	void OnFrameMove();	// This call will configure the pipeline to render the room 
	boolean OnFrame(); // This call will actually render the entire room

	void OnMessageReceived(
		_In_ ClientContext^ sender, 
		_In_ ClientContextInput^ eventArguments
		);
	void AttachTo( 
		_In_ ClientContext^ client 
		);

	REIGN_FORCEINLINING RoomCamera const *EditCamera() const { return &m_camera; }
	REIGN_FORCEINLINING RoomLight const *EditLight() const { return &m_light; }
	REIGN_FORCEINLINING RoomCamera *EditCamera() { return &m_camera; }
	REIGN_FORCEINLINING RoomLight *EditLight() { return &m_light; }
	REIGN_FORCEINLINING void Orientation( _In_ float32 value ) { m_orientation = value; }
	REIGN_FORCEINLINING void Height( _In_ USHORT value ) { m_height = value; }
	REIGN_FORCEINLINING void Width( _In_ USHORT value ) { m_width = value; }
	REIGN_FORCEINLINING void Near( _In_ float32 value ) { m_near = value; }
	REIGN_FORCEINLINING void Fov( _In_ float32 value ) { m_fov = value; }
	REIGN_FORCEINLINING void Far( _In_ float32 value ) { m_far = value; }
	REIGN_FORCEINLINING float32 Orientation() { return m_orientation; }
	REIGN_FORCEINLINING USHORT Height() const { return m_height; }
	REIGN_FORCEINLINING USHORT Width() const { return m_width; }
	REIGN_FORCEINLINING float32 Near() const { return m_near; }
	REIGN_FORCEINLINING float32 Fov() const { return m_fov; }
	REIGN_FORCEINLINING float32 Far() const { return m_far; }

	REIGN_FORCEINLINING void Moving( _In_ boolean value ) { m_moving = value; }
	REIGN_FORCEINLINING boolean Moving() { return m_moving; }

private:

	~RoomRenderer()
	{
		DeleteCriticalSection(&m_lock);
	}

protected private:

	void RenderShadow();
	void RenderDepth();

protected private:

	Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext;
	Microsoft::WRL::ComPtr<IWICImagingFactory2> m_wicFactory;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertices; // cub vertices vs
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexIndices; // cub vertex indices vs
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_wallTex; // texture ps
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_floorTex; // texture ps
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_wallTexView; // ps
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_floorTexView; // ps
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer; // ps

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_rounder;

	CRITICAL_SECTION m_lock;
	Sparkle m_sparkles[4];
	uint32 m_numSparkles;

	ShadowRenderer m_shadowRenderer;
	DepthRenderer m_depthRenderer;
	RenderTarget m_renderSurface0;
	RenderTarget m_renderSurface1;
	RenderTarget m_renderSurface2;
	RenderTarget m_renderSurface3;
	GameRenderer^ m_gameRenderer;
	float32 m_orientation;
	int32 m_moving;
	RoomCamera m_camera;
	RoomLight m_myLight;
	RoomLight m_light;
	uint32 m_boxVertices;
	uint32 m_boxIndices;
	Room *m_roomInfo;
	uint16 m_height;
	uint16 m_width;
	float32 m_near;
	float32 m_fov;
	float32 m_far;
	boolean m_present;
	boolean m_busy;
	bool m_update;


protected private:

	REIGN_FORCEINLINING void RenderShadowForTile( 
		_In_ Room::Tile *tile 
		)
	{
		m_shadowRenderer.SetColorTextureView( 
			!tile->bFordable ? m_floorTexView.Get() 
				: m_wallTexView.Get()
			);

		m_shadowRenderer.EditSpatial()->g_World = tile->World;
		m_shadowRenderer.OnFrame();
	}

	REIGN_FORCEINLINING void RenderDepthForTile( 
		_In_ Room::Tile *tile
		)
	{
		if (tile->bFordable) m_d3dContext->PSSetShaderResources( 0, 1, m_wallTexView.GetAddressOf());
		else m_d3dContext->PSSetShaderResources( 0, 1, m_floorTexView.GetAddressOf());

		m_depthRenderer.EditSpatial()->g_World = tile->World;
		m_depthRenderer.OnFrame();
	}
};
