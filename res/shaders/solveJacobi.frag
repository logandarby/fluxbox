#version 330 core

// Solves the jacobi equation for the input field u_x and b

layout(location = 0) out vec4 FragColor;
in vec2 v_texCoord;

uniform float u_alpha, u_rBeta;
uniform sampler2D u_x, u_b; // Ax = b

void main() {
    vec4 xLeft = texture2D(u_x, v_texCoord - vec2(1, 0));
    vec4 xRight = texture2D(u_x, v_texCoord + vec2(1, 0));
    vec4 xTop = texture2D(u_x, v_texCoord + vec2(0, 1));
    vec4 xBottom = texture2D(u_x, v_texCoord - vec2(0, 1));
    vec4 bC = texture2D(u_b, v_texCoord);
    vec4 newColor = (xLeft + xRight + xBottom + xTop + u_alpha * bC) * u_rBeta;
    FragColor = vec4(newColor);
}
