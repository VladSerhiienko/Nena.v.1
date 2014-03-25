#include "pch.h"
#include "RoomRenderer.h"
#include "GameRenderer.h"

RoomRenderer::RoomRenderer( _In_ ID3D11Device1 *device, _In_ ID3D11DeviceContext1 *context, _In_ IWICImagingFactory2 *factory, _In_ GameRenderer^ gameRenderer) : 
	m_depthRenderer( device, context ), m_shadowRenderer( device, context ), 
	m_d3dDevice( device ), m_d3dContext( context ), m_wicFactory( factory ),
	m_renderSurface0( device, context ),
	m_renderSurface1( device, context ),
	m_renderSurface2( device, context ),
	m_renderSurface3( device, context ),
	m_gameRenderer(gameRenderer),
	m_orientation(XM_PIDIV2),
	m_camera(), m_light(),
	m_fov(XM_PIDIV4),
	m_present(true),
	m_near(0.001f),
	m_update(true),
	m_busy(false),
	m_height(600),
	m_width(800),
	m_far(100.0f)
{
	OutputDebugStringA("<RoomRenderer::RoomRenderer>\n");

	InitializeCriticalSectionEx(&m_lock, 0, 0);

	ZeroValue(m_renderSurface0.EditClear());
	m_renderSurface0.EditClear()->x = 0.2f; // r
	m_renderSurface0.EditClear()->y = 0.2f; // g
	m_renderSurface0.EditClear()->z = 0.2f; // b
	m_renderSurface0.EditClear()->w = 1.0f; // a

	ZeroValue(m_renderSurface1.EditClear());
	m_renderSurface1.EditClear()->x = 0.2f; // r
	m_renderSurface1.EditClear()->y = 0.2f; // g
	m_renderSurface1.EditClear()->z = 0.2f; // b
	m_renderSurface1.EditClear()->w = 1.0f; // a

	ZeroValue(m_renderSurface2.EditClear());
	m_renderSurface2.EditClear()->x = 0.2f; // r
	m_renderSurface2.EditClear()->y = 0.2f; // g
	m_renderSurface2.EditClear()->z = 0.2f; // b
	m_renderSurface2.EditClear()->w = 1.0f; // a

	ZeroValue(m_renderSurface3.EditClear());
	m_renderSurface3.EditClear()->x = 0.2f; // r
	m_renderSurface3.EditClear()->y = 0.2f; // g
	m_renderSurface3.EditClear()->z = 0.2f; // b
	m_renderSurface3.EditClear()->w = 1.0f; // a

	m_camera.Aspect( _To_single m_width / _To_single m_height );
	ZeroValue(m_camera.EditRotation());
	ZeroValue(m_camera.EditFocus());
	m_camera.EditFocus()->x = 0.00f; 
	m_camera.EditFocus()->z = -3.30f; 
	ZeroValue(m_camera.EditEye());
	m_camera.EditEye()->x = -0.001f; 
	m_camera.EditEye()->y = 10.50f;
	m_camera.EditEye()->z = -4.0f;

	ZeroValue(m_camera.EditUp());
	m_camera.EditUp()->y = 1.0f; // y

	m_camera.Near(m_near);
	m_camera.Fov(m_fov);
	m_camera.Far(m_far);

	m_light.Aspect( _To_single m_width / _To_single m_height );
	ZeroValue(m_light.EditAmbient());
	m_light.EditAmbient()->x = 0.15f; // r
	m_light.EditAmbient()->y = 0.15f; // g
	m_light.EditAmbient()->z = 0.15f; // b
	m_light.EditAmbient()->w = 1.10f; // a

	ZeroValue(m_light.EditDiffuse());
	m_light.EditDiffuse()->x = 1.0f; // r
	m_light.EditDiffuse()->y = 1.0f; // g
	m_light.EditDiffuse()->z = 1.0f; // b
	m_light.EditDiffuse()->w = 1.0f; // a

	ZeroValue(m_light.EditFocus());
	m_light.EditFocus()->x = -1.5f; 
	m_light.EditFocus()->y = 0.6f; 
	m_light.EditFocus()->z = 2.0f; 
	ZeroValue(m_light.EditEye());
	m_light.EditEye()->x = 1.5f; // x 
	m_light.EditEye()->y = 1.f; // y
	m_light.EditEye()->z = 1.5f; // z
	//m_light.EditEye()->x = -1.75f; // x 
	//m_light.EditEye()->y = 1.f; // y
	//m_light.EditEye()->z = -1.750f; // z


	ZeroValue(m_light.EditUp());
	m_light.EditUp()->y = 1.0f; // y

	m_light.Near(1.0f);
	m_light.Fov(XM_PI / 6.0f);
	m_light.Far(100.0f);

	m_myLight = m_light;

	for (auto &sparkle : m_sparkles)
		sparkle.m_active = false;
	
	Sparkle sparkle;
	m_light.EditFocus()->x = -1.5f; 
	m_light.EditFocus()->y = 0.6f; 
	m_light.EditFocus()->z = -2.0f; 
	sparkle.m_active = true;
	sparkle.m_sparkleLight = m_light;

	m_sparkles[0] = sparkle;
	m_numSparkles = 1;

	OutputDebugStringA("</RoomRenderer::RoomRenderer>\n");
}

