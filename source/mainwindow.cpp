#include "mainwindow.h"
#include "utils.h"
#include <iostream>


//===================================================================================
// Map SDL Scan codes to "C64 key names"...
enum S {
    // Digits 1-9, 0
    d1 = SDL_SCANCODE_1, d2, d3, d4, d5, d6, d7, d8, d9, d0,

    // The alphabet
    A    = SDL_SCANCODE_A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Z, Y,

    // Function Keys F1 - F4
    F1   = SDL_SCANCODE_F1, F3, F5, F7,

    // Keys that are mapped "naturally"
    retn = SDL_SCANCODE_RETURN,
    Stop = SDL_SCANCODE_ESCAPE,
    del  = SDL_SCANCODE_BACKSPACE,
    Spc  = SDL_SCANCODE_SPACE,

    Ctrl = SDL_SCANCODE_LCTRL,
    lshf = SDL_SCANCODE_LSHIFT,
    rshf = SDL_SCANCODE_RSHIFT,
    home = SDL_SCANCODE_HOME,

    perd = SDL_SCANCODE_PERIOD,
    coma = SDL_SCANCODE_COMMA,

    updn = SDL_SCANCODE_DOWN,       // real: cursor down
    lfrt = SDL_SCANCODE_RIGHT,      // real: cursor right

    // Keys that are mapped weirdly
    plus = SDL_SCANCODE_RIGHTBRACKET,
    mnus = SDL_SCANCODE_SLASH,

    Cbm  = SDL_SCANCODE_LALT,       // real: left alt
    star = SDL_SCANCODE_DELETE,     // Should be apostrophe: '
    larr = SDL_SCANCODE_PAGEDOWN,   // should be....: circumflex
    uarr = SDL_SCANCODE_PAGEUP,     // real: PageUp

    slsh = SDL_SCANCODE_NONUSBACKSLASH, // real: <
    eql  = SDL_SCANCODE_BACKSLASH,      // real: #
    semi = SDL_SCANCODE_APOSTROPHE,     // real: ä
    coln = SDL_SCANCODE_SEMICOLON,      // real: ö
    at   = SDL_SCANCODE_LEFTBRACKET,    // real: ü

    pnd  = SDL_SCANCODE_MINUS,    // real: ß

    restore     = SDL_SCANCODE_PAUSE, // real: Pause

    // Artifical Keys
    reset       = SDL_SCANCODE_F12, // real: F12 (together with Esc)
    crsrUp      = SDL_SCANCODE_UP, // convenience: fake cursor up
    crsrLeft    = SDL_SCANCODE_LEFT, // convenience: fake cursor left
};

//===================================================================================
// Map Scan codes to C64 keyboard matrix (rows/columns)
// http://sta.c64.org/cbm64kbdlay.html
static const unsigned int scan_to_mask[8][8] =
{
    { S::del,  S::retn, S::lfrt, S::F7,   S::F1,   S::F3,   S::F5,   S::updn },
    { S::d3,   S::W,    S::A,    S::d4,   S::Z,    S::S,    S::E,    S::lshf },
    { S::d5,   S::R,    S::D,    S::d6,   S::C,    S::F,    S::T,    S::X    },
    { S::d7,   S::Y,    S::G,    S::d8,   S::B,    S::H,    S::U,    S::V    },
    { S::d9,   S::I,    S::J,    S::d0,   S::M,    S::K,    S::O,    S::N    },
    { S::plus, S::P,    S::L,    S::mnus, S::perd, S::coln, S::at,   S::coma },
    { S::pnd,  S::star, S::semi, S::home, S::rshf, S::eql,  S::uarr, S::slsh },
    { S::d1,   S::larr, S::Ctrl, S::d2,   S::Spc,  S::Cbm,  S::Q,    S::Stop },
};

//===================================================================================
C64::Key ScanCode_to_C64Key( const unsigned int key )
{
    //------------------------------------------------------------------
    for(int row=0; row<8; row++)
        for(int col=0; col<8; col++)
            if(key == scan_to_mask[row][col])
                return { row, col };
    //------------------------------------------------------------------
    return { -1, -1 };
    //------------------------------------------------------------------
}
//========================================================================
bool MainWindow::handle_scan_code( uint32_t scan_code, bool key_state )
{
    //------------------------------------------------------------------
    auto key = ScanCode_to_C64Key( scan_code );
    //------------------------------------------------------------------
    // Negative row means, an unrelated key was pressed.
    if( key.row >= 0)
    {
        c64.set_key_state( key, key_state );
        return true;
    }
    //------------------------------------------------------------------
    return false;
}

