#version 330 core

precision highp float;

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;

uniform sampler2D u_texture;

void main() {
    FragColor = texture2D(u_texture, v_texCoord);
}
