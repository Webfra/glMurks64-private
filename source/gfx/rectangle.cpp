
#include "rectangle.h"
#include "graphics.h"
#include "linmath.h"
#include <glad/glad.h>

//========================================================================
namespace gfx {

//========================================================================
// A simple vertex shader for textured vertices.
static const char *vxs =
    "#version 330 core\n"
    "uniform mat4 MVP;" // Model-View-Projection Matrix ("Camera")
    "in vec3 vPos;"     // Input: Position of the vertex in 3D space.
    "in vec2 tPos;"     // Input: Position of the vertex in the texture.
    "out vec2 texcoord;"  // Output: Position of the vertex in the texture.
    "void main()"       // Shader: Calculate screen coordinates of the
    "{"                 // vertex from the 3D position.
    "   gl_Position = MVP * vec4(vPos, 1);"
    "   texcoord    = tPos;" // Texture coordinates are just passed through unchanged.
    "}";

//========================================================================
// The fragment shader of the textured triangles to draw.
static const char *fts =
    "#version 330 core\n"
    "uniform sampler2D TEX;" // Defines which texture to use.
    "in vec2 texcoord;"     // Input: The texture coordinates of the pixel.
    "out vec4 FragColor;"   // Output: The calculated color of the pixel.
    "void main()" // Shader: Look up the color of the pixel in the
    "{"           // texture bitmap.
    "    FragColor = vec4( texture2D( TEX, texcoord ) );"
    "}";

//========================================================================
// Definition of a single vertex of the textured rectangle.
typedef struct
{
    GLfloat x, y, z;  // model coordinates
    GLfloat u, v;     // texture coordinates
} TexRectVertex;

//========================================================================
void Rectangle::init(GLfloat x, GLfloat y, GLfloat w, GLfloat h )
{
    //------------------------------------------------------------------
    // Create a new vertex shader object.
    GLuint shader_id_vxs = compile_shader( GL_VERTEX_SHADER, vxs );
    //------------------------------------------------------------------
    // Create a new fragment shader object.
    GLuint shader_id_fts = compile_shader( GL_FRAGMENT_SHADER, fts );
    //------------------------------------------------------------------
    program_id = link_program( shader_id_vxs, shader_id_fts );

    //======================================================================
    // Geometry relevant init.
    //------------------------------------------------------------------
    // Get locations of shader input variables and uniforms, for later reference.
    GLint loc_vPos = glGetAttribLocation( program_id, "vPos" );
    GLint loc_tPos = glGetAttribLocation( program_id, "tPos" );
    loc_TEX = glGetUniformLocation( program_id, "TEX"  );
    loc_MVP = glGetUniformLocation( program_id, "MVP"  );
    //------------------------------------------------------------------
    // Create a vertex attribute array and bind it.
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    //------------------------------------------------------------------
    // Create a new buffer and bind it for the vertex attribute array.
    GLuint vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    //------------------------------------------------------------------
    // Enable shader input "vPos" and describe its layout in the vertex buffer.
    glEnableVertexAttribArray(loc_vPos);
    glVertexAttribPointer( loc_vPos, 3, GL_FLOAT, GL_FALSE, sizeof(TexRectVertex), (void*)(0*sizeof(float)) /* <- 0 = offset of x in TexRectVertex */ );
    //------------------------------------------------------------------
    glEnableVertexAttribArray( loc_tPos);
    glVertexAttribPointer( loc_tPos, 2, GL_FLOAT, GL_FALSE, sizeof(TexRectVertex), (void*)(3*sizeof(float)) /* <- 3 = offset of u in TexRectVertex */ );
    //------------------------------------------------------------------
    // Define 4 vertices that make up a rectangle.
    float z=0;
    TexRectVertex vertices[4] =
    {
        { x,   y+h, z,    0, 1 },
        { x+w, y+h, z,    1, 1 },
        { x,   y,   z,    0, 0 },
        { x+w, y,   z,    1, 0 },
    };
    //------------------------------------------------------------------
    // Upload the vertices to the vertex buffer.
    glBufferData( GL_ARRAY_BUFFER, 4*sizeof(TexRectVertex), &vertices[0], GL_DYNAMIC_DRAW );
    //------------------------------------------------------------------
    // Unbind the vertex attribute array and the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //------------------------------------------------------------------
}

//========================================================================
void Rectangle::render()
{
    //------------------------------------------------------------------
    // Activate the shader program
    glUseProgram(program_id);
    glUniform1i(loc_TEX, 0);     // Texture unit 0 is for base images
    tex.activate().bind();
    glBindVertexArray(vertex_array_id);
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);
}

//========================================================================
void Rectangle::update_screen(int width, int height)
{
    //------------------------------------------------------------------
    mat4x4 MVP;
    mat4x4_ortho( MVP, 0, width, height, 0, 1, -1 );
    //------------------------------------------------------------------
    glUseProgram( program_id );
    glUniformMatrix4fv( loc_MVP, 1, false, &MVP[0][0]);
}

//========================================================================
} // End of namespace gfx.
