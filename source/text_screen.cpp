#include "text_screen.h"
#include "texture.h"
#include "utils.h"

#include "graphics.h"

#include "mainwindow.h"

#include <glad/glad.h>
#include <iostream>

namespace gfx {

//========================================================================
std::array<vec3, 16> color_table { {
        // Taken from the screenshot of the C64-wiki.com
        // https://www.c64-wiki.com/wiki/color
        // Note: The values in the table on the same site are different!
        { 0x00 / 255.0F, 0x00 / 255.0F, 0x00 / 255.0F },
        { 0xFF / 255.0F, 0xFF / 255.0F, 0xFF / 255.0F },
        {  146 / 255.0F,   74 / 255.0F,   64 / 255.0F },
        {  132 / 255.0F,  197 / 255.0F,  204 / 255.0F },
        {  147 / 255.0F,   81 / 255.0F,  182 / 255.0F },
        {  114 / 255.0F,  177 / 255.0F,   75 / 255.0F },
        {   72 / 255.0F,   58 / 255.0F,  170 / 255.0F },
        {  213 / 255.0F,  223 / 255.0F,  124 / 255.0F },
        {  103 / 255.0F,   82 / 255.0F,    0 / 255.0F },
        {   87 / 255.0F,   66 / 255.0F,    0 / 255.0F },
        {  193 / 255.0F,  129 / 255.0F,  120 / 255.0F },
        {   96 / 255.0F,   96 / 255.0F,   96 / 255.0F },
        {  138 / 255.0F,  138 / 255.0F,  138 / 255.0F },
        {  179 / 255.0F,  236 / 255.0F,  145 / 255.0F },
        {  134 / 255.0F,  122 / 255.0F,  222 / 255.0F },
        {  179 / 255.0F,  179 / 255.0F,  179 / 255.0F }
} };


//========================================================================
// A vertex shader for the text screen.
static const char *vxs =
    "#version 460 core\n"

    "uniform isampler2D CHARS;\n" // Screen characters: 1000 bytes
    "uniform isampler2D COLOR;\n" // Screen color ram: 1000 nibbles

    "uniform mat4 MVP;\n" // Model-View-Projection Matrix ("Camera")
    "uniform vec2 TextOffset;\n" // Offset on the screen, added to all coordinates.
    "uniform float scaling;\n"

    "in vec2 screen_coord;\n\n" // Input: The coordinates (0-39,0-24) of the character to display.

    "out int character_vs;\n"       // output: the character to display (0-255)
    "out flat int fg_col_vs;\n\n"   // output: the foreground color to display (0-15)

    "void main()\n"       // Shader: Calculate screen coordinates of the
    "{\n"                 // vertex from the 3D position.
    "    gl_Position  = vec4( TextOffset + screen_coord*scaling, 0, 1); \n"
    "    int index    = int(screen_coord.x) + int(screen_coord.y) * 40;\n"
    "    ivec2 coord  = ivec2(index,0);\n"
    "    character_vs = (texelFetch(CHARS, coord, 0 ).r) & 0xFF;\n"
    "    fg_col_vs    = (texelFetch(COLOR, coord, 0 ).r) & 0x0F;\n"
    "}\n";

//========================================================================
// Geometry shader for the text screen.
static const char *gms =
    "#version 460 core\n"

    "uniform mat4 MVP;\n" // Model-View-Projection Matrix ("Camera")
    "uniform float scaling;\n"

    // Input: Each vertex is a "point"
    "layout ( points ) in;\n"
    // Output: a triangle strip of 4 vertices as output.
    "layout ( triangle_strip, max_vertices = 4 ) out;\n"

    "in int character_vs[];\n" // Input: the character to display.
    "in int fg_col_vs[];\n"    // Input: the foreground color to use

    "out vec2 texcoord;\n"        // Output: The texture coordinate.
    "out flat int fg_col_gs;\n"   // Output: The foreground color to use
    "out flat int char_gs;\n"     // Output: The character to display.