//========================================================================
bool MainWindow::keypressed( uint32_t scan_code, bool key_state)
{
    //------------------------------------------------------------------
    // Generate NMI, when "Restore" key is pressed
    if ( key_state && (scan_code == S::restore) )
    {
        nmi6502(&c64);
        return true;
    }
    //------------------------------------------------------------------
    // Generate a RESET, when Esc + F12 is pressed
    if ( key_state && (scan_code == S::reset) )
    {
        // row 7 / col 7 = Run/Stop (aka Escape)
        if( c64.get_key_state( {7,7} ))
        {
            c64.reset = true;
        }
        return true;
    }
    //------------------------------------------------------------------
    // Fake cursor-up and cursor-left for convenience
    if( scan_code == S::crsrUp )
    {
        scan_code = S::updn;
        handle_scan_code( S::rshf, key_state  );
    }
    //------------------------------------------------------------------
    if( scan_code == S::crsrLeft )
    {
        scan_code = S::lfrt;
        handle_scan_code( S::rshf, key_state  );
    }
    //------------------------------------------------------------------
    return handle_scan_code( scan_code, key_state );
    //------------------------------------------------------------------
}



//======================================================================
void GLAPIENTRY MessageCallback(
                 GLenum source,  GLenum type,
                 GLuint id,      GLenum severity,
                 GLsizei length, const GLchar* message,
                 const void* userParam );

//======================================================================
MainWindow::MainWindow()
{
    //------------------------------------------------------------------
    // OpenGL things to do BEFORE creating the SDL window.
    //------------------------------------------------------------------
    SDL_GL_LoadLibrary(NULL); // Default OpenGL is fine.
    //------------------------------------------------------------------
    // Request a core OpenGL 4.6 context
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#if defined(DEBUG)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);

    //------------------------------------------------------------------
    // Creating an SDL window.
    //------------------------------------------------------------------
    pWin = SDL_CreateWindow(
        "glMurks64",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    //------------------------------------------------------------------
    if (pWin == nullptr)
    {
        std::cerr << "***ERROR: Could not create SDL window: " << SDL_GetError() << std::endl;
        exit(-1);
    }
    //------------------------------------------------------------------
    // OpenGL things to do AFTER creating the SDL window.
    //------------------------------------------------------------------
    // Create an OpenGL context for the window.
    gl_context = SDL_GL_CreateContext(pWin);
    if( gl_context == nullptr)
    {
        std::cerr << "***ERROR: Could not create OpenGL context: " << SDL_GetError() << std::endl;
        exit(-1);
    }
    //------------------------------------------------------------------
    // Now load OpenGL
    load_open_gl(SDL_GL_GetProcAddress);
    //------------------------------------------------------------------
    // Enable vsync. (Optional)
    SDL_GL_SetSwapInterval(1);
    //------------------------------------------------------------------
    graphics.init();
    //------------------------------------------------------------------
    // Set the camera to make sure the rectangle can be seen.
    // Must be done whenever the window/screen size changes...
    int width, height;
    SDL_GetWindowSize(pWin, &width, &height);
    graphics.resize_screen(width, height);

    //------------------------------------------------------------------
    // Load the character generator ROM.
    auto basic   { utils::RM.load("roms/basic") };
    auto kernal  { utils::RM.load("roms/kernal") };
    auto chargen { utils::RM.load("roms/chargen") };
    //------------------------------------------------------------------
    c64.init( basic, kernal, chargen );
    c64.paused = false;
}

//======================================================================
MainWindow::~MainWindow()
{
    SDL_DestroyWindow( pWin );
}

//======================================================================
// Perform all the rendering of the C64 screen.
void MainWindow::render_c64()
{
        //------------------------------------------------------------------
        // Get the Bank the VIC-II is working in.
        int vic_bank = 3 - (c64.RAM[0xDD00] & 3);
        // Get the base address of the VIC-II bank.
        int vic_base = vic_bank * 0x4000;
 
        //------------------------------------------------------------------
        // Get the address of the screen RAM
        int screen_base = vic_base + 1024*(c64.RAM[0xD018]>>4);
 
        //------------------------------------------------------------------
        // Get the address of the color RAM
        auto color_base = 0xD800;

        //------------------------------------------------------------------
        // Get the address of the char gen ROM.
        // Select where the char gen will be accessed.
        int cg_select = (c64.RAM[0xD018]>>1) & 7;
        // Startaddress of the char gen (relative to VIC bank address!)
        int cg_offset  = 0x800 * cg_select;
        uint8_t *cg_base = nullptr;
        // In VIC-II banks 0 and 2, the char gen ROM is visible
        // in the address range 0x1000 to 0x1FFF.
        // Otherwise, the VIC uses the RAM.
        if( ( ( vic_bank==0) || (vic_bank==2)) &&
            ( (cg_select>=2) && (cg_select<4)) )
        {
            // VIC-II uses the char gen ROM.
            cg_base = &c64.ROM[0xC000 + cg_offset];
        }
        else
        {
            // VIC-II uses RAM for char gen.
            cg_base = &c64.RAM[vic_base + cg_offset];
        }
        //------------------------------------------------------------------
        // Update the graphics module with the current VIC-II settings.
        graphics.update( &c64.RAM[screen_base],  // Screen RAM
                         &c64.RAM[color_base],   // Color RAM
                         cg_base );              // Char Gen ROM
        //------------------------------------------------------------------
        // Update border and background colors
        graphics.border.set_bg_color( c64.RAM[0xD020] );
        graphics.screen.set_bg_color( c64.RAM[0xD021] );
        //------------------------------------------------------------------
        graphics.render();
}

//======================================================================
void MainWindow::loop()
{
    while( run )
    {
        //------------------------------------------------------------------
        // Process SDL events.
        SDL_Event event;
        while( SDL_PollEvent(&event) )
        {
            on_event( event );
        }
        //------------------------------------------------------------------
        render_c64();
        //------------------------------------------------------------------
        // Make rendered frame visible.
        SDL_GL_SwapWindow(pWin);
        //------------------------------------------------------------------
    }
}

//======================================================================
void MainWindow::load_open_gl( GLADloadproc proc_address )
{
    //------------------------------------------------------------------
    gladLoadGLLoader(proc_address);
    //------------------------------------------------------------------
#if defined(DEBUG)
    //------------------------------------------------------------------
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, this );
    //------------------------------------------------------------------
    // Check OpenGL properties
    printf("OpenGL loaded\n");
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));
    fflush(stdout);