void RoomRenderer::GenerateResources( _In_ Room *room )
{
	OutputDebugStringA("<RoomRenderer::GenerateResources>\n");


	m_roomInfo = room;

	BasicLoader^ loader = ref new BasicLoader(m_d3dDevice.Get(), m_wicFactory.Get());
	{
		BasicShapes^ shapes = ref new BasicShapes(
			m_d3dDevice.Get()
			);
		shapes->CreateCube( //Box( // or box if uncomment both lines
			//float3(TileRadii, TileRadii, TileRadii), 
			m_vertices.GetAddressOf(), 
			m_vertexIndices.GetAddressOf(), 
			&m_boxVertices, 
			&m_boxIndices
			);
	}

	m_shadowRenderer.CreateResources();
	m_depthRenderer.CreateResources();

	m_renderSurface0.CreateResources(
		512.0f, 512.0f, 
		//m_width, m_height, 
		m_near, m_far
		);

	m_renderSurface1.CreateResources(
		512.0f, 512.0f, 
		//m_width, m_height, 
		m_near, m_far
		);

	m_renderSurface2.CreateResources(
		512.0f, 512.0f, 
		//m_width, m_height, 
		m_near, m_far
		);

	m_renderSurface3.CreateResources(
		512.0f, 512.0f, 
		//m_width, m_height, 
		m_near, m_far
		);

	//m_light.EditEye()->x = m_roomInfo->m_tiles[55].World._41 + 0.5f; // x 
	//m_light.EditEye()->y = m_roomInfo->m_tiles[55].World._42 + 0.5f; // x 
	//m_light.EditEye()->z = m_roomInfo->m_tiles[55].World._43 + 0.5f; // x 

	m_camera.Aspect( _To_single m_width / _To_single m_height );
	m_light.Aspect( _To_single m_width / _To_single m_height );

	m_camera.UpdateView();
	m_camera.UpdateProj();

	// Create shaders
	{
		loader->LoadShader(
			"MakeRoundComputeShader.cso", 
			m_rounder.GetAddressOf()
			);
	}

	// Textures and views
	{
		{
			std::wstring texw(room->m_paths["Wall"].begin(), room->m_paths["Wall"].end());
			auto texPath = ref new Platform::String(texw.c_str()); 
			loader->LoadTexture(texPath, m_wallTex.GetAddressOf(), m_wallTexView.GetAddressOf());
		}
		{
			std::wstring texw(room->m_paths["Floor"].begin(), room->m_paths["Floor"].end());
			auto texPath = ref new Platform::String(texw.c_str()); 
			loader->LoadTexture(texPath, m_floorTex.GetAddressOf(), m_floorTexView.GetAddressOf());
		}
	}
	{
		CD3D11_RASTERIZER_DESC desc(D3D11_DEFAULT);
		desc.FrontCounterClockwise = true;

		m_d3dDevice->CreateRasterizerState(&desc, m_rasterizer.GetAddressOf());

	}

	OutputDebugStringA("</RoomRenderer::GenerateResources>\n");
}

