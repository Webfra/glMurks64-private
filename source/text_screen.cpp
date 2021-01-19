#include "text_screen.h"
#include "texture.h"
#include "utils.h"

#include "graphics.h"

#include <glad/glad.h>
#include <iostream>

namespace gfx {

//------------------------------------------------------------------
// A simple vertex shader for textured vertices.
static const char *vxs =
    "#version 460 core\n"

    "uniform isampler2D CHARS;\n" // Screen characters: 1000 bytes
    "uniform isampler2D COLOR;\n" // Screen color ram: 1000 nibbles

    "uniform mat4 MVP;\n" // Model-View-Projection Matrix ("Camera")
    "uniform vec2 TextOffset;\n" // Offset on the screen, added to all coordinates.

    "in vec2 screen_coord;\n\n" // Input: The coordinates (0-39,0-24) of the character to display.

    "out int character_vs;\n"       // output: the character to display (0-255)
    "out flat int fg_col_vs;\n\n"   // output: the foreground color to display (0-15)

    "void main()\n"       // Shader: Calculate screen coordinates of the
    "{\n"                 // vertex from the 3D position.
    "    gl_Position = vec4( TextOffset + screen_coord*32, 0, 1); \n"
    "    int index = int(screen_coord.x) + int(screen_coord.y) * 40;\n"
    "    ivec2 coord = ivec2(index,0);\n"
    "    character_vs = (texelFetch(CHARS, coord, 0 ).r) & 0xFF;\n"
    "    fg_col_vs = (texelFetch(COLOR, coord, 0 ).r) & 0x0F;\n"
    "}\n";

//------------------------------------------------------------------
static const char *gms =
"#version 460 core\n"

"uniform mat4 MVP;\n" // Model-View-Projection Matrix ("Camera")

// Input: Each vertex we get in is a point
"layout ( points ) in;\n"
// Output: a triangle strip of 4 vertices as output.
"layout ( triangle_strip, max_vertices = 4 ) out;\n"

"in int character_vs[];\n" // Input: the character to display.
"in int fg_col_vs[];\n"    // Input: the foreground color to use

"out vec2 texcoord;\n"        // Texture coordinate to use in fragment shader
"out flat int fg_col_gs;\n"   // Output: the foreground color to use
"out flat int char_gs;\n"

"void emit_vertex(float x, float y)\n"
"{\n"
    // The relative position of the vertex in pixels.
    // Scaling factor must match the value in the vertex shader!
"   vec2 rel = vec2(x,y) * 32.0f;\n"
    // The absolute output position: Input + relative position
"   gl_Position = MVP * ( gl_in[0].gl_Position + vec4(rel,0,0) );\n"
    // The texture coordinates: select one of the 256 characters
    // Multiply by 8: The x-position reflects the bit in the byte from the
    // Character generator.
    // The y-position reflects the "row" in the character generator.
"   char_gs = character_vs[0];"
"   texcoord = vec2 (x,y)*8;\n"
    // Just push the foreground color through to the fragment shader.
"   fg_col_gs = fg_col_vs[0];\n"
    // emit the outputs.
"   EmitVertex();\n"
"}\n"

// For each screen coordinate given from the vertex shader,
// generate 4 vertices, one for each corner of the character to display.
"void main() {\n"
"   emit_vertex(  0, 0 );\n"    // top-left
"   emit_vertex(  1, 0 );\n"    // top-right
"   emit_vertex(  0, 1 );\n"    // bottom-left
"   emit_vertex(  1, 1 );\n"    // bottom-right
"	EndPrimitive();\n"
"}\n";

//------------------------------------------------------------------
// The fragment shader
static const char *fts =
"#version 460 core\n"

"uniform isampler2D TEX;\n" // character generator ROM.
"uniform int background_color;\n"  // global screen background color
"uniform vec3 pallette[16];\n" // The 16 colors

"in vec2 texcoord;\n"
"in flat int fg_col_gs;\n"
"in flat int char_gs;\n"

"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"   int bit  = int(texcoord.x) & 0x7;\n"
"   int col  = int(texcoord.y) & 0x7;\n"
"   int row  = char_gs;\n"
"   ivec2 tx = ivec2( col, row );\n"
"   int byte = texelFetch( TEX, tx, 0 ).r;\n"
"   float f  = float(((byte>>(7-bit))&1)) * 1.0f;\n"
"   vec4 fg_col = vec4( pallette[fg_col_gs], 1);\n"
"   vec4 bg_col = vec4( pallette[background_color], 1);\n"
"   FragColor = mix( bg_col, fg_col, f);\n"
"}\n";

GLuint link_program( GLuint vxs_id, GLuint gms_id, GLuint fts_id )
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

void text_screen::init()
{
    uint8_t chars[1000];
    vec2 screen_coords[1000];
    for(int i=0;i<1000;i++)
    {
        chars[i]=i;
        screen_coords[i][0]   = float(i % 40);
        screen_coords[i][1]   = float(i / 40);
    }

    screen.gen().unit(0).bind(GL_TEXTURE_2D).size(1000,1);
    screen.iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE);
    screen.Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP);
    screen.Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    screen.Image2D((char*)&chars[0]);
    screen.unbind();

    colram.gen().unit(1).bind(GL_TEXTURE_2D).size(1000,1);
    colram.iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE);
    colram.Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP);
    colram.Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    colram.Image2D((char*)&chars[0]);
    colram.unbind();

    auto CG { utils::RM.load("roms/chargen") };
    chrgen.gen().unit(2).bind(GL_TEXTURE_2D).size(8,512);
    chrgen.iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE);
    chrgen.Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP);
    chrgen.Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    chrgen.Image2D( CG.data() );
    chrgen.unbind();
    for ( int row =0; row<8; row++)
    {
        int rowsize = 16;
        for( int col=0; col<rowsize; col++)
        {
            int pos = row*rowsize + col;
            std::cout << std::hex << (int)CG[pos] << " ";
        }
        std::cout << std::dec << std::endl;
    }


    std::cout << "Compiling vertex shader ------------------------------------\n";
    auto vxs_id = compile_shader( GL_VERTEX_SHADER, vxs );
    std::cout << "Compiling geometry shader ------------------------------------\n";
    auto gms_id = compile_shader( GL_GEOMETRY_SHADER, gms );
    std::cout << "Compiling fragment shader ------------------------------------\n";
    auto fts_id = compile_shader( GL_FRAGMENT_SHADER, fts );
    std::cout << "Linking program  --------------------------------------------\n";
    program_id = link_program( vxs_id, gms_id, fts_id);
    std::cout << program_id << " " << vxs_id << " " << gms_id << " " << fts_id << "\n";

    GLint loc_index = glGetAttribLocation( program_id, "screen_coord" );

    loc_MVP = glGetUniformLocation( program_id, "MVP"  );
    loc_TEX = glGetUniformLocation( program_id, "TEX"  );
    loc_CHARS = glGetUniformLocation( program_id, "CHARS"  );
    loc_COLOR = glGetUniformLocation( program_id, "COLOR"  );
    loc_pallette = glGetUniformLocation( program_id, "pallette"  );
    loc_bg_color = glGetUniformLocation( program_id, "background_color"  );
    loc_Offset = glGetUniformLocation( program_id, "TextOffset");

    std::cout << "mvp" << loc_MVP << " TEX:" << loc_TEX << " CHARS:" ;
    std::cout << loc_CHARS << " COLOR:" << loc_COLOR;
    std::cout << " pall:" << loc_pallette
            << " bg:" << loc_bg_color << " index:" << loc_index
            << " offs:" << loc_Offset << std::endl;


    //------------------------------------------------------------------
    glUseProgram( program_id );
    glUniform2f( loc_Offset, 0, 0 );
    screen.gl_Uniform( loc_CHARS );
    colram.gl_Uniform( loc_COLOR );
    chrgen.gl_Uniform( loc_TEX );

    // Set the palette (just gray scale for now, 'cause its easy...)
    vec3 palette[16];
    for( int i=0; i<16; i++)
        palette[i][0] = palette[i][1] = palette[i][2] = 1.0f * i / 16.0f;
    glUniform3fv( loc_pallette, sizeof(palette), &palette[0][0] );

    //------------------------------------------------------------------
    // Create a vertex attribute array and bind it.
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    GLuint vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    //------------------------------------------------------------------
    // Enable shader input "vPos" and describe its layout in the vertex buffer.
    glEnableVertexAttribArray(loc_index);
    glVertexAttribPointer( loc_index, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0 );
    //------------------------------------------------------------------
    // Upload the vertices to the vertex buffer.
    std::cout << "BufferData " << std::size(screen_coords) << std::endl;
    glBufferData( GL_ARRAY_BUFFER, sizeof(screen_coords), &screen_coords[0], GL_DYNAMIC_DRAW );
    std::cout << "BufferData End" << std::endl;
    //------------------------------------------------------------------
    // Unbind the vertex attribute array and the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void text_screen::render()
{
    screen.bind( );
    colram.bind( );
    chrgen.bind();

    glUseProgram( program_id );
    glBindVertexArray(vertex_array_id);
    glDrawArrays( GL_POINTS, 0, 1000);
}


} // End of namespace gfx
