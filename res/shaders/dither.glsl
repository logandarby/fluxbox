#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_texCoord;

void main() {
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    v_texCoord = texCoord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;

uniform sampler2D u_texture;

void main() {
    const oldpixel = texture(u_texture, v_texCoord);
    const newpixel = round(oldpixel / 255);
    FragColor = newpixel;
        // quant_error := oldpixel - newpixel
        // pixels[x + 1][y    ] := pixels[x + 1][y    ] + quant_error × 7 / 16
        // pixels[x - 1][y + 1] := pixels[x - 1][y + 1] + quant_error × 3 / 16
        // pixels[x    ][y + 1] := pixels[x    ][y + 1] + quant_error × 5 / 16
        // pixels[x + 1][y + 1] := pixels[x + 1][y + 1] + quant_error × 1 / 16
}