// Configures rendering pipeline
void RoomRenderer::OnFrameMove()
{
	static const uint32 s_stride = _To_uint sizeof BasicVertex;
	static const uint32 s_offset = 0u;

	m_camera.UpdateView(); // Generate the camera view matrix
	m_camera.UpdateProj(); // Generate the camera proj matrix

	m_shadowRenderer.EditSpatial()->g_CameraView = m_camera.GetViewCpy();
	m_shadowRenderer.EditSpatial()->g_CameraProj = m_camera.GetProjCpy();

	// Update light transform

	// Configure input assembler for tile rendering
	m_d3dContext->IASetVertexBuffers(0, 1, m_vertices.GetAddressOf(), &s_stride, &s_offset);
	m_d3dContext->IASetIndexBuffer(m_vertexIndices.Get(), DXGI_FORMAT_R16_UINT, 0);
	m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_d3dContext->RSSetState(m_rasterizer.Get());

}

void RoomRenderer::OnMessageReceived(
	_In_ ClientContext^ sender, 
	_In_ ClientContextInput^ eventArguments
	)
{
	EnterCriticalSection(&m_lock);
	int32 sparkleId = 0u;

	m_numSparkles = min(eventArguments->Entries->Length, 4);

	for (int i = 0; i < 4; i++)
	{
		if (i >= eventArguments->Entries->Length)
		{
			m_sparkles[i].m_active = false;
			break;
		}

		auto var = eventArguments->Entries[i];

		Sparkle sparkle;
		sparkle.m_active = true;

		sparkle.m_sparkleLight.Aspect( _To_single m_width / _To_single m_height );
		ZeroValue(m_light.EditAmbient());
		sparkle.m_sparkleLight.EditAmbient()->x = 0.07f; // r
		sparkle.m_sparkleLight.EditAmbient()->y = 0.07f; // g
		sparkle.m_sparkleLight.EditAmbient()->z = 0.07f; // b
		sparkle.m_sparkleLight.EditAmbient()->w = 1.00f; // a

		ZeroValue(m_light.EditDiffuse());
		sparkle.m_sparkleLight.EditDiffuse()->x = 1.0f; // r
		sparkle.m_sparkleLight.EditDiffuse()->y = 1.0f; // g
		sparkle.m_sparkleLight.EditDiffuse()->z = 1.0f; // b
		sparkle.m_sparkleLight.EditDiffuse()->w = 1.0f; // a

		auto rgba = reinterpret_cast<float32*>(&sparkle.m_sparkleLight.EditDiffuse()->x);
		rgba[i % 3] = 0.0f;

		ZeroValue(m_light.EditEye());
		sparkle.m_sparkleLight.EditEye()->x = var.YPosition - (_To_single m_roomInfo->m_terrain.u16YDim) * 0.5f; // x 
		sparkle.m_sparkleLight.EditEye()->y = 1.f; // y
		sparkle.m_sparkleLight.EditEye()->z = -(_To_single m_roomInfo->m_terrain.u16XDim) * 0.5f + var.XPosition; // z

		ZeroValue(m_light.EditFocus());
		sparkle.m_sparkleLight.EditFocus()->y = -5.0f;
		sparkle.m_sparkleLight.EditFocus()->x = 15 * cosf(var.Orientation);// + 0.0001f;
		sparkle.m_sparkleLight.EditFocus()->z = 15 * sinf(var.Orientation);// + 0.0001f;

		ZeroValue(m_light.EditUp());	
		m_light.EditUp()->y = 1.0f; // y

		sparkle.m_sparkleLight.Near(1.0f);
		sparkle.m_sparkleLight.Fov(XM_PI / 4.0f);
		sparkle.m_sparkleLight.Far(100.0f);

		m_sparkles[i] = sparkle;
	}


	LeaveCriticalSection(&m_lock);
}

void RoomRenderer::AttachTo( _In_ ClientContext^ client )
{
	client->MessageReceived += ref new ClientContextInputCallback(
		this, &RoomRenderer::OnMessageReceived
		);
	PrintToDebugWindowOnly8(
		"Renderer is attached to client."
		);
}

