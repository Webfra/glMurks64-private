#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "text_screen.h"
#include "rectangle.h"

#include <glad/glad.h>
#include <array>

//========================================================================
namespace gfx {

typedef GLint ivec3[3];
extern std::array<ivec3, 16> color_table;

//========================================================================
GLuint compile_shader(GLenum type, const char * code );
GLuint link_program( GLuint vxs_id, GLuint fts_id, GLuint gms_id = 0 );

//========================================================================
template<typename T> struct Rect2D { T x, y, w, h; };

//========================================================================
template<typename T>
Rect2D<T> adapt_aspect( const Rect2D<T> &org, T target_aspect, T org_aspect);

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
            .Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST)
            .Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
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
        Rect.init( 0,0,1,1 );
        // --------------------------------------------------------------
    }
    //========================================================================
    void activate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_name);
        glViewport( 0,0, Rect.tex.width(), Rect.tex.height() );
    }
    //========================================================================
    void deactivate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    //========================================================================
    void render()
    {
        Rect.render();
    }
    //========================================================================
    void resize_screen(int width, int height)
    {
        // --------------------------------------------------------------
        // "Target" aspect ratio of the C64 screen.
        float target_aspect = float(384) / float(272);
        // Current window aspect ratio (after resize.)
        float window_aspect = float(width) / float(height);
        float screen_stretch = 1.08;
        // --------------------------------------------------------------
        // Allow the user to change the target aspect ration.
        // 1.0 = real C64 framebuffer aspect ratio.
        target_aspect /= float(screen_stretch);

        // --------------------------------------------------------------
        // The shader that renders the framebuffer on the screen also needs
        // to be adjusted to keep the real aspect ratio.
        {
            // --------------------------------------------------------------
            // Screen coordinates expected by the shader.
            gfx::Rect2D<float> org { 0,0,1,1 };
            // modified screen coordinates
            auto rst { gfx::adapt_aspect<float>(org, window_aspect, target_aspect) };
            mat4x4 FB_MVP;
            mat4x4_ortho(FB_MVP,
                         float(rst.x), float(rst.x + rst.w),
                         float(rst.y), float(rst.y + rst.h),
                         1.0f, -1.0f);
            // --------------------------------------------------------------
            Rect.SetMVP( FB_MVP );
            // --------------------------------------------------------------
        }

    }
    //========================================================================
    Rectangle Rect; // Provides a texture and a rectangle shader for drawing the framebuffer on the screen.
    //========================================================================
private:
    GLuint framebuffer_name {0};
};

//========================================================================
class Graphics
{
public:
    void init();
    void render();
    void resize_screen(int width, int height);

private:
    int m_Width, m_Height;
    Rectangle charset;
    text_screen screen;
    Framebuffer frame;
};

//========================================================================
} // End of namespace gfx

#endif // GRAPHICS_H
