#version 330 core

// Solves the jacobi equation for the input field u_x and b

precision highp float;

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;
in vec2 v_leftP, v_rightP, v_topP, v_bottomP;

uniform float u_alpha, u_rBeta;
uniform sampler2D u_x, u_centerValue; // Ax = b, solve for x

void main() {
    vec4 xLeft = texture2D(u_x, v_leftP);
    vec4 xRight = texture2D(u_x, v_rightP);
    vec4 xTop = texture2D(u_x, v_topP);
    vec4 xBottom = texture2D(u_x, v_bottomP);
    vec4 centerValue = texture2D(u_centerValue, v_texCoord);
    vec4 sol = (xLeft + xRight + xBottom + xTop + (u_alpha * centerValue)) * u_rBeta;
    FragColor = vec4(sol.xyz, 1.0);
}
