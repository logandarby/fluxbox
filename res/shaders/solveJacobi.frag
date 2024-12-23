#version 330 core

// Solves the jacobi equation for the input field u_x and b

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;
in vec2 v_leftP, v_rightP, v_topP, v_bottomP;

uniform float u_alpha, u_rBeta;
uniform sampler2D u_x, u_b; // Ax = b

void main() {
    vec4 xLeft = texture2D(u_x, v_leftP);
    vec4 xRight = texture2D(u_x, v_rightP);
    vec4 xTop = texture2D(u_x, v_topP);
    vec4 xBottom = texture2D(u_x, v_bottomP);
    vec4 bC = texture2D(u_b, v_texCoord);
    vec4 newColor = (xLeft + xRight + xBottom + xTop + (u_alpha * bC)) * u_rBeta;
    FragColor = vec4(newColor.xyz, 1.0);
}
