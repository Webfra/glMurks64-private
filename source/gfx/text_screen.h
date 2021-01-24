#ifndef TEXT_SCREEN_H
#define TEXT_SCREEN_H

#include "texture.h"
#include "utils.h"
#include "linmath.h"

//======================================================================
namespace gfx {

#if 0
#define COLS (384/8)
#define ROWS (272/8)
#define MAX_CHARS ( COLS * ROWS )
#endif

//======================================================================
class text_screen
{
public:
    //======================================================================
    void init( utils::Buffer &CG, int rows, int cols, const vec2 &pos );
    void render();
    void resize_screen(int width, int height);
    void set_memories( uint8_t *new_chars, uint8_t *new_colrs );
    void set_bg_color( int bg_color );

    int m_Rows, m_Cols;

private:
    //======================================================================
    Texture chrgen;     // A Texture to hold the character generator ROM
    Texture screen;     // A Texture to hold the 1000 bytes of screen RAM
    Texture colram;     // A Texture to hold the 1000 nibbles of color RAM.
    //======================================================================
    GLuint program_id;
    GLuint vertex_array_id;
    GLint loc_coord;        // Location of shader input "screen_coord"
    //======================================================================
    GLint loc_MVP;          // Location of uniform MVP
    GLint loc_TEX;          // Location of texture for character generator
    GLint loc_CHARS;        // Location of texture for screen memory
    GLint loc_COLOR;        // Location of texture for color memory
    GLint loc_palette;      // Location of color table (16 x vec3)
    GLint loc_bg_color;     // Location of background color (0-15)
    GLint loc_Offset;       // Location of Offset coordinate (ivec2)
    GLint loc_scaling;
    GLint loc_charset;
    //======================================================================
};

//======================================================================
} // End of namespace gfx

#endif // TEXT_SCREEN_H
