#include "text_screen.h"
#include "graphics.h"
#include "utils.h"

#include <iostream>

namespace gfx {

//------------------------------------------------------------------
// A simple vertex shader for textured vertices.
static const char *vxs =
    "#version 330 core\n"
    "uniform mat4 MVP;" // Model-View-Projection Matrix ("Camera")
    "in vec3 vPos;"     // Input: Position of the vertex in 3D space.
    "in vec2 tPos;"     // Input: Position of the vertex in the texture.
    "out vec2 texcoord;"  // Output: Position of the vertex in the texture.
    "void main()"       // Shader: Calculate screen coordinates of the
    "{"                 // vertex from the 3D position.
    "   gl_Position = MVP * vec4(vPos, 1);"
    "   texcoord    = tPos;" // Texture coordinates are just passed through unchanged.
    "}";

//------------------------------------------------------------------
// The fragment shader of the textured triangles to draw.
static const char *fts =
    "#version 330 core\n"
    "uniform sampler2D TEX;" // Defines which texture to use.
    "in vec2 texcoord;"     // Input: The texture coordinates of the pixel.
    "out vec4 FragColor;"   // Output: The calculated color of the pixel.
    "void main()" // Shader: Look up the color of the pixel in the
    "{"           // texture bitmap.
    "    FragColor = vec4( texture2D( TEX, texcoord ) );"
    "}";

//------------------------------------------------------------------
GLuint compile_shader(GLenum type, const char * code )
{
    //------------------------------------------------------------------
    // A buffer for holding messages from compiling shaders.
    GLchar buffer[2048];
    GLsizei length;
    //------------------------------------------------------------------
    // Create a new shader object.
    GLuint shader_id = glCreateShader(type);
    // Load the shader in OpenGL
    glShaderSource( shader_id, 1, &code, NULL);
    // Compile the shader.
    glCompileShader( shader_id );
    // See if any errors occurred during compilation.
    glGetShaderInfoLog( shader_id, 2047, &length, buffer);
    if (length > 0)
    {
        std::cerr << "Compiling shader log: " << buffer << std::endl;
        exit(-1);
    }
    return shader_id;
}

//------------------------------------------------------------------
GLuint link_program( GLuint vxs_id, GLuint fts_id )
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
// Definition of a single vertex of the textured rectangle.
typedef struct
{
    GLfloat x, y, z;  // model coordinates
    GLfloat u, v;     // texture coordinates
} TexRectVertex;

//------------------------------------------------------------------
void Rectangle::init(const Texture * texture, GLfloat x, GLfloat y, GLfloat w, GLfloat h )
{
    //------------------------------------------------------------------
    tex = texture;
    //------------------------------------------------------------------
    // Create a new vertex shader object.
    GLuint shader_id_vxs = compile_shader( GL_VERTEX_SHADER, vxs );
    //------------------------------------------------------------------
    // Create a new fragment shader object.
    GLuint shader_id_fts = compile_shader( GL_FRAGMENT_SHADER, fts );
    //------------------------------------------------------------------
    program_id = link_program( shader_id_vxs, shader_id_fts );

    //======================================================================
    // Geometry relevant init.
    //------------------------------------------------------------------
    // Get locations of shader input variables and uniforms, for later reference.
    GLint loc_vPos = glGetAttribLocation( program_id, "vPos" );
    GLint loc_tPos = glGetAttribLocation( program_id, "tPos" );
    loc_TEX = glGetUniformLocation( program_id, "TEX"  );
    loc_MVP = glGetUniformLocation( program_id, "MVP"  );
    //------------------------------------------------------------------
    // Create a vertex attribute array and bind it.
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    //------------------------------------------------------------------
    // Create a new buffer and bind it for the vertex attribute array.
    GLuint vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    //------------------------------------------------------------------
    // Enable shader input "vPos" and describe its layout in the vertex buffer.
    glEnableVertexAttribArray(loc_vPos);
    glVertexAttribPointer( loc_vPos, 3, GL_FLOAT, GL_FALSE, sizeof(TexRectVertex), (void*)(0*sizeof(float)) /* <- 0 = offset of x in TexRectVertex */ );
    //------------------------------------------------------------------
    glEnableVertexAttribArray( loc_tPos);
    glVertexAttribPointer( loc_tPos, 2, GL_FLOAT, GL_FALSE, sizeof(TexRectVertex), (void*)(3*sizeof(float)) /* <- 3 = offset of u in TexRectVertex */ );
    //------------------------------------------------------------------
    // Define 4 vertices that make up a rectangle.
    float z=0;
    TexRectVertex vertices[4] =
    {
        { x,   y,   z,    0, 0 },
        { x+w, y,   z,    1, 0 },
        { x,   y+h, z,    0, 1 },
        { x+w, y+h, z,    1, 1 },
    };
    //------------------------------------------------------------------
    // Upload the vertices to the vertex buffer.
    glBufferData( GL_ARRAY_BUFFER, 4*sizeof(TexRectVertex), &vertices[0], GL_DYNAMIC_DRAW );
    //------------------------------------------------------------------
    // Unbind the vertex attribute array and the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //------------------------------------------------------------------
}

void Rectangle::render()
{
    //------------------------------------------------------------------
    // Activate the shader program
    glUseProgram(program_id);
    glUniform1i(loc_TEX, 0);     // Texture unit 0 is for base images
    tex->bind();
    glBindVertexArray(vertex_array_id);
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);
}