#endif
}

//======================================================================
bool MainWindow::on_event( SDL_Event & event )
{
    // --------------------------------------------------------------
    // C64 simulation needs to be informed about key-down AND key-up events.
    // Therefore, we can't do this in the on_key_down() event function.
    if ( (event.type == SDL_KEYDOWN) or (event.type == SDL_KEYUP))
    {
        auto scan_code = event.key.keysym.scancode;
        auto key_state = (event.type == SDL_KEYDOWN);
        if( keypressed(scan_code, key_state ) )
        {
            return true;
        }
    }
    // --------------------------------------------------------------
    switch( event.type )
    {
    case SDL_QUIT:
        run = false;
        c64.stop();
        return true;
    case SDL_KEYDOWN: return on_keydown( event );
    case SDL_WINDOWEVENT: return on_window_event( event) ;
    }
    return false;
}

//======================================================================
bool MainWindow::on_keydown( SDL_Event & event )
{
    switch( event.key.keysym.sym )
    {
    case SDLK_F8:
        toggle_fullscreen();
        break;
    case SDLK_F11:
        c64.enable_debug_logs = !c64.enable_debug_logs;
        break;
    }
    return false;
}

//======================================================================
void MainWindow::toggle_fullscreen()
{
    // --------------------------------------------------------------
    auto flags { SDL_GetWindowFlags(pWin) & SDL_WINDOW_FULLSCREEN_DESKTOP};
    flags ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDL_SetWindowFullscreen(pWin, flags);
    // --------------------------------------------------------------
    // Inform the graphics module. (Why is the resize event not enough?)
    int w, h;
    SDL_GetWindowSize( pWin, &w, &h );
    graphics.resize_screen(w,h);
    // --------------------------------------------------------------
}

//======================================================================
bool MainWindow::on_window_event( SDL_Event & event)
{
    switch( event.window.type )
    {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        graphics.resize_screen(event.window.data1, event.window.data2 );
        return true;
        break;
    }
    return false;
}

//======================================================================
void GLAPIENTRY MessageCallback(
                 GLenum source,  GLenum type,
                 GLuint id,      GLenum severity,
                 GLsizei length, const GLchar* message,
                 const void* userParam )
{
    (void)source; (void)id; (void)length; (void)userParam;
    //------------------------------------------------------------------
    const char * ctype = "";
    switch(type)
    {
    case GL_DEBUG_TYPE_ERROR:               ctype = "** GL ERROR **"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ctype = "** GL DEPRECATED **"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  ctype = "** GL UNDEFINED **"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         ctype = "** GL PORTABILITY **"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         ctype = "** GL PERFORMANCE **"; break;
    case GL_DEBUG_TYPE_MARKER:              ctype = "** GL MARKER **"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          ctype = "** GL PUSH **"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           ctype = "** GL POP **"; break;
    case GL_DEBUG_TYPE_OTHER:               ctype = "** GL OTHER **"; break;
    default:                                ctype = "** GL UNKNOWN **"; break;
    }
    //------------------------------------------------------------------
#if 1 // Only show errors?
    if (type != GL_DEBUG_TYPE_ERROR ) return;
#endif
    //------------------------------------------------------------------
    printf("-----------------------\nGL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                      ctype,      type,         severity,      message );
    fflush(stdout);
    if( type == GL_DEBUG_TYPE_ERROR )
        throw(-1);
    //------------------------------------------------------------------
}
//======================================================================
