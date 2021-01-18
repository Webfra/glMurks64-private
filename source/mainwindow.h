#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//======================================================================
#include "graphics.h"
//======================================================================
#include <SDL2/SDL.h>
#include <glad/glad.h>
//======================================================================
#define SCREEN_WIDTH  (1024)
#define SCREEN_HEIGHT  (786)
//======================================================================
class MainWindow
{
public:
    MainWindow();
    ~MainWindow();
    void loop();

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
