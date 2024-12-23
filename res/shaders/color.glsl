#shader vertex
#version 330 core

layout(location = 0) in vec2 position;

uniform mat4 u_model = mat4(1.0);

void main() {
    gl_Position = u_model * vec4(position, 0.0, 1.0);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;

uniform vec3 u_color;

void main() {
    FragColor = vec4(u_color, 1.0);
}
