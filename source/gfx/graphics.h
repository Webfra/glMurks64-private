//========================================================================

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "framebuffer.h"
#include "text_screen.h"
#include "rectangle.h"

#include "definitions.h"

#define DRAW_CHARSET (0)

//========================================================================
namespace gfx {

//========================================================================
class Graphics
{
public:
    void init();
    void render();
    void resize_screen(int width, int height);

private:
    int m_Width, m_Height;
#if(DRAW_CHARSET)
    Rectangle charset;
#endif
    text_screen screen;
    text_screen overlay;
    Framebuffer frame;
};

//========================================================================
} // End of namespace gfx

#endif // GRAPHICS_H
