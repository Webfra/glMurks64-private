#include "text_screen.h"
#include "graphics.h"
#include "utils.h"

#include <iostream>

namespace gfx {

//========================================================================
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

//========================================================================
GLuint link_program( GLuint vxs_id, GLuint fts_id, GLuint gms_id )
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
    if( gms_id != 0)
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

//========================================================================
void Graphics::init()
{
    // Prepare the texture data from the character ROM
    GLchar image[128][128];
    auto chargen { utils::RM.load("roms/chargen") };
    prepare_charset( (uint8_t*)chargen.data(), image );

    // Create an OpenGL texture from the image.
    charset.tex.gen().activate(0).bind(GL_TEXTURE_2D).size(128,128)
            .iformat(GL_R8).format(GL_RED).type(GL_UNSIGNED_BYTE)
            .Pi(GL_TEXTURE_WRAP_S, GL_CLAMP).Pi(GL_TEXTURE_WRAP_T, GL_CLAMP)
            .Pi(GL_TEXTURE_MIN_FILTER, GL_NEAREST).Pi(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
            .Image2D(&image[0][0])
            .unbind();
    charset.init( 532, 20, 512, 512);

    screen.init();
}

//========================================================================
void Graphics::render()
{
    //std::cout << "--- Start Renderloop ---" << std::endl;
    //------------------------------------------------------------------
    // Disable depth test and face culling.
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //------------------------------------------------------------------
    screen.render();
    charset.render();
}

//========================================================================
void Graphics::update_screen(int width, int height)
{
    //------------------------------------------------------------------
    glViewport(0, 0, width, height);
    charset.update_screen( width, height);
    screen.update_screen(width, height);
}

//========================================================================
} // End of namespace gfx.
