#pragma once
//========================================================================

#include "gfx_utils.h"

//========================================================================

#include <vector>

//========================================================================
namespace gfx {

//========================================================================
// A class to handle compiling and linking shader programs.
class Shader 
{
public:
    //========================================================================
    Shader() = default;
    NO_MOVE( Shader );
    NO_COPY( Shader );
    virtual ~Shader() = default;
    //========================================================================
    // Provide a converstion that can be used in OpenGL calls.
    operator GLuint() { return prg_id; }
    //========================================================================
    void compile( GLenum type, const char * code );  // Compile a shader and store it.
    void link();    // Link all compiled shaders.
    //========================================================================
private:
    GLuint prg_id { 0 };
    std::vector<GLuint> shaders_ids; // List of compiled shaders IDs.
};

//========================================================================
} // End of namespace gfx

//========================================================================
// End of file.
//========================================================================
