//========================================================================

#include "text_screen.h"
#include "graphics.h"
#include "utils.h"

#include <iostream>
namespace gfx {

//========================================================================
// C64 color table. Taken from the screenshot of the C64-wiki.com
// https://www.c64-wiki.com/wiki/color
// Note: The values in the table on the same site are different!
std::array<ivec3, 16> color_table { {
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

#if (DRAW_CHARSET)
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
#endif

//========================================================================
void Graphics::init()
{
    frame.init(384, 272);

    // Prepare the texture data from the character ROM
    GLchar image[128][128];
    auto chargen { utils::RM.load("roms/chargen") };

    // Initialize the text screen.
    vec2 pos { 32, 36 };
    screen.init( chargen, 40, 25, pos );

    vec2 opos { 0, 4 };
    overlay.init( chargen, 48, 33, opos);
    overlay.set_bg_color( 14);

#if 1
    int cols=40, rows=25, max_chars = rows*cols;
    uint8_t chars[max_chars];    // A buffer representing the text screen.
    uint8_t colrs[max_chars];    // A buffer representing the color memory.    

    //------------------------------------------------------------------
    // Clear the buffers.
    for( int i=0; i<max_chars; i++ )
    {
        chars[i]=i; // Space character
        colrs[i]=14; // light blue color
    }
    #if 1
    //------------------------------------------------------------------
    // Fill the screen with something visible
    for(int x=0;x<16;x++)
        for(int y=0;y<16;y++)
            chars[x+y*cols] = x+y*16;
    #endif

    screen.set_memories( chars, colrs );
#endif
    //overlay.set_memories( chars, colrs );

#if (DRAW_CHARSET)
    // Create an OpenGL texture from the image.
    prepare_charset( (uint8_t*)chargen.data(), image );
    charset.tex.gen().activate(0).bind(GL_TEXTURE_2D)
        .iformat(GL_R8).size(128,128).format(GL_RED).type(GL_UNSIGNED_BYTE).Image2D(&image[0][0])
        .Pi(GL_TEXTURE_WRAP_S, GL_CLAMP)
        .Pi(GL_TEXTURE_WRAP_T, GL_CLAMP)
        .Pi(GL_TEXTURE_MIN_FILTER, GL_LINEAR)
        .Pi(GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        .unbind();
    // Create the OpenGL Rectangle.
    charset.init( 20, 20, 128, 128);
#endif
}

//========================================================================
void Graphics::render()
{
    //------------------------------------------------------------------
    // Set up the framebuffer for rendering INTO it.
    frame.activate();
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    // Disable depth test and face culling.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    //------------------------------------------------------------------
    // Define the border color.
    glClearColor( color_table[14][0] / 255.0f, 
                  color_table[14][1] / 255.0f, 
                  color_table[14][2] / 255.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    overlay.render();
    screen.render();
    //charset.render();

    //------------------------------------------------------------------
    // Deactivate the framebuffer to enable rendering to the screen.
    frame.deactivate();
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    // Render the framebuffer to the screen.
    glViewport(0,0, m_Width, m_Height);
    glClearColor( 0,0,0, 0.0f);
    frame.render(); // Render the frame buffer on the screen.
}

//========================================================================
void Graphics::resize_screen(int width, int height)
{
    //------------------------------------------------------------------
    // Framebuffer renders to the screen, so it must be adjusted to
    // the screen size.
    m_Width = width;
    m_Height = height;
    frame.resize_screen   ( width, height );
    //------------------------------------------------------------------
    // Everything else renders to the framebuffer, so it must be 
    // adjusted to the framebuffer size.
#if(DRAW_CHARSET)
    charset.resize_screen ( frame.Rect.tex.width(), frame.Rect.tex.height() );
#endif
    screen.resize_screen  ( frame.Rect.tex.width(), frame.Rect.tex.height() );
    overlay.resize_screen ( frame.Rect.tex.width(), frame.Rect.tex.height() );
    //------------------------------------------------------------------
}

//========================================================================
} // End of namespace gfx.
