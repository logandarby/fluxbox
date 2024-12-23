#version 330 core

// Outputs the divergence of input field u_field in the red color channel

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;

uniform float u_grid_scale; // 1 / gridscale
uniform sampler2D u_field; // field to calculate

void main() {
    vec4 xLeft  = texture2D(u_field, v_texCoord - vec2(1, 0));
    vec4 xRight = texture2D(u_field, v_texCoord + vec2(1, 0));
    vec4 xTop   = texture2D(u_field, v_texCoord + vec2(0, 1));
    vec4 xBottom= texture2D(u_field, v_texCoord - vec2(0, 1));
    float newColor = ((xLeft.x - xRight.x) + (xTop.y - xBottom.y)) * u_grid_scale * 0.5;
    FragColor = vec4(newColor, 0, 0, 1);
}
