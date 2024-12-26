#version 330 core

precision highp float;

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;
in vec2 v_position;
in vec2 v_leftP;

uniform sampler2D u_texture;

void main() {
    float value = texture2D(u_texture, v_texCoord).x;
    if (value < 0.0) {
        FragColor = vec4(0.0, 0.0, -value, 1.0);
    } else {
        FragColor = vec4(value, 0.0, 0.0, 1.0);
    }
    // FragColor = abs(texture2D(u_texture, v_texCoord));
}
