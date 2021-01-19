#include "text_screen.h"
#include "texture.h"
#include "utils.h"

#include "graphics.h"
#include "linmath.h"

#include "mainwindow.h"

#include <glad/glad.h>
#include <iostream>

namespace gfx {

//========================================================================

std::array<ivec3, 16> color_table { {
    // Taken from the screenshot of the C64-wiki.com
    // https://www.c64-wiki.com/wiki/color
    // Note: The values in the table on the same site are different!
    {    0,    0,    0 }, //  0  Black
    {  255,  255,  255 }, //  1  White
    {  146,   74,   64 }, //  2  Red
    {  132,  197,  204 }, //  3  Cyan
    {  147,   81,  182 }, //  4  Violet
    {  114,  177,   75 }, //  5  Green
    {   72,   58,  170 }, //  6  Blue
    {  213,  223,  124 }, //  7  Yellow
    {  103,   82,    0 }, //  8  Orange
    {   87,   66,    0 }, //  9  Brown
    {  193,  129,  120 }, // 10  Light Red
    {   96,   96,   96 }, // 11  Dark Grey
    {  138,  138,  138 }, // 12  Grey
    {  179,  236,  145 }, // 13  Light Green
    {  134,  122,  222 }, // 14  Light Blue
    {  179,  179,  179 }  // 15  Light Grey
} };


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

#if 0
//========================================================================
// Link 3 shader into a shader program.
GLuint link_program3( GLuint vxs_id, GLuint gms_id, GLuint fts_id )
{
    //------------------------------------------------------------------
    // A buffer for holding messages from linking the program.
    GLchar buffer[2048];
    GLsizei length;
    //------------------------------------------------------------------
    // Create a new shader program object to hold the shaders.
    GLuint program_id = glCreateProgram();
    // Attach the vertex and fragment shaders.
    glAttachShader( program_id, vxs_id );
    glAttachShader( program_id, gms_id );
    glAttachShader( program_id, fts_id );
    // Link the shader program.
    glLinkProgram( program_id );
    // See if any errors occurred during linking.
    glGetProgramInfoLog( program_id, 2047, &length, buffer);
    if (length > 0)
    {
        fprintf(stderr, "Linking Program log: %s", buffer);
        exit(-1);
    }
    return program_id;
}
#endif

//========================================================================
// Setup the text screen objects;
void text_screen::init()
{
    //------------------------------------------------------------------
    // Clear the buffers.
    for(int i=0;i<1000;i++)
    {
        chars[i]=32; // Space character
        colrs[i]=14; // light blue color
        screen_coords[i][0]   = float(i % 40);
        screen_coords[i][1]   = float(i / 40);
    }
    //------------------------------------------------------------------
    // Fill the screen with something visible
    for(int x=0;x<16;x++)
    {
        for(int y=0;y<16;y++)
        {
            chars[x+y*40] = x+y*16;
        }
    }

    //------------------------------------------------------------------
    // Set upt the "texture" to hold the screen RAM.
    screen.gen().activate(0).bind(GL_TEXTURE_2D).size(1000,1);
    screen.iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE);
    screen.Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP);
    screen.Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    screen.Image2D((char*)&chars[0]);
    screen.unbind();

    //------------------------------------------------------------------
    // Set up the "texture" to hold the color RAM.
    colram.gen().activate(1).bind(GL_TEXTURE_2D).size(1000,1);
    colram.iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE);
    colram.Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP);
    colram.Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    colram.Image2D((char*)&colrs[0]);
    colram.unbind();

    //------------------------------------------------------------------
    // Set up the texture to hold the character generator ROM.
    auto CG { utils::RM.load("roms/chargen") };
    chrgen.gen().activate(2).bind(GL_TEXTURE_2D).size(8,512);
    chrgen.iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE);
    chrgen.Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP);
    chrgen.Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    chrgen.Image2D( CG.data() );
    chrgen.unbind();

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

    glUniform2f( loc_Offset, BORDER, BORDER );
    glUniform1f( loc_scaling, SCALING); // keep it power of 2, or it may look ugly.
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
    glBufferData( GL_ARRAY_BUFFER, sizeof(screen_coords), &screen_coords[0], GL_DYNAMIC_DRAW );
    //------------------------------------------------------------------
    // Unbind the vertex attribute array and the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

//======================================================================
void text_screen::render()
{
    //------------------------------------------------------------------
    // Define the background color
    glClearColor(   color_table[14][0]/255.0f, 
                    color_table[14][1]/255.0f, 
                    color_table[14][2]/255.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    screen.activate().bind();
    colram.activate().bind();
    chrgen.activate().bind();

    glUseProgram( program_id );
    glBindVertexArray(vertex_array_id);
    glDrawArrays( GL_POINTS, 0, 1000);
}


//======================================================================
} // End of namespace gfx
//======================================================================