    // Emit a single vertex.
    // x and y designate the corner of the character. Must be 0 or 1.
    "void emit_vertex(float x, float y)\n"
    "{\n"
        // The relative position of the vertex in pixels.
    "   vec2 rel = vec2(x,y) * scaling;\n"
        // The absolute output position: Input + relative position
    "   gl_Position = MVP * ( gl_in[0].gl_Position + vec4(rel,0,0) );\n"
        // The texture coordinates: select one of the 256 characters
        // Multiply by 8: The x-position reflects the bit in the byte from the
        // Character generator.
        // The y-position reflects the "row" in the character generator.
    "   char_gs = character_vs[0];"
    "   texcoord = vec2(x,y) * 8.0f;\n"
        // Just push the foreground color through to the fragment shader.
    "   fg_col_gs = fg_col_vs[0];\n"
        // emit the outputs.
    "   EmitVertex();\n"
    "}\n"

    // For each screen coordinate given from the vertex shader,
    // generate 4 vertices, one for each corner of the character to display.
    "void main() {\n"
    "   emit_vertex(  0, 1 );\n"    // bottom-left
    "   emit_vertex(  1, 1 );\n"    // bottom-right
    "   emit_vertex(  0, 0 );\n"    // top-left
    "   emit_vertex(  1, 0 );\n"    // top-right
    "	EndPrimitive();\n"
    "}\n";

//========================================================================
// The fragment shader for the text screen
static const char *fts =
    "#version 460 core\n"

    "uniform isampler2D TEX;\n"        // character generator ROM.
    "uniform int background_color;\n"  // global screen background color.
    "uniform vec3 pallette[16];\n"     // The 16 colors.
    "uniform int charset;\n"           // Selects which character set to use (0 or 1)

    "in vec2 texcoord;\n"           // The interpolated texture coordinate.
    "in flat int fg_col_gs;\n"      // The foreground color.
    "in flat int char_gs;\n"        // The chaaracter to display.

    "out vec4 FragColor;\n"         // The pixel output color.

    "void main()\n"
    "{\n"

    "   int bit  = int(texcoord.x) & 0x7;\n"
    "   int col  = int(texcoord.y) & 0x7;\n"
    "   int row  = char_gs + 256*charset;\n"

    "   ivec2 tx = ivec2( col, row );\n"
    "   int byte = texelFetch( TEX, tx, 0 ).r;\n"

    "   float f  = float(((byte>>(7-bit))&1)) * 1.0f;\n"

    "   vec4 fg_col = vec4( pallette[fg_col_gs], 1);\n"
    "   vec4 bg_col = vec4( pallette[background_color], 1);\n"

    "   FragColor = mix( bg_col, fg_col, f);\n"

    "}\n";

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
    screen.gen().unit(0).bind(GL_TEXTURE_2D).size(1000,1);
    screen.iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE);
    screen.Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP);
    screen.Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    screen.Image2D((char*)&chars[0]);
    screen.unbind();

    //------------------------------------------------------------------
    // Set up the "texture" to hold the color RAM.
    colram.gen().unit(1).bind(GL_TEXTURE_2D).size(1000,1);
    colram.iformat(GL_R8UI).format(GL_RED_INTEGER).type(GL_UNSIGNED_BYTE);
    colram.Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP);
    colram.Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    colram.Image2D((char*)&colrs[0]);
    colram.unbind();

    //------------------------------------------------------------------
    // Set up the texture to hold the character generator ROM.
    auto CG { utils::RM.load("roms/chargen") };
    chrgen.gen().unit(2).bind(GL_TEXTURE_2D).size(8,512);
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
    program_id = link_program3( vxs_id, gms_id, fts_id);

    //------------------------------------------------------------------
    // Get the locations of the shader inputs and uniforms.
    loc_coord =    glGetAttribLocation( program_id, "screen_coord" );

    loc_MVP =      glGetUniformLocation( program_id, "MVP"  );
    loc_TEX =      glGetUniformLocation( program_id, "TEX"  );
    loc_CHARS =    glGetUniformLocation( program_id, "CHARS"  );
    loc_COLOR =    glGetUniformLocation( program_id, "COLOR"  );
    loc_pallette = glGetUniformLocation( program_id, "pallette"  );
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
    glUniform3fv( loc_pallette, sizeof(vec3[16]), &color_table.data()[0][0] );

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
    screen.bind( );
    colram.bind( );
    chrgen.bind();

    glUseProgram( program_id );
    glBindVertexArray(vertex_array_id);
    glDrawArrays( GL_POINTS, 0, 1000);
}


//======================================================================
} // End of namespace gfx
//======================================================================
