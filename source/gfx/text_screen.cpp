
#include "text_screen.h"
#include "texture.h"
#include "definitions.h"
#include "utils.h"

#include "linmath.h"
#include <glad/glad.h>
#include <iostream>

namespace gfx {

//========================================================================
// A vertex shader for the text screen.
static const char *vxs =
#include "text_screen_shaders.vs"
;

static const char *gms =
#include "text_screen_shaders.gs"
;

static const char *fts =
#include "text_screen_shaders.fs"
;

//========================================================================
// Setup the text screen objects;
void text_screen::init( utils::Buffer &CG )
{
    //------------------------------------------------------------------
    // Clear the buffers.
    for(int i=0;i<1000;i++)
    {
        chars[i]=32; // Space character
        colrs[i]=14; // light blue color
        coords[i][0]   = float(i % 40);
        coords[i][1]   = float(i / 40);
    }
    //------------------------------------------------------------------
    // Fill the screen with something visible
    for(int x=0;x<16;x++)
        for(int y=0;y<16;y++)
            chars[x+y*40] = x+y*16;

    //------------------------------------------------------------------
    // Set up the "texture" to hold the screen RAM.
    screen.gen().activate(0).bind(GL_TEXTURE_2D).size(1000,1)
        .iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE)
        .Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP)
        .Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        .Image2D((char*)&chars[0])
        .unbind();

    //------------------------------------------------------------------
    // Set up the "texture" to hold the color RAM.
    colram.gen().activate(1).bind(GL_TEXTURE_2D).size(1000,1)
        .iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE)
        .Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP)
        .Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        .Image2D((char*)&colrs[0])
        .unbind();

    //------------------------------------------------------------------
    // Set up the texture to hold the character generator ROM.
    //auto CG { utils::RM.load("roms/chargen") };
    chrgen.gen().activate(2).bind(GL_TEXTURE_2D).size(8,512)
        .iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE)
        .Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP)
        .Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        .Image2D( CG.data() )
        .unbind();

    //------------------------------------------------------------------
    // Compile and link the shader program.
    auto vxs_id = compile_shader( GL_VERTEX_SHADER, vxs );
    auto gms_id = compile_shader( GL_GEOMETRY_SHADER, gms );
    auto fts_id = compile_shader( GL_FRAGMENT_SHADER, fts );
    program_id = link_program( vxs_id, fts_id, gms_id);

    //------------------------------------------------------------------
    // Get the locations of the shader inputs and uniforms.
    loc_coord =    glGetAttribLocation( program_id, "screen_coord" );

    loc_MVP =      glGetUniformLocation( program_id, "MVP"  );
    loc_TEX =      glGetUniformLocation( program_id, "TEX"  );
    loc_CHARS =    glGetUniformLocation( program_id, "CHARS"  );
    loc_COLOR =    glGetUniformLocation( program_id, "COLOR"  );
    loc_palette =  glGetUniformLocation( program_id, "palette"  );
    loc_bg_color = glGetUniformLocation( program_id, "background_color"  );
    loc_Offset =   glGetUniformLocation( program_id, "TextOffset");
    loc_scaling =  glGetUniformLocation( program_id, "scaling"  );
    loc_charset =  glGetUniformLocation( program_id, "charset");

    //------------------------------------------------------------------
    // Set some defaults of the shader uniforms
    glUseProgram( program_id );

    glUniform2f( loc_Offset, 64/2, 72/2 );
    glUniform1f( loc_scaling, 8); // 8 = "real life pixel size" 
    glUniform1i( loc_charset, 0);
    glUniform1i( loc_bg_color, 6);

    screen.gl_Uniform( loc_CHARS );
    colram.gl_Uniform( loc_COLOR );
    chrgen.gl_Uniform( loc_TEX );

    //------------------------------------------------------------------
    // Set the palette
    glUniform3iv( loc_palette, sizeof(ivec3[16]), &color_table.data()[0][0] );
    //------------------------------------------------------------------
    // Create a vertex attribute array and bind it.
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    GLuint vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    //------------------------------------------------------------------
    // Enable shader input "vPos" and describe its layout in the vertex buffer.
    glEnableVertexAttribArray(loc_coord);
    glVertexAttribPointer( loc_coord, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0 );
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
    // TEMPORARY TEST: CHANGE SCREEN CHARACTERS
    #if 0
    chars[0]++;
    colrs[1]++;
    update_memories( chars, colrs );
    #endif
    //------------------------------------------------------------------
    // Activate the texture units and bind the texture buffers
    // as defined at initialization.
    screen.activate().bind();
    colram.activate().bind();
    chrgen.activate().bind();
    //------------------------------------------------------------------
    // Draw the vertices of the texture screen.
    glUseProgram( program_id );
    glBindVertexArray(vertex_array_id);
    glDrawArrays( GL_POINTS, 0, 1000);
    //------------------------------------------------------------------
}

//======================================================================
void text_screen::update_memories( uint8_t new_chars[1000], uint8_t new_colrs[1000] )
{
    screen.bind().Image2D( &new_chars[0] );
    colram.bind().Image2D( &new_colrs[0] );
    glUseProgram( program_id );
    glUniform1i( loc_bg_color, colrs[1] & 0x0f );
}

//======================================================================
void text_screen::resize_screen(int width, int height)
{
    //------------------------------------------------------------------
    mat4x4 MVP;
    mat4x4_ortho( MVP, 0, width, height, 0, 1, -1 );
    //------------------------------------------------------------------
    glUseProgram( program_id );
    glUniformMatrix4fv( loc_MVP, 1, false, &MVP[0][0]);
    //------------------------------------------------------------------
}

//======================================================================
} // End of namespace gfx
//======================================================================