boolean RoomRenderer::OnFrame()
{
	EnterCriticalSection(&m_lock);
	//}

	PrintToDebugWindowOnly8("Rendering");

#define Testing0 0

	m_shadowRenderer.EditNumLights()->g_NumSparkles = m_numSparkles;

	for (int i = 0; i < m_numSparkles; i++)
	{
		m_sparkles[i].m_sparkleLight.UpdateView(); // Generate the light view matrix
		m_sparkles[i].m_sparkleLight.UpdateProj(); // Generate the light proj matrix

		m_shadowRenderer.EditLightSpatial()->g_LightPositionWorld[i].x = m_sparkles[i].m_sparkleLight.GetEyeCpy().x;
		m_shadowRenderer.EditLightSpatial()->g_LightPositionWorld[i].y = m_sparkles[i].m_sparkleLight.GetEyeCpy().y;
		m_shadowRenderer.EditLightSpatial()->g_LightPositionWorld[i].z = m_sparkles[i].m_sparkleLight.GetEyeCpy().z;
		m_shadowRenderer.EditLightColors()->g_AmbientColor = m_sparkles[i].m_sparkleLight.GetAmbientCpy();
		m_shadowRenderer.EditLightColors()->g_DiffuseColor[i] = m_sparkles[i].m_sparkleLight.GetDiffuseCpy();
		m_shadowRenderer.EditSpatial()->g_LightView[i] = m_sparkles[i].m_sparkleLight.GetViewCpy();
		m_shadowRenderer.EditSpatial()->g_LightProj[i] = m_sparkles[i].m_sparkleLight.GetProjCpy();
	}

	m_gameRenderer->BeginFrame();
	RenderDepth();
	RenderShadow();
	m_gameRenderer->EndFrame();

	ClientContextOutput out;
	out.Content = m_moving;
	out.XPosition = m_myLight.GetEye()->z + (_To_single m_roomInfo->m_terrain.u16YDim) * 0.5f;
	out.YPosition = (_To_single m_roomInfo->m_terrain.u16XDim) * 0.5f + m_myLight.GetEye()->x;
	out.Orientation = m_orientation;
	out.KeyMask = m_orientation;
	ClientContext::SendMessage(out);
	m_moving *= -1;

	m_update = false;
	m_present = false;
	LeaveCriticalSection(&m_lock);
	return true;
}

void RoomRenderer::RenderShadow()
{
	PrintToDebugWindowOnly8("RenderShadow");

	m_shadowRenderer.SetNumIndices(m_boxIndices);
	m_shadowRenderer.OnFrameMove();

	// Render all the tiles
	for ( auto &tile : m_roomInfo->m_tiles ) RenderShadowForTile(&tile);
	m_shadowRenderer.UnbindRenderTargetFromInput();
}

