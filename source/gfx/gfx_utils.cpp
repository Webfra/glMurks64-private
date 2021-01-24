
#include "gfx_utils.h"

#include <iostream>

//========================================================================
namespace gfx {

//========================================================================
// Extend the given rectangle "org" such, that it has the given
// target aspect ratio.
// MODIFIES org_rect!
void adjust_aspect(Rect2D<float> &org, float target_aspect)
{

    auto org_aspect { org.w / org.h };

    if( target_aspect > org_aspect )
    {
        auto new_w = target_aspect * org.h;
        org.x = (org.w - new_w) / 2.0f;
        org.w = new_w;
    }
    else
    {
        auto new_h = org.w / target_aspect;
        org.y = (org.h - new_h) / 2.0f;
        org.h = new_h;
    }
}

#if 0 // No longer used.
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

} // End of namespace gfx

//========================================================================
// End of file.
//========================================================================
