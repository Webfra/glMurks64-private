#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "rectangle.h"
#include "gfx_utils.h"
#include <glad/glad.h>
#include <stdexcept>

//========================================================================
namespace gfx {

//========================================================================
class Framebuffer
{
public:
    //========================================================================
    // Initialize the framebuffer with the given width and height.
    void init(GLsizei width, GLsizei height);
    //========================================================================
    // Activate the Framebuffer, so that following draw-calls go on the 
    // Framebuffer, not on the screen.
    void activate();
    //========================================================================
    // Deactivate the Framebuffer, draw-calls go to the screen again.
    void deactivate();
    //========================================================================
    // Render the content of the Framebuffer (on the screen).
    void render();
    //========================================================================
    // React to changes of the screen size. 
    // Keep the Framebuffer maximized on the screen, but keep its 
    // aspect ratio intact.
    void resize_screen(int width, int height);
    //========================================================================
    Rectangle Rect; // Provides a texture and a rectangle shader for drawing the framebuffer on the screen.
    //========================================================================
private:
    GLuint framebuffer_name {0};
};

//========================================================================
} // End of namespace gfx

#endif // FRAMEBUFFER_H
