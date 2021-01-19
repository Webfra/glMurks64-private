R"(
#version 460 core

uniform isampler2D CHARS;   // Screen characters: 1000 bytes
uniform isampler2D COLOR;   // Screen color ram: 1000 nibbles

uniform mat4 MVP;           // Model-View-Projection Matrix (Camera)
uniform vec2 TextOffset;    // Offset on the screen, added to all coordinates.
uniform float scaling;      

in vec2 screen_coord;       // Input: The coordinates (0-39,0-24) of the character to display.

out int character_vs;       // output: the character to display (0-255)
out flat int fg_col_vs;     // output: the foreground color to display (0-15)

void main()                 // Shader: Calculate screen coordinates of the
{                           // vertex from the 3D position.
    gl_Position  = vec4( TextOffset + screen_coord*scaling, 0, 1);
    int index    = int(screen_coord.x) + int(screen_coord.y) * 40;
    ivec2 coord  = ivec2(index,0);
    character_vs = (texelFetch(CHARS, coord, 0 ).r) & 0xFF;
    fg_col_vs    = (texelFetch(COLOR, coord, 0 ).r) & 0x0F;
}

)"
