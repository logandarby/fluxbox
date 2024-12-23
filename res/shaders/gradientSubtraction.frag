#version 330 core

// Calulates the gradient of u_pressure and subtracts it from u_field

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;

uniform float u_grid_scale; // 1 / gridscale
uniform sampler2D u_pressure; // pressure field calculate the gradient
uniform sampler2D u_field; // field to subtract from

void main() {
    float pLeft  = texture2D(u_pressure, v_texCoord - vec2(1, 0)).x;
    float pRight = texture2D(u_pressure, v_texCoord + vec2(1, 0)).x;
    float bTop = texture2D(u_field, v_texCoord + vec2(0, 1)).x;
    float bBottom = texture2D(u_field, v_texCoord - vec2(0, 1)).x;
    vec4 u_newField = texture2D(u_field, v_texCoord);
    u_newField.xy -= u_grid_scale * 0.5 * vec2(pRight - pLeft, bTop - bBottom);
}
