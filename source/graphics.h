#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "text_screen.h"
#include "texture.h"
#include "linmath.h"

#include <glad/glad.h>

//========================================================================
namespace gfx {

GLuint compile_shader(GLenum type, const char * code );

//========================================================================
class Rectangle
{
public:
    void init( const Texture * texture, GLfloat x, GLfloat y, GLfloat w, GLfloat h );
    void render();
    void update_screen(int width, int height);
public:
    const Texture *tex {nullptr};
    GLuint program_id;
    GLint loc_TEX;
    GLint loc_MVP;
    GLuint vertex_array_id;
};

//========================================================================
class Graphics
{
public:
    void init();
    void render();
    void update_screen(int width, int height)
    {
        //------------------------------------------------------------------
        glViewport(0, 0, width, height);
        charset.update_screen(width, height);
        screen.update_screen(width, height);
    }
private:
    Texture texture;
    text_screen screen;
    Rectangle charset;
};

//========================================================================
} // End of namespace gfx

#endif // GRAPHICS_H
