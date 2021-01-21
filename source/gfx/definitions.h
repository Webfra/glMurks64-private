#pragma once

#include <glad/glad.h>
#include <array>

//========================================================================
namespace gfx {

//========================================================================
typedef GLint ivec3[3];
extern std::array<ivec3, 16> color_table;

//========================================================================
template<typename T>
struct Rect2D { T x, y, w, h; };

//========================================================================
GLuint compile_shader(GLenum type, const char * code );
GLuint link_program( GLuint vxs_id, GLuint fts_id, GLuint gms_id = 0 );

} // End of namespace gfx

//========================================================================
// End of file.
//========================================================================
