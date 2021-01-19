#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//======================================================================
#include "graphics.h"
//======================================================================
#include <SDL2/SDL.h>
#include <glad/glad.h>
//======================================================================
#define SCALING (32)
#define BORDER (60)
#define SCREEN_WIDTH  (40*SCALING + BORDER*2)
#define SCREEN_HEIGHT (25*SCALING + BORDER*2)
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
    SDL_Window *pWin;
    SDL_GLContext gl_context;
    bool run { true };

    gfx::Graphics graphics;

    void load_open_gl(GLADloadproc proc_address);
    bool on_event( SDL_Event &event );
    bool on_keydown( SDL_Event & event );
    void toggle_fullscreen();
    bool on_window_event( SDL_Event & event);
};

#endif // MAINWINDOW_H
//======================================================================
