#include "texture.h"

namespace gfx {

void Texture::load( char * buffer, GLint width, GLint height, GLint internalformat, GLenum format )
{
    //------------------------------------------------------------------
    // Generate a new texture ID and make it the current 2D Texture object.
    glGenTextures( 1, &texture_name );
    glBindTexture( GL_TEXTURE_2D, texture_name );
    //------------------------------------------------------------------
    // Upload the texture data to OpenGL.
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height,
                                0, format, GL_UNSIGNED_BYTE, &buffer[0]);
    //------------------------------------------------------------------
    // Configure texture wrapping at the edges and filtering mode.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //------------------------------------------------------------------
    // Unbind the texture
    glBindTexture( GL_TEXTURE_2D, 0 );
}

} // End of namespace gfx
