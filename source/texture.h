#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

//========================================================================
namespace gfx {

class Texture
{
public:
    void load( char * buffer, GLint width, GLint height,
               GLint internalformat, GLenum format );
    operator GLint() const { return texture_name; }
    void bind(GLenum target=GL_TEXTURE0) const {
        // Make sure Texture unit 0 is active.
        glActiveTexture( target );
        glBindTexture( GL_TEXTURE_2D, texture_name);
    }
private:
    GLuint texture_name { GL_INVALID_VALUE };
};

} // End of namespace gfx

#endif // TEXTURE_H
