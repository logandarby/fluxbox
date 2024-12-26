#version 330 core

// Outputs the divergence of input field u_field in the red color channel

precision highp float;

layout(location = 0) out vec4 FragColor;

in vec2 v_leftP, v_rightP, v_topP, v_bottomP;
uniform sampler2D u_field; // field to calculate

void main() {
    vec4 xLeft      = texture2D(u_field, v_leftP);
    vec4 xRight     = texture2D(u_field, v_rightP);
    vec4 xTop       = texture2D(u_field, v_topP);
    vec4 xBottom    = texture2D(u_field, v_bottomP);
    float divergence = ((xLeft.x - xRight.x) + (xTop.y - xBottom.y));
    FragColor       = vec4(divergence, 0, 0, 1.0);
}
