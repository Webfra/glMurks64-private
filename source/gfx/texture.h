#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <cassert>

//========================================================================
namespace gfx {

class Texture
{
public:
    void gl_Uniform( GLint location );          // glUniform1i()
    
    Texture &gen();                             // glGenTextures()
    
    Texture &activate( GLenum textureUnit );    // glActiveTexture - set given value
    Texture &activate();                        // glActiveTexture - reuse last set value
    
    Texture &bind( GLenum target );             // glBindTExture   - set given value
    Texture &bind();                            // glBindTexte     - reuse last set value

    Texture &iformat( GLint internalFormat = GL_RGB );  // set internalFormat for Image2D()
    Texture &size( GLsizei width, GLsizei height );     // set width and height for Image2D()
    Texture &format( GLint format = GL_RGB );           // set format for Image2D()
    Texture &type( GLint type = GL_UNSIGNED_BYTE );     // set type for Image2D()
    Texture &Image2D(const GLvoid * data);              // glTexImage2D()

    Texture &Pi( GLenum pname, GLint param );   // glTexParameteri()
    
    void unbind();                              // glBindTexture(0)

private:
    GLuint texture_name { GL_INVALID_VALUE };
    GLenum tex_unit { GL_TEXTURE0 };
    GLenum tex_target { GL_TEXTURE_2D };
    GLint  tex_level {0};
    GLint  tex_internalFormat { GL_RGB };
    GLsizei  tex_width {-1};
    GLsizei  tex_height {-1};
    GLint tex_border {0};
    GLint tex_format { GL_RGB };
    GLint tex_type { GL_UNSIGNED_BYTE };
};

} // End of namespace gfx

#endif // TEXTURE_H