void RoomRenderer::RenderDepth()
{
	const static ID3D11UnorderedAccessView *s_null[] = { nullptr };
	const static uint32 s_initCount = 0u;

	PrintToDebugWindowOnly8("RenderDepth");
	m_renderSurface0.Unbind();
	m_renderSurface1.Unbind();
	m_renderSurface2.Unbind();
	m_renderSurface3.Unbind();

	if (m_numSparkles >= 1)
	{
		m_depthRenderer.EditSpatial()->g_Proj = m_sparkles[0].m_sparkleLight.GetProjCpy();
		m_depthRenderer.EditSpatial()->g_View = m_sparkles[0].m_sparkleLight.GetViewCpy();

		m_renderSurface0.Bind(); // Set the render target to be the render to texture
		m_renderSurface0.Clear(); // Clear the render to texture

		// Configure pipeline for rendering to the depth texture
		m_depthRenderer.SetNumIndices(m_boxIndices);
		m_depthRenderer.OnFrameMove();

		// Render all the tiles to the depth texture
		for ( auto &tile : m_roomInfo->m_tiles ) 
			RenderDepthForTile(&tile);

		m_renderSurface0.Unbind();
		m_gameRenderer->Bind();

		m_d3dContext->CSSetShader(
			m_rounder.Get(), 
			nullptr, 0u
			);
		m_d3dContext->CSSetUnorderedAccessViews(0, 1, 
			m_renderSurface0.GetWriteViewAddress(), 
			&s_initCount
			);
		m_d3dContext->Dispatch(2, 256, 1);
		m_d3dContext->CSSetUnorderedAccessViews(0, 1, 
			(ID3D11UnorderedAccessView**)s_null, 
			&s_initCount
			);

		m_shadowRenderer.SetDepthTextureView(
			m_renderSurface0.GetReadView(), 0
			);
	}
	else m_shadowRenderer.SetDepthTextureView(nullptr, 0);

	if (m_numSparkles >= 2)
	{
		m_depthRenderer.EditSpatial()->g_Proj = m_sparkles[1].m_sparkleLight.GetProjCpy();
		m_depthRenderer.EditSpatial()->g_View = m_sparkles[1].m_sparkleLight.GetViewCpy();

		m_renderSurface1.Bind(); // Set the render target to be the render to texture
		m_renderSurface1.Clear(); // Clear the render to texture

		// Configure pipeline for rendering to the depth texture
		m_depthRenderer.SetNumIndices(m_boxIndices);
		m_depthRenderer.OnFrameMove();

		// Render all the tiles to the depth texture
		for ( auto &tile : m_roomInfo->m_tiles ) 
			RenderDepthForTile(&tile);

		m_renderSurface1.Unbind();
		m_gameRenderer->Bind();

		m_d3dContext->CSSetShader(
			m_rounder.Get(), 
			nullptr, 0u
			);
		m_d3dContext->CSSetUnorderedAccessViews(0, 1, 
			m_renderSurface1.GetWriteViewAddress(), 
			&s_initCount
			);
		m_d3dContext->Dispatch(2, 256, 1);
		m_d3dContext->CSSetUnorderedAccessViews(0, 1, 
			(ID3D11UnorderedAccessView**)s_null, 
			&s_initCount
			);

		m_shadowRenderer.SetDepthTextureView(m_renderSurface1.GetReadView(), 1);
	}
	else m_shadowRenderer.SetDepthTextureView(nullptr, 1);

	if (m_numSparkles >= 3)
	{
		m_depthRenderer.EditSpatial()->g_Proj = m_sparkles[2].m_sparkleLight.GetProjCpy();
		m_depthRenderer.EditSpatial()->g_View = m_sparkles[2].m_sparkleLight.GetViewCpy();
		m_renderSurface2.Bind(); // Set the render target to be the render to texture
		m_renderSurface2.Clear(); // Clear the render to texture

		// Configure pipeline for rendering to the depth texture
		m_depthRenderer.SetNumIndices(m_boxIndices);
		m_depthRenderer.OnFrameMove();

		// Render all the tiles to the depth texture
		for ( auto &tile : m_roomInfo->m_tiles ) 
			RenderDepthForTile(&tile);

		m_renderSurface2.Unbind();
		m_gameRenderer->Bind();

		m_d3dContext->CSSetShader(
			m_rounder.Get(), 
			nullptr, 0u
			);
		m_d3dContext->CSSetUnorderedAccessViews(0, 1, 
			m_renderSurface2.GetWriteViewAddress(), 
			&s_initCount
			);
		m_d3dContext->Dispatch(2, 256, 1);
		m_d3dContext->CSSetUnorderedAccessViews(0, 1, 
			(ID3D11UnorderedAccessView**)s_null, 
			&s_initCount
			);

		m_shadowRenderer.SetDepthTextureView(
			m_renderSurface2.GetReadView(), 2
			);
	}
	else m_shadowRenderer.SetDepthTextureView(nullptr, 2);

	if (m_numSparkles >= 4)
	{
		m_depthRenderer.EditSpatial()->g_Proj = m_sparkles[3].m_sparkleLight.GetProjCpy();
		m_depthRenderer.EditSpatial()->g_View = m_sparkles[3].m_sparkleLight.GetViewCpy();
		m_renderSurface3.Bind(); // Set the render target to be the render to texture
		m_renderSurface3.Clear(); // Clear the render to texture

		// Configure pipeline for rendering to the depth texture
		m_depthRenderer.SetNumIndices(m_boxIndices);
		m_depthRenderer.OnFrameMove();

		// Render all the tiles to the depth texture
		for ( auto &tile : m_roomInfo->m_tiles ) 
			RenderDepthForTile(&tile);

		m_renderSurface3.Unbind();
		m_gameRenderer->Bind();

		m_d3dContext->CSSetShader(
			m_rounder.Get(), 
			nullptr, 0u
			);
		m_d3dContext->CSSetUnorderedAccessViews(0, 1, 
			m_renderSurface3.GetWriteViewAddress(), 
			&s_initCount
			);
		m_d3dContext->Dispatch(2, 256, 1);
		m_d3dContext->CSSetUnorderedAccessViews(0, 1, 
			(ID3D11UnorderedAccessView**)s_null, 
			&s_initCount
			);

		m_shadowRenderer.SetDepthTextureView(
			m_renderSurface3.GetReadView(), 3
			);
	}
	else m_shadowRenderer.SetDepthTextureView(nullptr, 3);
}

