//========================================================================

#include "framebuffer.h"

#include "rectangle.h"
#include "gfx_utils.h"
#include <glad/glad.h>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace gfx {


    //========================================================================
    void Framebuffer::init(GLsizei w, GLsizei h)
    {
        m_Width = w;
        m_Height = h;
        // --------------------------------------------------------------
        // Generate a framebuffer and bind it.
        glGenFramebuffers(1,&framebuffer_name);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_name);
        // --------------------------------------------------------------
        // Generate a texture for the framebuffer.
        Rect.tex.gen().activate(0).bind(GL_TEXTURE_2D)
            .iformat(GL_RGB).size(w,h).format(GL_RGB).type(GL_UNSIGNED_BYTE).TexImage2D()
            .Pi(GL_TEXTURE_WRAP_S, GL_CLAMP)
            .Pi(GL_TEXTURE_WRAP_T, GL_CLAMP)
            .Pi(GL_TEXTURE_MIN_FILTER, GL_LINEAR) //GL_LINEAR_MIPMAP_LINEAR)
            .Pi(GL_TEXTURE_MAG_FILTER, GL_LINEAR) //GL_LINEAR_MIPMAP_LINEAR)
            //.GenerateMipMap() // Doesn't seem to work properly...?
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
        Rect.init( 0,0, m_Width, m_Height );
        // --------------------------------------------------------------
    }
    //========================================================================
    // Activate the Framebuffer, so that following draw calls go on the 
    // Framebuffer, not on the screen.
    void Framebuffer::activate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_name);
    }
    //========================================================================
    // Deactivate the Framebuffer, draw calls go to the screen again.
    void Framebuffer::deactivate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    //========================================================================
    // Render the content of the Framebuffer (on the screen).
    void Framebuffer::render()
    {
        Rect.render();
    }
    //========================================================================
    // React to changes of the screen size. 
    // Keep the Framebuffer maximized on the screen, but keep its 
    // aspect ratio intact.
    void Framebuffer::resize_screen(int width, int height)
    {
        // --------------------------------------------------------------
        // "original" coordinate system of the framebuffer.
        Rect2D<float> rst { 0.0f, 0.0f, float(m_Width), float(m_Height) };

        // --------------------------------------------------------------
        // Adjust the coordinate system to fit on the screen, keeping 
        // the aspect ratio for the framebuffer intact.
        adjust_aspect( rst, float(width) / float(height) );

        // --------------------------------------------------------------
        // Calculate the MVP such that it covers the extended coordinate system.
        auto MVP { glm::ortho( rst.x, rst.x + rst.w,
                               rst.y, rst.y + rst.h,
                                1.0f, -1.0f) };
        // --------------------------------------------------------------
        Rect.SetMVP( MVP );
        // --------------------------------------------------------------
    }

} // End of namespace gfx.

//========================================================================
// End of file.
//========================================================================
