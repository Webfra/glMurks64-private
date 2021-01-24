//========================================================================

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "framebuffer.h"
#include "text_screen.h"
#include "rectangle.h"

#include "gfx_utils.h"

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

    text_screen screen;
    text_screen border;
    Framebuffer frame;
};

//========================================================================
} // End of namespace gfx

#endif // GRAPHICS_H
