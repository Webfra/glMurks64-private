#ifndef TEXT_SCREEN_H
#define TEXT_SCREEN_H

#include "texture.h"

#include "linmath.h"

//======================================================================
namespace gfx {

//======================================================================
class text_screen
{
public:
    //======================================================================
    void init();
    void render();
    void update_screen(int width, int height);
    void update_memories( uint8_t new_chars[1000] );

private:
    //======================================================================
    Texture chrgen;
    Texture screen;
    Texture colram;
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
    uint8_t chars[1000];    // A buffer representing the text screen.
    uint8_t colrs[1000];    // A buffer representing the color memory.    
    vec2   coords[1000];    // A buffer holding the screen coordinates (0-39,0-24) of each character.

};

//======================================================================
} // End of namespace gfx

#endif // TEXT_SCREEN_H
