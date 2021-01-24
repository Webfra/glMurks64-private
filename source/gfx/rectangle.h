#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "texture.h"
#include "shader.h"
#include "gfx_utils.h"

//========================================================================
namespace gfx {

//========================================================================
class Rectangle
{
public:
    void init( GLfloat x, GLfloat y, GLfloat w, GLfloat h );
    void render();
    void resize_screen(int width, int height);
    void SetMVP( const glm::mat4 &MVP)
    {
        glUseProgram(shader);
        glUniformMatrix4fv( loc_MVP, 1, false, &MVP[0][0]);
    }

    Texture tex;
    Shader shader;
private:
    GLint loc_TEX { -1 };
    GLint loc_MVP { -1 };
    GLuint vertex_array_id { 0 };
    GLuint vertex_buffer_id { 0 };
};

//========================================================================
} // End of namespace gfx

#endif // RECTANGLE_H
