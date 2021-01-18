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
    "uniform isampler1D CHARS;\n"
    "uniform isampler1D COLOR;\n"
    "uniform mat4 MVP;\n" // Model-View-Projection Matrix ("Camera")
    "uniform vec2 TextOffset;\n" // Offset on the screen, added to all coordinates.
    "uniform vec4 background_color;\n"  // global screen background color
    "uniform vec3 pallette[16];\n\n" // The 16 colors

    "in vec2 screen_coord;\n\n" // Input: The index of the character to display.

    "out int character_vs;\n"
    "out flat int fg_col_vs;\n\n"

    "void main()\n"       // Shader: Calculate screen coordinates of the
    "{\n"                 // vertex from the 3D position.
//        "float x = float(index&0x0f);"
//    "   int x = int(mod(index,40));"
//    "   int y = int(index/40);"
//    "   vec2 screen_coord = vec2(x, y);\n" // convert index to screen coordinates.
    //"   gl_Position = vec4(screen_coord /*+ TextOffset*/, 0, 1);\n"
    "    gl_Position = vec4( TextOffset + screen_coord*8, 0, 1); \n"
    "    int index = int(screen_coord.x + screen_coord.y*40);\n"
    "   character_vs = int(texture(CHARS, index).r) & 0xFF;\n"
    "   fg_col_vs = texture(COLOR, index).r & 0x0F;\n"
    "}\n";

//------------------------------------------------------------------
static const char *gms =
"#version 460 core\n"
"uniform mat4 MVP;" // Model-View-Projection Matrix ("Camera")
"layout ( points ) in;"// Each vertex we get in is a point
"layout ( triangle_strip, max_vertices = 4 ) out;"// From the input we will produce a triangle strip of 4 vertices as output.
"in int character_vs[];"
"in int fg_col_vs[];"
"out vec2 texcoord;"
"out flat int fg_col_gs;"
"void emit_vertex(float x, float y)"
"{"
"   vec2 rel = vec2(x,y) * 8.0f;" // The relative position of the vertex in pixels.
"   gl_Position = MVP * ( gl_in[0].gl_Position + vec4(rel,0,0) );"
"   texcoord = vec2 ( (character_vs[0] + x)*8, y*8);"
"   fg_col_gs = fg_col_vs[0];"
"   EmitVertex();"
"}"
"void main() {"
"   emit_vertex(  0, 0 );"
"   emit_vertex(  1, 0 );"
"   emit_vertex(  0, 1 );"
"   emit_vertex(  1, 1 );"
"	EndPrimitive();"
"}"
;

//------------------------------------------------------------------
// The fragment shader
static const char *fts =
"#version 460 core\n"
"uniform isampler2D TEX;" // character generator ROM.
"uniform int background_color;"  // global screen background color
"uniform vec3 pallette[16];" // The 16 colors
"in vec2 texcoord;"
"in flat int fg_col_gs;"
"out vec4 FragColor;"
"void main()"
"{"
"   int bit  = int(texcoord.x) % 32;"
"   ivec2  tx = ivec2( int(texcoord.x/8), int(texcoord.y) );"
"   int   r  = texture( TEX, tx ).r;"
"   float f  = (( r & (1<<bit) )!=0) ? 1.0f : 0.0f;"
"   vec3 fg_col = pallette[fg_col_gs];"
"   FragColor = mix( vec4( pallette[background_color],1), vec4(fg_col, 1.0f), f);"
"}"
;

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

    auto chargen { utils::RM.load("roms/chargen") };

    chargen_tex.load( chargen.data(), 512, 8, GL_R8, GL_RED );
    screen.load(    (char*)&chars[0], 1000, 1, GL_R8, GL_RED );
    colram.load(   (char*)&chars[0], 1000, 1, GL_R8, GL_RED );

# if 1
    std::cout << "Compiling vertex shader ------------------------------------\n";
    auto vxs_id = compile_shader( GL_VERTEX_SHADER, vxs );
    std::cout << "Compiling geometry shader ------------------------------------\n";
    auto gms_id = compile_shader( GL_GEOMETRY_SHADER, gms );
    std::cout << "Compiling fragment shader ------------------------------------\n";
    auto fts_id = compile_shader( GL_FRAGMENT_SHADER, fts );
    std::cout << "Linking program  --------------------------------------------\n";
    program_id = link_program( vxs_id, gms_id, fts_id);
    std::cout << program_id << " " << vxs_id << " " << gms_id << " " << fts_id << "\n";
#endif

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
    glUseProgram( program_id );
    glUniform2f( loc_Offset, 0, 0 );
    glUniform1i(loc_TEX, 0);
    chargen_tex.bind( GL_TEXTURE0 );
    glUniform1i(loc_CHARS, 1 );
    screen.bind( GL_TEXTURE1 );
    glUniform1i(loc_COLOR, 2 );
    colram.bind( GL_TEXTURE2 );
    glBindVertexArray(vertex_array_id);
    glDrawArrays( GL_POINTS, 0, 1000);
}


} // End of namespace gfx
