#version 330 core

// Advects the field u_field according to the velocity field u_velocity

layout(location = 0) out vec4 FragColor;
in vec2 v_position;
in vec2 v_texCoord;

uniform float u_deltaT;
uniform float u_gridScale; // 1 / gridscale
uniform float u_dissipation = 1;
uniform sampler2D u_velocity;
uniform sampler2D u_field;   // quantity to advect

void main() {
    // vec2 lastPos = v_texCoord - u_deltaT * u_gridScale * texture2D(u_velocity, v_texCoord).xy;
    vec2 currentVelocity = texture2D(u_velocity, v_texCoord).xy;
    vec2 lastPos = v_texCoord - u_deltaT * u_gridScale * currentVelocity;
    FragColor = u_dissipation * texture2D(u_field, lastPos);
}
