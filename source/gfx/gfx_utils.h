#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
// Compile the given shader code of the given shader type.
// Returns the shader id ("name").
GLuint compile_shader(GLenum type, const char * code );
// Link the given shaders into a shader program.
// Returns the program id ("name").
GLuint link_program( GLuint vxs_id, GLuint fts_id, GLuint gms_id = 0 );

//========================================================================
// Extend the given rectangle "org_rect" such, that it has the given
// target aspect ratio.
// MODIFIES org_rect!
void adjust_aspect(Rect2D<float> &org_rect, float target_aspect);

//========================================================================
// Read the character set out of C64 ROM and prepare a texture image.
// The texture will be 16x16 characters = 128x128 pixels
// Each pixel is 1 byte in the texture image. (Instead of 1 bit in the character rom.)
void prepare_charset( uint8_t char_rom[], GLchar image[128][128] );

} // End of namespace gfx

//========================================================================
// End of file.
//========================================================================
