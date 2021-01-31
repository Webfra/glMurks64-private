#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//======================================================================
#include "graphics.h"
#include "c64.h"
//======================================================================
#include <SDL2/SDL.h>
#include <glad/glad.h>
//======================================================================
// Note: A SCALING of 8 means characters are 8x8 pixels in size.
#define SCALING (8)
#define SCREEN_WIDTH  (384*3)
#define SCREEN_HEIGHT (272*3)
//======================================================================
class MainWindow
{
public:
    MainWindow();
    ~MainWindow();
    void loop();
    void close()
    {
        SDL_Event ev { SDL_QUIT };
        SDL_PushEvent( &ev);
    }

private:
    C64 c64;

    SDL_Window *pWin;
    SDL_GLContext gl_context;
    bool run { true };

    gfx::Graphics graphics;

    void load_open_gl(GLADloadproc proc_address);
    bool on_event( SDL_Event &event );
    bool on_keydown( SDL_Event & event );
    bool on_keyup( SDL_Event & event );
    void toggle_fullscreen();
    bool on_window_event( SDL_Event & event);

    bool handle_scan_code( uint32_t scan_code, bool key_state );
    bool keypressed( uint32_t scan_code, bool key_state);

    void render_c64();
};

#endif // MAINWINDOW_H
//======================================================================
