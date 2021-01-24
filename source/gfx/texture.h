#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils.h"

#include <glad/glad.h>

//========================================================================
namespace gfx {

//========================================================================
// Simple abstaction layer over OpenGL Textures.
// Provides a fluent interface.
class Texture
{
public:
    //========================================================================
    Texture() = default;
    NO_COPY( Texture );
    NO_MOVE( Texture );
    virtual ~Texture() { del(); }

    operator GLuint() { return texture_name; }

    // glUniform1i() - provides the texture unit defined in activate() 
    // to the uniform sampler2D location given here. 
    void set_texture_unit( GLint location );          
    
    Texture &gen(); // glGenTextures()
    void del();     // glDeleteTextures()
    
    Texture &activate( GLenum textureUnit );    // glActiveTexture - set given value
    Texture &activate();                        // glActiveTexture - reuse last set value
    
    Texture &bind( GLenum target );             // glBindTexture   - set given target and bind to it.
    Texture &bind();                            // glBindTexte     - reuse last set target
    void unbind();                              // glBindTexture(0)

    // Functions to provide format information for glTexImage2D()
    Texture &iformat( GLint   internalFormat = GL_RGB ); // set internalFormat
    Texture &size   ( GLsizei width, GLsizei height );   // set width and height
    Texture &format ( GLint   format = GL_RGB );         // set format
    Texture &type   ( GLint   type = GL_UNSIGNED_BYTE ); // set type
    Texture &Image2D( const GLvoid * data);              // provide pixel data and call glTexImage2D()

    Texture &Pi( GLenum pname, GLint param );   // glTexParameteri()
    Texture &GenerateMipMap();

    GLsizei width()  { return tex_width;  }
    GLsizei height() { return tex_height; }

private:
    GLuint texture_name { 0 };
    GLenum tex_unit {  0 /* + GL_TEXTURE0 */ };
    GLenum tex_target { GL_TEXTURE_2D };
    
    // The following members store the values to be used by Image2D()
    GLint    tex_level  { 0 };
    GLint    tex_internalFormat { GL_RGB };
    GLsizei  tex_width  { 0 };
    GLsizei  tex_height { 0 };
    GLint    tex_border { 0 };
    GLint    tex_format { GL_RGB };
    GLint    tex_type   { GL_UNSIGNED_BYTE };
};

//========================================================================
} // End of namespace gfx

//========================================================================
#endif // TEXTURE_H
