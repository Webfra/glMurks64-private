
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

} // End of namespace gfx

//========================================================================
// End of file.
//========================================================================
