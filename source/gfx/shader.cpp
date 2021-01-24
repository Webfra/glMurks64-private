
#include "shader.h"

//========================================================================
namespace gfx {

//========================================================================
// Compile a shader and store its ID.
void Shader::compile( GLenum type, const char * code )  
{
    //------------------------------------------------------------------
    // A buffer for holding messages from compiling shaders.
    GLchar buffer[2048];
    GLsizei length;
    buffer[2047]=0;
    //------------------------------------------------------------------
    // Create a new shader object.
    auto shader_id = glCreateShader(type);
    //------------------------------------------------------------------
    // Load the shader in OpenGL
    glShaderSource( shader_id, 1, &code, NULL);
    //------------------------------------------------------------------
    // Compile the shader.
    glCompileShader( shader_id );
    //------------------------------------------------------------------
    // See if any errors occurred during compilation.
    glGetShaderInfoLog( shader_id, 2047, &length, buffer);
    if (length > 0)
    {
        std::cerr << "Compiling shader log: " << buffer << std::endl;
        exit(-1); // Rather throw an exception?
    }
    //------------------------------------------------------------------
    // Store the ID for linking the program.
    shaders_ids.emplace_back(shader_id);
    //------------------------------------------------------------------
}

//========================================================================
// Attach all shaders and link them to a shader program.
void Shader::link()
{
    //------------------------------------------------------------------
    // A buffer for holding messages from linking the program.
    GLchar buffer[2048];
    GLsizei length;
    buffer[2047]=0;
    //------------------------------------------------------------------
    // Create a new shader program object to hold the shaders.
    prg_id = glCreateProgram();
    //------------------------------------------------------------------
    // Attach all the previously compiled shaders.
    for( auto &shader_id: shaders_ids )
    {
        glAttachShader( prg_id, shader_id );
        // Mark the shader for deletion.
        glDeleteShader( shader_id );
    }
    //------------------------------------------------------------------
    // We don't need the shader IDs anymore.
    shaders_ids.clear();
    //------------------------------------------------------------------
    // Link the shader program.
    glLinkProgram( prg_id );
    //------------------------------------------------------------------
    // See if any errors occurred during linking.
    glGetProgramInfoLog( prg_id, 2047, &length, buffer);
    //------------------------------------------------------------------
    if (length > 0)
    {
        fprintf(stderr, "Linking Program log: %s", buffer);
        exit(-1); // Rather throw an exception?
    }
    //------------------------------------------------------------------
}

} // End of namespace gfx

//========================================================================
// End of file.
//========================================================================
