R"(
#version 460 core

uniform isampler2D TEX;        // character generator ROM.
uniform int background_color;  // global screen background color.
uniform ivec3 palette[16];      // The 16 colors.
uniform int charset;           // Selects which character set to use (0 or 1)

in vec2 texcoord;           // The interpolated texture coordinate.
in flat int fg_col_gs;      // The foreground color.
in flat int char_gs;        // The chaaracter to display.

out vec4 FragColor;         // The pixel output color.

void main()
{

   int bit  = int(texcoord.x) & 0x7;
   int col  = int(texcoord.y) & 0x7;
   int row  = char_gs + 256*charset;

   ivec2 tx = ivec2( col, row );
   int byte = texelFetch( TEX, tx, 0 ).r;

   float f  = float(((byte>>(7-bit))&1)) * 1.0f;

   vec4 fg_col = vec4( float(palette[fg_col_gs       ].r)/255, float(palette[fg_col_gs       ].g)/255, float(palette[fg_col_gs       ].b)/255, 1);
   vec4 bg_col = vec4( float(palette[background_color].r)/255, float(palette[background_color].g)/255, float(palette[background_color].b)/255, 1);

   FragColor = mix( bg_col, fg_col, f);

};

)"
