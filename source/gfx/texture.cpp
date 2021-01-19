#include "texture.h"

namespace gfx {

#if 0

void Texture::load( char * buffer, GLenum unit, GLenum target, GLsizei width,
                    GLsizei height, GLint internalformat, GLenum format )
{
    //------------------------------------------------------------------
    // Store texture target for later binds.
    tex_unit = unit;
    tex_target = target;
    //------------------------------------------------------------------
    // Generate a new texture ID and make it the current 2D Texture object.
    glGenTextures( 1, &texture_name );
    glActiveTexture( GL_TEXTURE0 + tex_unit);
    glBindTexture( tex_target, texture_name );
    //------------------------------------------------------------------
    // Upload the texture data to OpenGL.
    glTexImage2D( tex_target, 0, internalformat, width, height,
                                0, format, GL_UNSIGNED_BYTE, &buffer[0]);
    //------------------------------------------------------------------
    // Configure texture wrapping at the edges and filtering mode.
    glTexParameteri(tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(tex_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(tex_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //------------------------------------------------------------------
    // Unbind the texture
    glBindTexture( tex_target, 0 );
}

#endif

} // End of namespace gfx
