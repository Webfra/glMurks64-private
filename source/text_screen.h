#ifndef TEXT_SCREEN_H
#define TEXT_SCREEN_H

#include "texture.h"

#include "linmath.h"

//------------------------------------------------------------------
namespace gfx {

//------------------------------------------------------------------
class text_screen
{
public:
    //------------------------------------------------------------------
    void init();
    void render();

    //------------------------------------------------------------------
    void update_screen(int width, int height)
    {
        //------------------------------------------------------------------
        mat4x4 MVP;
        mat4x4_ortho( MVP, 0, width, height, 0, 1, -1 );
        //------------------------------------------------------------------
        glUseProgram( program_id );
        glUniformMatrix4fv( loc_MVP, 1, false, &MVP[0][0]);
    }

private:
    //------------------------------------------------------------------
    Texture chrgen;
    Texture screen;
    Texture colram;
    //------------------------------------------------------------------
    GLuint program_id;
    GLuint vertex_array_id;
    GLint loc_coord;
    //------------------------------------------------------------------
    GLint loc_MVP;
    GLint loc_TEX;
    GLint loc_CHARS;
    GLint loc_COLOR;
    GLint loc_pallette;
    GLint loc_bg_color;
    GLint loc_Offset;
    GLint loc_scaling;
    GLint loc_charset;
    //------------------------------------------------------------------
    // A buffer representing the text screen.
    uint8_t chars[1000];
    uint8_t colrs[1000];
    // A buffer holding the screen coordinates (0-39,0-24) of each character.
    vec2 screen_coords[1000];

};

} // End of namespace gfx

#endif // TEXT_SCREEN_H
