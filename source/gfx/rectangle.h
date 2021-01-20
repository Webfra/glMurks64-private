#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "texture.h"

#include "linmath.h"

//========================================================================
namespace gfx {

//========================================================================
class Rectangle
{
public:
    void init( GLfloat x, GLfloat y, GLfloat w, GLfloat h );
    void render();
    void resize_screen(int width, int height);

    void SetMVP( mat4x4 &MVP)
    {
        glUseProgram( program_id );
        glUniformMatrix4fv( loc_MVP, 1, false, &MVP[0][0]);
    }

    Texture tex;
public:
    GLuint program_id {0};
    GLint loc_TEX;
    GLint loc_MVP;
    GLuint vertex_array_id;
};

//========================================================================
} // End of namespace gfx

#endif // RECTANGLE_H
