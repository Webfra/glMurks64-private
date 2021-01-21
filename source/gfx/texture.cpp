
#include "texture.h"
namespace gfx {

    void Texture::gl_Uniform( GLint location )
    {
        glUniform1i( location, tex_unit );
    }
    Texture &Texture::gen()
    {
        glGenTextures(1, &texture_name);
        return *this;
    }
    Texture &Texture::activate()
    {
        glActiveTexture( GL_TEXTURE0 + tex_unit);
        return *this;
    }
    Texture &Texture::activate( GLenum textureUnit )
    {
        tex_unit = textureUnit;
        return activate();
    }
    Texture &Texture::bind()
    {
        glBindTexture( tex_target, texture_name );
        return *this;
    }
    Texture &Texture::bind( GLenum target )
    {
        tex_target = target;
        return bind();
    }
    Texture &Texture::iformat( GLint internalFormat /* = GL_RGB */ )
    {
        tex_internalFormat = internalFormat;
        return *this;
    }
    Texture &Texture::size( GLsizei width, GLsizei height )
    {
        tex_width  = width;
        tex_height = height;
        return *this;
    }
    Texture &Texture::format( GLint format /* = GL_RGB */ )
    {
        tex_format = format;
        return *this;
    }
    Texture &Texture::type( GLint type /* = GL_UNSIGNED_BYTE */ )
    {
        tex_type = type;
        return *this;
    }
    Texture &Texture::Image2D(const GLvoid * data)
    {
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
    Texture &Texture::Pi( GLenum pname, GLint param )
    {
        glTexParameteri( tex_target, pname, param );
        return *this;
    }
    Texture &Texture::GenerateMipMap()
    {
        glGenerateMipmap(tex_target);
        return *this;
    }
    void Texture::unbind()
    {
        glBindTexture( tex_target, 0 );
    }

} // End of namespace gfx
