#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <cassert>

//========================================================================
namespace gfx {

class Texture
{
public:
    void load( char * buffer, GLenum unit, GLenum target, GLint width, GLint height,
               GLint internalformat, GLenum format );
    //operator GLint() const { return texture_name; }
    void bind() const {
        glActiveTexture( GL_TEXTURE0 + tex_unit );
        glBindTexture( tex_target, texture_name);
    }
    void gl_Uniform( GLint location )
    {
        glUniform1i( location, tex_unit );
    }
    Texture &gen()
    {
        glGenTextures(1, &texture_name);
        return *this;
    }
    Texture &unit( GLenum textureUnit = 0)
    {
        tex_unit = textureUnit;
        glActiveTexture( GL_TEXTURE0 + tex_unit);
        return *this;
    }
    Texture &bind( GLenum target )
    {
        tex_target = target;
        glBindTexture( tex_target, texture_name );
        return *this;
    }
    Texture &iformat( GLint internalFormat = GL_RGB )
    {
        tex_internalFormat = internalFormat;
        return *this;
    }
    Texture &size( GLsizei width, GLsizei height )
    {
        tex_width = width;
        tex_height = height;
        return *this;
    }
    Texture &format( GLint format = GL_RGB )
    {
        tex_format = format;
        return *this;
    }
    Texture &type( GLint type = GL_UNSIGNED_BYTE )
    {
        tex_type = type;
        return *this;
    }
    Texture &Image2D(const GLvoid * data)
    {
        assert( (tex_target == GL_TEXTURE_2D) );
        glTexImage2D( tex_target,
                      tex_level,
                      tex_internalFormat,
                      tex_width,
                      tex_height,
                      tex_border,
                      tex_format,
                      tex_type,
                      data);
        return *this;
    }
    Texture &Image1D(const GLvoid * data)
    {
        assert( (tex_target == GL_TEXTURE_1D)
                || (tex_target == GL_PROXY_TEXTURE_1D) );
        //glEnable(GL_TEXTURE_1D);
        glTexImage1D( tex_target,
                      tex_level,
                      tex_internalFormat,
                      tex_width,
                      tex_border,
                      tex_format,
                      tex_type,
                      data);
        return *this;
    }
    Texture &Pi( GLenum pname, GLint param )
    {
        glTexParameteri( tex_target, pname, param );
        return *this;
    }
    void unbind()
    {
        glBindTexture( tex_target, 0 );
    }
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
