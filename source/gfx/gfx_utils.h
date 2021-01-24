#pragma once
//========================================================================

#include "utils.h"

//========================================================================

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

//========================================================================
#include <array>

//========================================================================
namespace gfx {

//========================================================================
// The C64 color table.
extern std::array<glm::ivec3, 16> color_table;

//========================================================================
// A rectangle definition I thought I would need more than once.
template<typename T>
struct Rect2D { T x, y, w, h; };

//========================================================================
// Extend the given rectangle "org_rect" such, that it has the given
// target aspect ratio.
// MODIFIES org_rect!
void adjust_aspect(Rect2D<float> &org_rect, float target_aspect);

#if 0 // No longer used.
//========================================================================
// Read the character set of the C64 ROM and prepare a texture image.
// The texture will be 16x16 characters = 128x128 pixels
// Each pixel is 1 byte in the texture image. (Instead of 1 bit in the character rom.)
void prepare_charset( uint8_t char_rom[], GLchar image[128][128] );
#endif

//========================================================================
} // End of namespace gfx

//========================================================================
// End of file.
//========================================================================
