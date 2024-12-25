#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

uniform vec2 u_stride;

out vec2 v_texCoord;
out vec2 v_position;
out vec2 v_leftP, v_rightP, v_topP, v_bottomP;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    v_texCoord = texCoord;
    v_position = 0.5 * position + 0.5;
    v_leftP = v_position - vec2(u_stride.x, 0.0);
    v_rightP = v_position + vec2(u_stride.x, 0.0);
    v_topP = v_position + vec2(0.0, u_stride.y);
    v_bottomP = v_position - vec2(0.0, u_stride.y);
}