void Rectangle::update_screen(int width, int height)
{
    //------------------------------------------------------------------
    mat4x4 MVP;
    mat4x4_ortho( MVP, 0, width, height, 0, 1, -1 );
    //------------------------------------------------------------------
    glUseProgram( program_id );
    glUniformMatrix4fv( loc_MVP, 1, false, &MVP[0][0]);
}


//========================================================================
// Read the character set out of C64 ROM and prepare a texture image.
// The texture will be 16x16 characters = 128x128 pixels
// Each pixel is 1 byte in the texture image. (Instead of 1 bit in the character rom.)
void prepare_charset( uint8_t char_rom[], GLchar image[128][128] )
{
    // --------------------------------------------------------------
    // For each petscii character in the set...
    for( int petscii=0; petscii<256; petscii++)
    {
        // --------------------------------------------------------------
        // For each row in the character...
        for( int row=0; row<8; row++)
        {
            // --------------------------------------------------------------
            // For each pixel in the row...
            for( int bit=0; bit<8; bit++ )
            {
                // --------------------------------------------------------------
                // Mask to get the current bit from the char rom.
                int mask = 1<<bit;
                // Calculate the pixel coordinate in the texture image.
                int x = (petscii & 15) * 8  +  (7-bit);
                int y = (petscii >> 4) * 8  +  row;
                // --------------------------------------------------------------
                // Set or clear the pixel in the texture image.
                image[y][x] = ( (char_rom[ petscii*8 + row] & mask) == mask ) ? 0xFF : 0x00;
                // --------------------------------------------------------------
            }
        }
    }
}

void Graphics::init()
{
    GLchar image[128][128];
    auto chargen { utils::RM.load("roms/chargen") };
    prepare_charset( (uint8_t*)chargen.data(), image );
    texture.load( &image[0][0], 128, 128, GL_R8, GL_RED );
//    texture.load( chargen.data(), 64, 64, GL_RED, GL_RED );
    charset.init( &texture, 0, 0, 512, 512);
    screen.init();
}

void Graphics::render()
{
    //------------------------------------------------------------------
    // Disable depth test and face culling.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    //------------------------------------------------------------------
    // Define the background color
    glClearColor(0.1f, 0.3f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    //------------------------------------------------------------------
//    charset.render();
    screen.render();
}


} // End of namespace gfx.
