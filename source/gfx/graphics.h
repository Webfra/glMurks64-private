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

GLuint compile_shader(GLenum type, const char * code );
GLuint link_program( GLuint vxs_id, GLuint fts_id, GLuint gms_id = 0 );

//========================================================================
class Graphics
{
public:
    void init();
    void render();
    void resize_screen(int width, int height);
private:

    Rectangle charset;
    text_screen screen;
};

//========================================================================
} // End of namespace gfx

#endif // GRAPHICS_H
