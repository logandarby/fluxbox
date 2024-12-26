#version 330 core

// Calulates the gradient of u_pressure and subtracts it from u_field

precision highp float;

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;
in vec2 v_position;
in vec2 v_leftP, v_rightP, v_topP, v_bottomP;

uniform float u_gridScale; // 1 / gridscale
uniform sampler2D u_pressure; // pressure field calculate the gradient
uniform sampler2D u_velocity; // field to subtract from

void main() {
    float pLeft  = texture2D(u_pressure, v_leftP).x;
    float pRight = texture2D(u_pressure, v_rightP).x;
    float bTop = texture2D(u_pressure, v_topP).x;
    float bBottom = texture2D(u_pressure, v_bottomP).x;
    vec2 gradient = vec2(pRight - pLeft, bTop - bBottom);
    FragColor = vec4(gradient, 0.0, 1.0);
    vec2 currentVelocity = texture2D(u_velocity, v_position).xy;
    FragColor = vec4(currentVelocity - gradient, 0.0, 1.0);
}
