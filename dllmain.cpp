#include <misc/includes.hpp>
#include <misc/utilities.h>
#include <misc/xor.h>
#include <cheat.h>   

typedef HRESULT( __stdcall* present_fn ) ( IDXGISwapChain*, UINT, UINT );
typedef LRESULT( __stdcall* WNDPROC )( HWND, UINT, WPARAM, LPARAM );
typedef unsigned __int64 _QWORD;

ID3D11RenderTargetView* render_target;
ID3D11DeviceContext* win_ctx;
ID3D11Device* device;
present_fn o_present;
WNDPROC o_wndproc;
HWND window = 0;

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT __stdcall WndProc( const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if ( uMsg == WM_KEYDOWN && LOWORD( wParam ) == VK_HOME )
        g_globals.menu_open = !g_globals.menu_open;

    ImGuiIO& io = ImGui::GetIO( );
    POINT position;

    GetCursorPos( &position ); 
    ScreenToClient( window, &position );
    io.MousePos.x = ( float )position.x;
    io.MousePos.y = ( float )position.y;

    if( g_globals.menu_open )
    {
        ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam );
        return true;
    }

    return CallWindowProc( o_wndproc, hWnd, uMsg, wParam, lParam );
}

HRESULT __fastcall present_hook( IDXGISwapChain* pSwapChain, unsigned int SyncInterval, unsigned int Flags )
{
    static bool first = false;
    if ( !first )
    {
        if ( SUCCEEDED( pSwapChain->GetDevice( __uuidof( ID3D11Device ), ( void** )&device ) ) )
        {
            device->GetImmediateContext( &win_ctx );
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc( &sd );
            window = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer;

            pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
            device->CreateRenderTargetView( pBackBuffer, NULL, &render_target );
            pBackBuffer->Release( );

            o_wndproc = ( WNDPROC )SetWindowLongPtr( window, GWLP_WNDPROC, ( LONG_PTR )WndProc );

            ImGui::CreateContext( );
            ImGuiIO& io = ImGui::GetIO( );
            io.Fonts->AddFontFromFileTTF( _( "C:\\Windows\\Fonts\\Tahoma.ttf" ), 16.0f );
            ImGui_ImplWin32_Init( window );
            ImGui_ImplWin32_Init( window );
            ImGui_ImplDX11_Init( device, win_ctx );

            first = true;
        }
        else
            return o_present( pSwapChain, SyncInterval, Flags );
    }

    if ( device || win_ctx )
    {
        ID3D11Texture2D* renderTargetTexture = nullptr;
        if ( !render_target )
        {
            if ( SUCCEEDED( pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<LPVOID*>( &renderTargetTexture ) ) ) )
            {
                device->CreateRenderTargetView( renderTargetTexture, nullptr, &render_target );
                renderTargetTexture->Release( );
            }
        }
    }

    ImGui_ImplDX11_NewFrame( );
    ImGui_ImplWin32_NewFrame( );
    ImGui::NewFrame( );
    if ( g_globals.menu_open )
    {
        ImGui::GetIO( ).MouseDrawCursor = 1;

        ImGui::Begin( _( "24" ) );
        
        ImGui::Checkbox( _( "kill all (f1)" ), &kill_all );
        ImGui::Checkbox( _( "teleport only aim target" ), &aim_target );

        ImGui::Checkbox( _( "draw aim fov" ), &show_aim_fov );
        ImGui::Checkbox( _( "enable aimbot" ), &aimbot_enabled );
        ImGui::SliderFloat( _( "aim fov" ), &aim_fov, 0, 350 );

        ImGui::Checkbox( _( "aim at nearest bone" ), &aim_at_nearest_bone );
        if( !aim_at_nearest_bone )
            ImGui::SliderInt( _( "aim bone" ), &aim_hitbox, 0, 5 );

        ImGui::SliderInt( _( "aim smooth" ), &smoothness_amount, 0, 15 );

        ImGui::Checkbox( _( "draw esp" ), &esp );
        ImGui::Checkbox( _( "draw box" ), &draw_box );
        
        ImGui::Checkbox( _( "draw skeleton" ), &draw_skeleton );
        ImGui::Checkbox( _( "draw line" ), &draw_lines );
        ImGui::Checkbox( _( "draw teammates" ), &draw_teammates );
        ImGui::End( );
    }
    else
        ImGui::GetIO( ).MouseDrawCursor = 0;

    start_loop( );

    ImGui::EndFrame( );
    win_ctx->OMSetRenderTargets( 1, &render_target, NULL );
    ImGui::Render( );

    if ( render_target )
    {
        render_target->Release( );
        render_target = nullptr;
    }

    ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

    return o_present( pSwapChain, SyncInterval, Flags );
}

/* no comments. */
void hook( __int64 addr, __int64 func, __int64* orig )
{
    static uintptr_t hook_addr = g_globals.sig_scan( _( "GameOverlayRenderer64.dll" ), _( "48 ? ? ? ? 57 48 83 EC 30 33 C0" ) );
    if ( !hook_addr ) 
        return;

    auto hook = ( ( __int64( __fastcall* )( __int64, __int64, __int64*, __int64 ) )( hook_addr ) );
    hook( addr, func, orig, 1 );
}

void init( ) {
    if ( !GetModuleHandleA( _( "GameOverlayRenderer64.dll" ) ) )
        return;

    uintptr_t dxgi_present = g_globals.sig_scan( _( "GameOverlayRenderer64.dll" ), _( "48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC 20 41 8B E8" ) );
    if ( !dxgi_present )
        return;

    uintptr_t base = reinterpret_cast< uintptr_t >( GetModuleHandle( NULL ) );

    hook( dxgi_present, ( __int64 )present_hook, ( __int64* )&o_present );
}

bool DllMain( void* module, unsigned long reason, void* reserved ) {
    if( reason == 1 )
    {
        init( );
        return true;
    }

    return false;
}