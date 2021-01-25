
#include "text_screen.h"
#include "texture.h"
#include "gfx_utils.h"
#include "utils.h"
#include <glad/glad.h>
#include <iostream>

namespace gfx {

//========================================================================
// A vertex shader for the text screen.
static const char *vxs =
//#include "text_screen_shaders.vs"

R"(
#version 460 core

uniform isampler2D CHARS;   // Screen characters: 1000 bytes
uniform isampler2D COLOR;   // Screen color ram: 1000 nibbles

uniform mat4 MVP;           // Model-View-Projection Matrix (Camera)
uniform vec2 TextOffset;    // Offset on the screen, added to all coordinates.
uniform float scaling;      

in vec3 screen_coord;       // Input: The xy-coordinates of the character to display, 
                            //             z is the index in screen and color RAM.

out int character_vs;       // output: the character to display (0-255)
out flat int fg_col_vs;     // output: the foreground color to display (0-15)

void main()                 // Shader: Calculate screen coordinates of the
{                           // vertex from the 3D position.
    gl_Position  = vec4( TextOffset + (screen_coord.xy)*scaling, 0, 1);
    ivec2 coord  = ivec2( int(screen_coord.z),0);
    character_vs = (texelFetch(CHARS, coord, 0 ).r) & 0xFF;
    fg_col_vs    = (texelFetch(COLOR, coord, 0 ).r) & 0x0F;
}

)"

;

static const char *gms =
// #include "text_screen_shaders.gs"

R"(
#version 460 core

uniform mat4 MVP;           // Model-View-Projection Matrix (Camera)
uniform float scaling;
    
layout ( points ) in;       // Input: Each vertex is a point.
in int character_vs[];      // Input: the character to display.
in int fg_col_vs[];         // Input: the foreground color to use

layout ( triangle_strip, max_vertices = 4 ) out; // Output: a triangle strip of 4 vertices as output.
out vec2 texcoord;          // Output: The texture coordinate.
out flat int fg_col_gs;     // Output: The foreground color to use
out flat int char_gs;       // Output: The character to display.

// Emit a single vertex.
// x and y designate the corner of the character. Must be 0 or 1.
void emit_vertex(float x, float y)
{
    // The relative position of the vertex in pixels.
    vec2 rel = vec2(x,y) * scaling;

    // The absolute output position: Input + relative position
    gl_Position = MVP * ( gl_in[0].gl_Position + vec4(rel,0,0) );
    
    // The texture coordinates: select one of the 256 characters
    // Multiply by 8: The x-position reflects the bit in the byte from the
    // Character generator.
    // The y-position reflects the row in the character generator.
    char_gs = character_vs[0];
    texcoord = vec2(x,y) * 8.0f;
    
    // Just push the foreground color through to the fragment shader.
    fg_col_gs = fg_col_vs[0];
    
    EmitVertex();   // emit the outputs.
}

// For each screen coordinate given from the vertex shader,
// generate 4 vertices, one for each corner of the character to display.
void main()
{
    emit_vertex( 0, 1 );    // bottom-left
    emit_vertex( 1, 1 );    // bottom-right
    emit_vertex( 0, 0 );    // top-left
    emit_vertex( 1, 0 );    // top-right

    EndPrimitive();
};

)"

;

static const char *fts =
// #include "text_screen_shaders.fs"

R"(
#version 460 core

uniform isampler2D TEX;        // character generator ROM.
uniform int background_color;  // global screen background color.
uniform ivec3 palette[16];      // The 16 colors.
uniform int charset;           // Selects which character set to use (0 or 1)

in vec2 texcoord;           // The interpolated texture coordinate.
in flat int fg_col_gs;      // The foreground color.
in flat int char_gs;        // The chaaracter to display.

out vec4 FragColor;         // The pixel output color.

void main()
{

   int bit  = int(texcoord.x) & 0x7;
   int col  = int(texcoord.y) & 0x7;
   int row  = char_gs + 256*charset;

   ivec2 tx = ivec2( col, row );
   int byte = texelFetch( TEX, tx, 0 ).r;

   float f  = float(((byte>>(7-bit))&1)) * 1.0f;

   vec4 fg_col = vec4( float(palette[fg_col_gs       ].r)/255, float(palette[fg_col_gs       ].g)/255, float(palette[fg_col_gs       ].b)/255, 1);
   vec4 bg_col = vec4( float(palette[background_color].r)/255, float(palette[background_color].g)/255, float(palette[background_color].b)/255, 1);

   FragColor = mix( bg_col, fg_col, f);

};

)"

;

