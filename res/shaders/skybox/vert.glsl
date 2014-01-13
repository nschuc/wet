#version 150

uniform mat4 camera;
uniform mat4 model;

in vec3 vertex;
out vec3 texCoord;

void main() {
	texCoord = vertex;
	vec4 projected = camera * model * vec4(vertex, 1.0);
    gl_Position = projected;
}