//========================================================================

#include "text_screen.h"
#include "graphics.h"
#include "utils.h"

#include <glm/glm.hpp>

#include <iostream>
namespace gfx {

//========================================================================
// C64 color table. Taken from the screenshot of the C64-wiki.com
// https://www.c64-wiki.com/wiki/color
// Note: The values in the table on the same site are different!
std::array<glm::ivec3, 16> color_table { {
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
void Graphics::init()
{
    constexpr int cols=40, rows=25;
    //------------------------------------------------------------------
    // Initialize the framebuffer.
    frame.init(384, 272);
    //------------------------------------------------------------------
    // Load the character generator ROM.
    auto chargen { utils::RM.load("roms/chargen") };
    //------------------------------------------------------------------
    // Initialize the border and text screen.
    screen.init( chargen, cols, rows, glm::vec2 { 32, 36 } );
    border.init( chargen,   48,   35, glm::vec2 {  0, -4 } );
    //------------------------------------------------------------------
    // Everything that renders to the framebuffer, must be 
    // adjusted to the framebuffer size.
    border.resize_screen ( frame.Rect.tex.width(), frame.Rect.tex.height() );
    screen.resize_screen ( frame.Rect.tex.width(), frame.Rect.tex.height() );
    //------------------------------------------------------------------
#if 1 // Put something on the screen - just for testing.
    int max_chars = rows*cols;
    uint8_t chars[max_chars*2];    // A buffer representing the text screen.
    uint8_t colrs[max_chars*2];    // A buffer representing the color memory.    
    for( int i=0; i<max_chars*2; i++ )
    {
        chars[i]=i; // 32 = Space character
        colrs[i]=14; // 14 = light blue color
    }
    border.set_bg_color( 14 );
    screen.set_bg_color( 6 );
    border.set_memories( chars, colrs );
    screen.set_memories ( chars, colrs );
#endif
}

//========================================================================
void Graphics::render()
{
    //------------------------------------------------------------------
    // RENDERING TO THE FRAMEBUFFER
    //------------------------------------------------------------------
    // Set up the framebuffer for rendering INTO it.
    frame.activate();
    glViewport( 0,0, frame.Rect.tex.width(), frame.Rect.tex.height() );
    //------------------------------------------------------------------
    // Disable depth test and face culling.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    //------------------------------------------------------------------
    border.render();
    screen.render();
    //------------------------------------------------------------------
    // RENDERING TO THE SCREEN
    //------------------------------------------------------------------
    // Deactivate the framebuffer to enable rendering to the screen.
    frame.deactivate();
    glViewport(0,0, m_Width, m_Height);
    //------------------------------------------------------------------
    //glClearColor( 0,0,0, 0.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    //------------------------------------------------------------------
    // Render the framebuffer to the screen.
    frame.render(); // Render the frame buffer on the screen.
}

//========================================================================
void Graphics::resize_screen(int width, int height)
{
    //------------------------------------------------------------------
    // Store the new screen size.
    m_Width = width;
    m_Height = height;
    //------------------------------------------------------------------
    // Framebuffer renders to the screen, so it must be adjusted to
    // the screen size.
    frame.resize_screen   ( m_Width, m_Height );
    //------------------------------------------------------------------
}

//========================================================================
} // End of namespace gfx.
