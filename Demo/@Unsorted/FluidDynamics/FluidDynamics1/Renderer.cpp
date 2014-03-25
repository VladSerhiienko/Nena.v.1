#include "Precompiled.h"
#include "Renderer.h"

Sandbox::Renderer::Renderer( ) :
	DirectXBaseDesktop( ),
	m_fluidRenderer( )
{
	PrintToDebugWindowOnly8( "Sandbox. Creating renderer." );
	InitializeCriticalSection( &m_lock );
}

Sandbox::Renderer::~Renderer( )
{
	PrintToDebugWindowOnly8( "Sandbox. Deleting renderer." );
	DeleteCriticalSection( &m_lock );
}

void Sandbox::Renderer::CreateDeviceResources( )
{
	PrintToDebugWindowOnly8( "Sandbox. Creating device resources." );

	{ // Create rasterizer state
		Rendering::RasterizerStateDescrition rasterDesc;
		ZeroValue( &rasterDesc );

		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = true;
		//rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		Sehnsucht::ThrowIfFailed( m_d3dDevice->CreateRasterizerState1(
			&rasterDesc, &m_rasterizerCcw
			) );
	}
	{ // Create alpha on blend state
		Rendering::BlendStateDescrition blendStateDescription;
		ZeroValue( &blendStateDescription );

		// Create an alpha enabled blend state description.
		blendStateDescription.RenderTarget[ 0 ].BlendEnable = TRUE;
		blendStateDescription.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDescription.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[ 0 ].RenderTargetWriteMask = 0x0f;

		m_d3dDevice->CreateBlendState1( &blendStateDescription, m_blendingEnabled.GetAddressOf( ) ); // !supp win7
	}
}

void Sandbox::Renderer::CreateDeviceIndependentResources( )
{
	PrintToDebugWindowOnly8( "Sandbox. Creating device independent resources." );
}

void Sandbox::Renderer::CreateWindowSizeDependentResources( )
{
	PrintToDebugWindowOnly8( "Sandbox. Creating window size dependent resources." );
}

void Sandbox::Renderer::Initialize( )
{
	PrintToDebugWindowOnly8( "Sandbox. Initializing renderer." );
	EnterCriticalSection( &m_lock );

	DirectXBaseDesktop::Initialize( );

	CreateDeviceIndependentResources( );
	CreateDeviceResources( );
	CreateWindowSizeDependentResources( );

	m_clear.x = 0.4f;
	m_clear.y = 0.4f;
	m_clear.z = 0.4f;
	m_clear.w = 1.0f;

	m_camera.Aspect( m_d3dRenderTargetSize.width / m_d3dRenderTargetSize.height );
	m_camera.Fov( Reign::XM_PIDIV4 );
	m_camera.Near( 100.0f );
	m_camera.Far( 0.01f );
	( *m_camera.EditFocus( ) ) = Reign::Vector3( 0.0f, 0.0f, 10.0f );
	( *m_camera.EditEye( ) ) = Reign::Vector3( 0.0f, 0.0f, -8.0f );

	m_camera.UpdateProj( );
	m_camera.UpdateView( );

	m_fluidRenderer.EditSolver( )->PoissonSolverGridStep( 1.0f );
	m_fluidRenderer.EditSolver( )->ProblemTimeStep( 0.001f );
	m_fluidRenderer.EditSolver( )->ProblemHeight( 150 );
	m_fluidRenderer.EditSolver( )->ProblemWidth( 150 );
	//m_fluidRenderer.EditSolver( )->ProblemHeight( 64 );
	//m_fluidRenderer.EditSolver( )->ProblemWidth( 64 );
	m_fluidRenderer.EditSolver( )->Viscosity( 0.01f );

	m_fluidRenderer.WICFactory( m_wicFactory.Get( ) );
	m_fluidRenderer.Context3( m_d3dContext.Get( ) );
	m_fluidRenderer.Device3( m_d3dDevice.Get( ) );
	m_fluidRenderer.Initialize( );

	m_d3dContext->RSSetState( m_rasterizerCcw.Get( ) );

	BindToPipeline( m_d3dContext.Get( ) );

	LeaveCriticalSection( &m_lock );
}

void Sandbox::Renderer::UpdateForWindowSizeChange( )
{
	PrintToDebugWindowOnly8( "Sandbox. Updating renderer on window size changed." );
	EnterCriticalSection( &m_lock );

	DirectXBaseDesktop::UpdateForWindowSizeChange( );

	m_windowSizeChangedArgs.Dim.y = m_d3dRenderTargetSize.height;
	m_windowSizeChangedArgs.Dim.x = m_d3dRenderTargetSize.width;
	m_windowSizeChangedArgs.Dpi.y = m_dpi;
	m_windowSizeChangedArgs.Dpi.x = m_dpi;

	m_fluidRenderer.WICFactory( m_wicFactory.Get( ) );
	m_fluidRenderer.Context3( m_d3dContext.Get( ) );
	m_fluidRenderer.Device3( m_d3dDevice.Get( ) );
	m_fluidRenderer.UpdateForWindowSizeChange(
		m_windowSizeChangedArgs
		);

	BindToPipeline( m_d3dContext.Get( ) );

	LeaveCriticalSection( &m_lock );
}