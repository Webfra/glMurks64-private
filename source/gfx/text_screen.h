#ifndef TEXT_SCREEN_H
#define TEXT_SCREEN_H

#include "texture.h"
#include "gfx_utils.h"
#include "utils.h"

//======================================================================
namespace gfx {

//======================================================================
class text_screen
{
public:
    //========================================================================
    text_screen() = default;
    NO_COPY( text_screen );
    NO_MOVE( text_screen );
    virtual ~text_screen() = default;
    //======================================================================
    void init( utils::Buffer &CG, int rows, int cols, const glm::vec2 &pos );
    void set_memories( uint8_t *new_chars, uint8_t *new_colrs );
    void set_bg_color( int bg_color );
    void render();
    void resize_screen( int width, int height );


private:
    int m_Rows, m_Cols; // Number of rows and columns of the text screen.
    //======================================================================
    Texture chrgen;     // A Texture to hold the character generator ROM
    Texture screen;     // A Texture to hold the 1000 bytes of screen RAM
    Texture colram;     // A Texture to hold the 1000 nibbles of color RAM.
    //======================================================================
    //GLuint program_id;
    Shader program;
    GLuint vertex_array_id;
    GLuint vertex_buffer_id;
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
