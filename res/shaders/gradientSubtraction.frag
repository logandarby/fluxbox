#version 330 core

// Calulates the gradient of u_pressure and subtracts it from u_field

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;
in vec2 v_leftP, v_rightP, v_topP, v_bottomP;

uniform float u_gridScale; // 1 / gridscale
uniform sampler2D u_pressure; // pressure field calculate the gradient
uniform sampler2D u_velocity; // field to subtract from

void main() {
    float pLeft  = texture2D(u_pressure, v_leftP).x;
    float pRight = texture2D(u_pressure, v_rightP).x;
    float bTop = texture2D(u_pressure, v_topP).x;
    float bBottom = texture2D(u_pressure, v_bottomP).x;
    vec4 u_newVelocity = texture2D(u_velocity, v_texCoord);
    u_newVelocity.xy -= u_gridScale * 0.5 * vec2(pRight - pLeft, bTop - bBottom);
}
