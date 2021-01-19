R"(
#version 460 core

uniform mat4 MVP;           // Model-View-Projection Matrix (Camera)
uniform float scaling;
    
layout ( points ) in;       // Input: Each vertex is a point.
in int character_vs[];      // Input: the character to display.
in int fg_col_vs[];         // Input: the foreground color to use

layout ( triangle_strip, max_vertices = 4 ) out; // Output: a triangle strip of 4 vertices as output.
out vec2 texcoord;          // Output: The texture coordinate.
out flat int fg_col_gs;     // Output: The foreground color to use
out flat int char_gs;       // Output: The character to display.

// Emit a single vertex.
// x and y designate the corner of the character. Must be 0 or 1.
void emit_vertex(float x, float y)
{
    // The relative position of the vertex in pixels.
    vec2 rel = vec2(x,y) * scaling;

    // The absolute output position: Input + relative position
    gl_Position = MVP * ( gl_in[0].gl_Position + vec4(rel,0,0) );
    
    // The texture coordinates: select one of the 256 characters
    // Multiply by 8: The x-position reflects the bit in the byte from the
    // Character generator.
    // The y-position reflects the row in the character generator.
    char_gs = character_vs[0];
    texcoord = vec2(x,y) * 8.0f;
    
    // Just push the foreground color through to the fragment shader.
    fg_col_gs = fg_col_vs[0];
    
    EmitVertex();   // emit the outputs.
}

// For each screen coordinate given from the vertex shader,
// generate 4 vertices, one for each corner of the character to display.
void main()
{
    emit_vertex( 0, 1 );    // bottom-left
    emit_vertex( 1, 1 );    // bottom-right
    emit_vertex( 0, 0 );    // top-left
    emit_vertex( 1, 0 );    // top-right

    EndPrimitive();
};

)"