//========================================================================
// Setup the text screen objects;
void text_screen::init( utils::Buffer &CG, int cols, int rows, const glm::vec2 &pos )
{
    m_Rows = rows;
    m_Cols = cols;
    int max_chars = rows*cols;

    //------------------------------------------------------------------
    // Set up the "texture" to hold the screen RAM.
    screen.gen().activate(0).bind(GL_TEXTURE_2D).size(max_chars,1)
        .iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE).TexImage2D()
        .Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP)
        .Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        .unbind();

    //------------------------------------------------------------------
    // Set up the "texture" to hold the color RAM.
    colram.gen().activate(1).bind(GL_TEXTURE_2D).size(max_chars,1)
        .iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE).TexImage2D()
        .Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP)
        .Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        .unbind();

    //------------------------------------------------------------------
    // Set up the texture to hold the character generator ROM.
    //auto CG { utils::RM.load("roms/chargen") };
    chrgen.gen().activate(2).bind(GL_TEXTURE_2D).size(8,512)
        .iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE).TexImage2D( nullptr /*CG.data()*/ )
        .Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP)
        .Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        .unbind();

    //------------------------------------------------------------------
    // Compile and link the shader program.
    program.compile( GL_VERTEX_SHADER,   vxs );
    program.compile( GL_GEOMETRY_SHADER, gms );
    program.compile( GL_FRAGMENT_SHADER, fts );
    program.link();

    //------------------------------------------------------------------
    // Get the locations of the shader inputs and uniforms.
    loc_coord =    glGetAttribLocation( program, "screen_coord" );

    loc_MVP =      glGetUniformLocation( program, "MVP"  );
    loc_TEX =      glGetUniformLocation( program, "TEX"  );
    loc_CHARS =    glGetUniformLocation( program, "CHARS"  );
    loc_COLOR =    glGetUniformLocation( program, "COLOR"  );
    loc_palette =  glGetUniformLocation( program, "palette"  );
    loc_bg_color = glGetUniformLocation( program, "background_color"  );
    loc_Offset =   glGetUniformLocation( program, "TextOffset");
    loc_scaling =  glGetUniformLocation( program, "scaling"  );
    loc_charset =  glGetUniformLocation( program, "charset");

    //------------------------------------------------------------------
    // Set some defaults of the shader uniforms.
    glUseProgram( program );
    glUniform2f( loc_Offset, pos[0], pos[1] );
    glUniform1f( loc_scaling,  8);   // 8 = "real life pixel size" .
    glUniform1i( loc_charset,  0);   // Which of the two character sets to show.
    glUniform1i( loc_bg_color, 0);   // The background color to use for all characters.
    // Set the palette / color table.
    glUniform3iv( loc_palette, sizeof(color_table), &color_table.data()[0][0] );
    // Assign the textures for screen RAM, color RAM and character generator ROM.
    screen.set_texture_unit( loc_CHARS );
    colram.set_texture_unit( loc_COLOR );
    chrgen.set_texture_unit( loc_TEX   );

    //------------------------------------------------------------------
    // Create a vertex attribute array and bind it.
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    //------------------------------------------------------------------
    // Enable shader input "vPos" and describe its layout in the vertex buffer.
    glEnableVertexAttribArray(loc_coord);
    glVertexAttribPointer( loc_coord, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0 );

    //------------------------------------------------------------------
    // Create a buffer holding the screen coordinates (0-39,0-24) of each character.
    glm::vec3 coords[max_chars]; 
    for( int i=0; i<max_chars; i++ )
    {
        coords[i][0]   = float(i % m_Cols); // x-position on the screen.
        coords[i][1]   = float(i / m_Cols); // y-position on the screen.
        coords[i][2]   = float(i);          // Index into the screen RAM.
    }
    //------------------------------------------------------------------
    // Upload the vertices to the vertex buffer.
    glBufferData( GL_ARRAY_BUFFER, sizeof(coords), &coords[0], GL_DYNAMIC_DRAW );

    //------------------------------------------------------------------
    // Unbind the vertex attribute array and the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //------------------------------------------------------------------
}

//======================================================================
void text_screen::render()
{
    //------------------------------------------------------------------
    // Activate the texture units and bind the texture buffers
    // as defined at initialization.
    screen.activate().bind();
    colram.activate().bind();
    chrgen.activate().bind();
    //------------------------------------------------------------------
    // Draw the vertices of the texture screen.
    glUseProgram( program );
    glBindVertexArray(vertex_array_id);
    glDrawArrays( GL_POINTS, 0, m_Rows * m_Cols);
    //------------------------------------------------------------------
}

//======================================================================
void text_screen::set_memories( void *new_chars, void *new_colrs, void *chargen )
{
    if( new_chars )
        screen.bind().TexImage2D( new_chars );
    if( new_colrs )
        colram.bind().TexImage2D( new_colrs );
    if( chargen )
        chrgen.bind().TexImage2D( chargen );
}

//======================================================================
// Set the background color
void text_screen::set_bg_color( int bg_color )
{
    glUseProgram( program );
    glUniform1i( loc_bg_color, bg_color );
}

//======================================================================
void text_screen::resize_screen(int width, int height)
{
    //------------------------------------------------------------------
    auto MVP { glm::ortho<float>( 0, width, height, 0, 1, -1 ) };
    //------------------------------------------------------------------
    glUseProgram( program );
    glUniformMatrix4fv( loc_MVP, 1, false, &MVP[0][0]);
    //------------------------------------------------------------------
}

//======================================================================
} // End of namespace gfx
//======================================================================
