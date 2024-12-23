#version 330 core

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;

uniform float u_scale;
uniform vec2 u_offset;
uniform sampler2D u_field;

void main() {
    float value = u_scale * texture2D(u_field, v_texCoord + u_offset).x;
    FragColor = vec4(value, 0, 0, 1);
}