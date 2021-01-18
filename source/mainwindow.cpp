#include "mainwindow.h"
#include "utils.h"
#include <iostream>

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
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
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
    graphics.update_screen(width, height);
    //------------------------------------------------------------------
}

//======================================================================
MainWindow::~MainWindow()
{
    SDL_DestroyWindow( pWin );
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
        // TODO: Render here
        glClear( GL_COLOR_BUFFER_BIT );
        //------------------------------------------------------------------
        graphics.render();
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
    printf("Shader language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION) );

    GLint maxVertUniformsVect;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertUniformsVect);
    std::cout << "Maximum uniform vectors: " << maxVertUniformsVect << std::endl;

    fflush(stdout);
#endif
}

//======================================================================
bool MainWindow::on_event( SDL_Event & event )
{
    switch( event.type )
    {
    case SDL_QUIT: run = false; return true;
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
    case SDLK_RETURN:
        if( (event.key.keysym.mod & KMOD_ALT) )
            toggle_fullscreen();
        break;
    }
    return false;
}

//======================================================================
void MainWindow::toggle_fullscreen()
{
    auto flags { SDL_GetWindowFlags(pWin) & SDL_WINDOW_FULLSCREEN_DESKTOP};
    flags ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDL_SetWindowFullscreen(pWin, flags);
}

//======================================================================
bool MainWindow::on_window_event( SDL_Event & event)
{
    switch( event.window.type )
    {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        graphics.update_screen(event.window.data1, event.window.data2 );
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
#if 0
    if (type != GL_DEBUG_TYPE_ERROR ) return;
#endif
    //------------------------------------------------------------------
    printf("-----------------------\nGL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                      ctype,      type,         severity,      message );
    fflush(stdout);
    //------------------------------------------------------------------
}
//======================================================================
