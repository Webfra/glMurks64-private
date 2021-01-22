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
    void init(GLsizei w, GLsizei h)
    {
        // --------------------------------------------------------------
        // Generate a framebuffer and bind it.
        glGenFramebuffers(1,&framebuffer_name);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_name);
        // --------------------------------------------------------------
        // Generate a texture for the framebuffer.
        Rect.tex.gen().activate(0).bind(GL_TEXTURE_2D)
            .iformat(GL_RGB).size(w,h).format(GL_RGB).type(GL_UNSIGNED_BYTE).Image2D( nullptr )
            .Pi(GL_TEXTURE_WRAP_S, GL_CLAMP)
            .Pi(GL_TEXTURE_WRAP_T, GL_CLAMP)
            .Pi(GL_TEXTURE_MIN_FILTER, GL_LINEAR) //GL_LINEAR_MIPMAP_LINEAR)
            .Pi(GL_TEXTURE_MAG_FILTER, GL_LINEAR) //GL_LINEAR_MIPMAP_LINEAR)
            //.GenerateMipMap()
        .unbind();
        // --------------------------------------------------------------
        // Assign the texture to the frame buffer.
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Rect.tex, 0);
        if( GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
        {
            throw std::runtime_error("Framebuffer could not be completed!");
        }
        // --------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // --------------------------------------------------------------
        // Create the rectangle for drawing the framebuffer on the screen.
        Rect.init( 0,0, w, h );
        // --------------------------------------------------------------
    }
    //========================================================================
    // Activate the Framebuffer, so that following draw calls go on the 
    // Framebuffer, not on the screen.
    void activate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_name);
        glViewport( 0,0, Rect.tex.width(), Rect.tex.height() );
    }
    //========================================================================
    // Deactivate the Framebuffer, draw calls go to the screen again.
    void deactivate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    //========================================================================
    // Render the content of the Framebuffer (on the screen).
    void render()
    {
        Rect.render();
    }
    //========================================================================
    // React to changes of the screen size. 
    // Keep the Framebuffer maximized on the screen, but keep its 
    // aspect ratio intact.
    void resize_screen(int width, int height)
    {
        // --------------------------------------------------------------
        // "original" coordinate system of the framebuffer.
        Rect2D<float> rst { 0.0f, 0.0f, float(Rect.tex.width()), float(Rect.tex.height()) };

        // Adjust the coordinate system to fit on the screen, keeping 
        // the aspect ratio for the framebuffer intact.
        adjust_aspect( rst, float(width) / float(height) );

        // --------------------------------------------------------------
        mat4x4 MVP;
        mat4x4_ortho(MVP, float(rst.x), float(rst.x + rst.w),
                          float(rst.y), float(rst.y + rst.h),
                          1.0f, -1.0f);
        // --------------------------------------------------------------
        Rect.SetMVP( MVP );
        // --------------------------------------------------------------
    }
    //========================================================================
    Rectangle Rect; // Provides a texture and a rectangle shader for drawing the framebuffer on the screen.
    //========================================================================
private:
    GLuint framebuffer_name {0};
};

//========================================================================
} // End of namespace gfx

#endif // FRAMEBUFFER_H
